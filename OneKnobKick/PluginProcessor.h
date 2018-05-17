
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnvelopeDetector.h"
#include "DynamicsProcessor.h"

//==============================================================================
/**
*/
class OneKnobKickAudioProcessor  : public AudioProcessor, public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    OneKnobKickAudioProcessor();
    ~OneKnobKickAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void parameterChanged(const String &parameterID, float newValue) override;
    
    void updateCoefficients ();

private:
    AudioProcessorValueTreeState parameters;
    EnvelopeDetector detector;
    DynamicsProcessor processor;
    IIRFilter hpf, hpf2, lowShelf, lowNotch, midBell, highShelf, lpf;
    
    float knobValue;
    double sampleRateInFloat;
    double hpfRange, lpfRange;
    float lowShelfGain, lowNotchGain, midBellGain, highShelfGain;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobKickAudioProcessor)
};
