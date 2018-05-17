
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobOhAudioProcessor::OneKnobOhAudioProcessor()
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
    hpfRange = 1500 - 40;
    midBellGain = 0.5623 - 1.0;
    highShelfGain = 1.4962 - 1.0;
    
    delayBufferResized = false;
    
    parameters.createAndAddParameter ("knob", "Ratio", String(),
                                      NormalisableRange<float> (.001f, 1.0f, 0.001f),
                                      0.001f, nullptr, nullptr);
    
    parameters.addParameterListener ("knob", this);
    
    parameters.state = ValueTree (Identifier ("OneKnobOHParameters"));
    
    knobValue = *parameters.getRawParameterValue ("knob");
}

OneKnobOhAudioProcessor::~OneKnobOhAudioProcessor()
{
}

//==============================================================================
const String OneKnobOhAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobOhAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobOhAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneKnobOhAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneKnobOhAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneKnobOhAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneKnobOhAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneKnobOhAudioProcessor::setCurrentProgram (int index)
{
}

const String OneKnobOhAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneKnobOhAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void OneKnobOhAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRateInFloat = (float)sampleRate;
    
    hpf_left.reset();
    hpf2_left.reset();
    midBell_left.reset();
    highShelf_left.reset();
    
    hpf_right.reset();
    hpf2_right.reset();
    midBell_right.reset();
    highShelf_right.reset();
    
    reverb.reset();
    reverb.setSampleRate (sampleRate);
    
    leftReverbHighPass.reset();
    rightReverbHighPass.reset();
    
    updateCoefficients();
}

void OneKnobOhAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobOhAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OneKnobOhAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (!delayBufferResized)
    {
        leftReverbBuffer.resize (buffer.getNumSamples());
        rightReverbBuffer.resize (buffer.getNumSamples());
        
        delayBufferResized = true;
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        if (channel == 0)
        {
            hpf_left.processSamples (channelData, buffer.getNumSamples());
            hpf2_left.processSamples (channelData, buffer.getNumSamples());
            midBell_left.processSamples (channelData, buffer.getNumSamples());
            highShelf_left.processSamples (channelData, buffer.getNumSamples());
        }
        else if (channel == 1)
        {
            hpf_right.processSamples (channelData, buffer.getNumSamples());
            hpf2_right.processSamples (channelData, buffer.getNumSamples());
            midBell_right.processSamples (channelData, buffer.getNumSamples());
            highShelf_right.processSamples (channelData, buffer.getNumSamples());
        }
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if (channel == 0)
                leftReverbBuffer.set (sample, channelData[sample]);
            else if (channel == 1)
                rightReverbBuffer.set (sample, channelData[sample]);
        }
    }
    
//    auto* channelDataLeft = buffer.getWritePointer (0);
//    auto* channelDataRight = buffer.getWritePointer (1);
    auto* channelDataLeft = leftReverbBuffer.getRawDataPointer();
    auto* channelDataRight = rightReverbBuffer.getRawDataPointer();
    reverb.processStereo (channelDataLeft, channelDataRight, buffer.getNumSamples());
    
    leftReverbHighPass.processSamples (channelDataLeft, buffer.getNumSamples());
    rightReverbHighPass.processSamples (channelDataRight, buffer.getNumSamples());
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if (channel == 0)
                channelData[sample] += channelDataLeft[sample] * 0.5 * knobValue;
            else if (channel == 1)
                channelData[sample] += channelDataRight[sample] * 0.5 * knobValue;
        }
    }
    
}

//==============================================================================
bool OneKnobOhAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OneKnobOhAudioProcessor::createEditor()
{
    return new OneKnobOhAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OneKnobOhAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OneKnobOhAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
void OneKnobOhAudioProcessor::parameterChanged (const String& parameterID, float newValue)
{
    updateCoefficients();
}
void OneKnobOhAudioProcessor::updateCoefficients()
{
    knobValue = *parameters.getRawParameterValue ("knob");
    
    hpf_left.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40, 1.0));
    hpf2_left.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40, 1.0));
    midBell_left.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 500, 1.0, (midBellGain * knobValue) + 1.0));
    highShelf_left.setCoefficients (IIRCoefficients::makeHighShelf ((double)sampleRateInFloat, 8000.0, 0.3, (highShelfGain * knobValue) + 1.0));
    
    hpf_right.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40, 1.0));
    hpf2_right.setCoefficients (IIRCoefficients::makeHighPass ((double)sampleRateInFloat, (hpfRange * knobValue) + 40, 1.0));
    midBell_right.setCoefficients (IIRCoefficients::makePeakFilter ((double)sampleRateInFloat, 500, 1.0, (midBellGain * knobValue) + 1.0));
    highShelf_right.setCoefficients (IIRCoefficients::makeHighShelf ((double)sampleRateInFloat, 8000.0, 0.3, (highShelfGain * knobValue) + 1.0));
    
    leftReverbHighPass.setCoefficients(IIRCoefficients::makeLowPass ((double)sampleRateInFloat, 8000.0));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobOhAudioProcessor();
}
