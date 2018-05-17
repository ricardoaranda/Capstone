
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobKickAudioProcessor::OneKnobKickAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr)
#endif
{
    hpfRange = 60.0 - 40;           // 20 Hz -> 60 Hz
    lpfRange = 13000 - 20000;       // 20 kHz -> 13 kHz
    lowShelfGain = 2.24 - 1.0;     // 0 -> 6dB == 1.0 -> 1.99(gain factor)
    lowNotchGain = 0.0447 - 1.0;    // 0 -> -27db
    midBellGain = 0.1778 - 1.0;     // 0 -> -15dB
    highShelfGain = 2.24 - 1.0;     // 0 -> 7dB
    
    parameters.createAndAddParameter ("knob", "Ratio", String(),
                                      NormalisableRange<float> (0.001f, 1.0f, 0.001f),
                                      0.001f, nullptr, nullptr);
    
    knobValue = *parameters.getRawParameterValue ("knob");
    
    parameters.addParameterListener ("knob", this);     //
    
    parameters.state = ValueTree (Identifier("OneKnobKickParameters"));
}

OneKnobKickAudioProcessor::~OneKnobKickAudioProcessor()
{
}

//==============================================================================
const String OneKnobKickAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobKickAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobKickAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobKickAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobKickAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobKickAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobKickAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobKickAudioProcessor::setCurrentProgram (int index)
{
}

const String OneKnobKickAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobKickAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OneKnobKickAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    detector.initEnvelopeDetector (sampleRate);
    
    sampleRateInFloat = sampleRate;
    
    hpf.reset();
    hpf2.reset();
    lowShelf.reset();
    lowNotch.reset();
    midBell.reset();
    highShelf.reset();
    lpf.reset();
    
    updateCoefficients();
}

void OneKnobKickAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobKickAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OneKnobKickAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    knobValue = *parameters.getRawParameterValue ("knob");

//    float outputGain = powf (10.0f, outputGain / 20.0f);
    auto* channelData = buffer.getWritePointer (0);     // arg = leftChannel
    float compressedSample;
    
    hpf.processSamples (channelData, buffer.getNumSamples());
    hpf2.processSamples (channelData, buffer.getNumSamples());
    lowShelf.processSamples (channelData, buffer.getNumSamples());
    lowNotch.processSamples (channelData, buffer.getNumSamples());
    midBell.processSamples (channelData, buffer.getNumSamples());
    highShelf.processSamples (channelData, buffer.getNumSamples());
    lpf.processSamples (channelData, buffer.getNumSamples());
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float gainFactor = 1.0f;
        float envelope = detector.getEnvelope (channelData[sample], 50.33f, 1500.0f);
        envelope = Decibels::gainToDecibels (envelope);
        
        gainFactor = processor.calcCompressorGain (envelope, -24.0f, 17.0f);
        
        // TODO multiply by makeup gain
        compressedSample = gainFactor * channelData[sample] * knobValue * 1.14f;
        channelData[sample] += compressedSample;
    }
    
    buffer.copyFrom (1, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool OneKnobKickAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OneKnobKickAudioProcessor::createEditor()
{
    return new OneKnobKickAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OneKnobKickAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobKickAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

void OneKnobKickAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    updateCoefficients();
}

void OneKnobKickAudioProcessor::updateCoefficients()
{
    hpf.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 0.85));
    hpf2.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 0.85));
    lowShelf.setCoefficients (IIRCoefficients::makeLowShelf ((double)sampleRateInFloat, 120.0, 2.94, (lowShelfGain * knobValue) + 1.0));
    lowNotch.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 145.0, 5.0, (lowNotchGain * knobValue) + 1.0));
    midBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 440, 2.15, (midBellGain * knobValue) + 1.0));
    highShelf.setCoefficients (IIRCoefficients::makeHighShelf ((double)sampleRateInFloat, 7000, 1.0, (highShelfGain * knobValue) + 1.0));
    lpf.setCoefficients (IIRCoefficients::makeLowPass ((double)sampleRateInFloat, (lpfRange * knobValue) + 20000 , 1.0));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobKickAudioProcessor();
}
