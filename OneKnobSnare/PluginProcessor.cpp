/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobSnareAudioProcessor::OneKnobSnareAudioProcessor()
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
    hpfRange = 100.0 - 40.0;        // 40 Hz -> 100 Hz
    lowShelfGain = 1.135 - 1.0;     // 0 -> 1.1dB == 1.0 -> 2.24(gain factor)
    lowBellGain = 0.8 - 1.0;        // 0 -> -2db
    midBellGain = 0.4467 - 1.0;     // 0 -> -7dB
    
    parameters.createAndAddParameter ("knob", "Ratio", String(),
                                      NormalisableRange<float>(.001f, 1.0f, 0.001f),
                                      0.001f, nullptr, nullptr);
    
    parameters.addParameterListener ("knob", this);
    
    parameters.state = ValueTree (Identifier ("OneKnobSnareParameters"));
    
}

OneKnobSnareAudioProcessor::~OneKnobSnareAudioProcessor()
{
}

//==============================================================================
const String OneKnobSnareAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobSnareAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobSnareAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobSnareAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobSnareAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobSnareAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobSnareAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobSnareAudioProcessor::setCurrentProgram (int index)
{
}

const String OneKnobSnareAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobSnareAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OneKnobSnareAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRateInFloat = (float)sampleRate;
    detector.initEnvelopeDetector (sampleRateInFloat);
    
    hpf.reset();
    hpf2.reset();
    lowShelf.reset();
    lowBell.reset();
    midBell.reset();
    
    updateCoefficients();
}

void OneKnobSnareAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobSnareAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OneKnobSnareAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* channelData = buffer.getWritePointer (0);
    knobValue = *parameters.getRawParameterValue ("knob");
    
    float compressedSample = 1.0f;
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float compressorGainFactor = 1.0f;

//        float envelope = detector.getEnvelope (channelData[sample], 15.0f, 500.0f);
        float envelope = detector.getEnvelope (buffer.getRMSLevel (0, sample, 1), 15.0f, 500.0f);
        envelope = Decibels::gainToDecibels (envelope);

        compressorGainFactor = processor.calcCompressorGain (envelope, -17.0f, 5.0f);

        compressedSample = compressorGainFactor * channelData[sample];
        channelData[sample] = compressedSample;
    }
    
    hpf.processSamples (channelData, buffer.getNumSamples());
    hpf2.processSamples (channelData, buffer.getNumSamples());      // do I need another IIR object here?
    lowShelf.processSamples (channelData, buffer.getNumSamples());
    lowBell.processSamples (channelData, buffer.getNumSamples());
    midBell.processSamples (channelData, buffer.getNumSamples());
    
    buffer.copyFrom (1, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool OneKnobSnareAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OneKnobSnareAudioProcessor::createEditor()
{
    return new OneKnobSnareAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OneKnobSnareAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobSnareAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
void OneKnobSnareAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
     updateCoefficients();
}

void OneKnobSnareAudioProcessor::updateCoefficients()
{
    knobValue = *parameters.getRawParameterValue ("knob");
    
    hpf.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 0.85));
    hpf2.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40.0, 0.85));
    lowShelf.setCoefficients (IIRCoefficients::makeLowShelf ((double) sampleRateInFloat, 300.0, 7.4, (lowShelfGain * knobValue) + 1.0));
    lowBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 400.0, 2.15, (lowBellGain * knobValue) + 1.0));
    midBell.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 720.0, 6.0, (midBellGain * knobValue) + 1.0));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobSnareAudioProcessor();
}
