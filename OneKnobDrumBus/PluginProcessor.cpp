
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobDrumBusAudioProcessor::OneKnobDrumBusAudioProcessor()
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
    hpfRange = 100 - 40;
    lowBellGain = 1.778279410038923 - 1.0;  //  5dB
    midBellGain = 0.446683592150963 - 1.0;  // -7dB
    highShelfGain = 1.258925411794167 - 1.0; // 2dB
    
    parameters.createAndAddParameter ("knob", "Ratio", String(),
                                      NormalisableRange<float> (0.001f, 1.0f, 0.001f),
                                      0.001f, nullptr, nullptr);
    
    parameters.addParameterListener ("knob", this);
    
    parameters.state = ValueTree (Identifier("OneKnobDrumBusParameters"));
}

OneKnobDrumBusAudioProcessor::~OneKnobDrumBusAudioProcessor()
{
}

//==============================================================================
const String OneKnobDrumBusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobDrumBusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobDrumBusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobDrumBusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobDrumBusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobDrumBusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobDrumBusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobDrumBusAudioProcessor::setCurrentProgram (int index)
{
}

const String OneKnobDrumBusAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobDrumBusAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OneKnobDrumBusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRateInFloat = (float)sampleRate;
    detector.initEnvelopeDetector (sampleRateInFloat);
    
    hpf_left.reset();
    hpf2_left.reset();
    lowBell_left.reset();
    midBell_left.reset();
    highShelf_left.reset();
    
    hpf_right.reset();
    hpf2_right.reset();
    lowBell_right.reset();
    midBell_right.reset();
    highShelf_right.reset();
    
    updateCoefficients();
}

void OneKnobDrumBusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobDrumBusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OneKnobDrumBusAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    knobValue = *parameters.getRawParameterValue ("knob");
    
    // thresh: -36 | Ratio: 10:1 | outPutGain: 7.88 dB
    float compressedSample;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float envelope = detector.getEnvelope (channelData[sample], 2.24f, 50.0f);
            envelope = Decibels::gainToDecibels (envelope);
            
            float compressedGainFactor = processor.calcCompressorGain (envelope, (-36.0f) * knobValue, 10.0f);
            
            compressedSample = compressedGainFactor * channelData[sample];
            channelData[sample] += compressedSample * knobValue;
//            channelData[sample] *= 1.4125;
        }
    }
    auto* channelData = buffer.getWritePointer (0);
    hpf_left.processSamples (channelData, buffer.getNumSamples());
    hpf2_left.processSamples (channelData, buffer.getNumSamples());
    lowBell_left.processSamples (channelData, buffer.getNumSamples());
    midBell_left.processSamples (channelData, buffer.getNumSamples());
    highShelf_left.processSamples (channelData, buffer.getNumSamples());
    
    channelData = buffer.getWritePointer (1);
    hpf_right.processSamples (channelData, buffer.getNumSamples());
    hpf2_right.processSamples (channelData, buffer.getNumSamples());
    lowBell_right.processSamples (channelData, buffer.getNumSamples());
    midBell_right.processSamples (channelData, buffer.getNumSamples());
    highShelf_right.processSamples (channelData, buffer.getNumSamples());
}

//==============================================================================
bool OneKnobDrumBusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OneKnobDrumBusAudioProcessor::createEditor()
{
    return new OneKnobDrumBusAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OneKnobDrumBusAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobDrumBusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
void OneKnobDrumBusAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    updateCoefficients();
}
void OneKnobDrumBusAudioProcessor::updateCoefficients()
{
    knobValue = *parameters.getRawParameterValue ("knob");
    
    hpf_left.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 1.0));
    hpf2_left.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 1.0));
    lowBell_left.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 80, 1.25, (lowBellGain * knobValue) + 1.0));
    midBell_left.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 400.0, 2.15, (midBellGain * knobValue) + 1.0));
    highShelf_left.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 10000, 1.0, (highShelfGain * knobValue) + 1.0));
    
    hpf_right.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 1.0));
    hpf2_right.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 1.0));
    lowBell_right.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 80, 1.25, (lowBellGain * knobValue) + 1.0));
    midBell_right.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 400.0, 2.15, (midBellGain * knobValue) + 1.0));
    highShelf_right.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 10000, 1.0, (highShelfGain * knobValue) + 1.0));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobDrumBusAudioProcessor();
}
