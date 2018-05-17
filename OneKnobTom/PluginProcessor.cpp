
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobTomAudioProcessor::OneKnobTomAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr)
#endif
{
    hpfRange = 97 - 40;             //
    lowBellGain = 0.2512 - 1.0;     //
    midBellGain = 0.4217 - 1.0;     //
    highBellGain = 0.73  - 1.0;     //
    
    parameters.createAndAddParameter ("knob", "Ratio", String(),
                                      NormalisableRange<float> (.001f, 1.0f, 0.001f),
                                      0.001f, nullptr, nullptr);
    
    parameters.addParameterListener ("knob", this);
    
    parameters.state = ValueTree (Identifier ("OneKnobTomParameters"));
}

OneKnobTomAudioProcessor::~OneKnobTomAudioProcessor()
{
}

//==============================================================================
const String OneKnobTomAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobTomAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobTomAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobTomAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobTomAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobTomAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobTomAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobTomAudioProcessor::setCurrentProgram (int index)
{
}

const String OneKnobTomAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobTomAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OneKnobTomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRateInFloat = (float)sampleRate;
    detector.initEnvelopeDetector (sampleRateInFloat);
    
    hpf.reset();
    lowBell.reset();
    midBell.reset();
    highBell.reset();
    
    updateCoefficients();
}

void OneKnobTomAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobTomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OneKnobTomAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* channelData = buffer.getWritePointer (0);
    knobValue = *parameters.getRawParameterValue ("knob");
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float compressorGainFactor = 1.0f;
        
        float envelope = detector.getEnvelope (channelData[sample], 24.0f, 210.0f);
        envelope = Decibels::gainToDecibels (envelope);
        
        compressorGainFactor = processor.calcCompressorGain (envelope, -22, 5);
        
        float compressedSample = compressorGainFactor * channelData[sample];
        channelData[sample] = compressedSample;
    }
    
    hpf.processSamples (channelData, buffer.getNumSamples());
    lowBell.processSamples (channelData, buffer.getNumSamples());
    midBell.processSamples (channelData, buffer.getNumSamples());
    highBell.processSamples (channelData, buffer.getNumSamples());
    
    buffer.copyFrom (1, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool OneKnobTomAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OneKnobTomAudioProcessor::createEditor()
{
    return new OneKnobTomAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OneKnobTomAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobTomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
void OneKnobTomAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    updateCoefficients();
}

void OneKnobTomAudioProcessor::updateCoefficients()
{
    hpf.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 1.0));
    lowBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 400.0, 1.0, (lowBellGain * knobValue) + 1.0));
    midBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 695, 2.10, (midBellGain * knobValue) + 1.0));
    highBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 2200, 1.8, (highBellGain * knobValue) + 1.0));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobTomAudioProcessor();
}
