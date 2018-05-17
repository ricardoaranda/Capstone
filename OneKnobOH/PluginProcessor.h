
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class OneKnobOhAudioProcessor  : public AudioProcessor,
                                 public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    OneKnobOhAudioProcessor();
    ~OneKnobOhAudioProcessor();

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
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;
    void updateCoefficients();
    
private:
    AudioProcessorValueTreeState parameters;
    IIRFilter hpf_left, hpf2_left, midBell_left, highShelf_left;
    IIRFilter hpf_right, hpf2_right, midBell_right, highShelf_right;
    IIRFilter leftReverbHighPass, rightReverbHighPass;
    Reverb reverb;
    Array<float> leftReverbBuffer, rightReverbBuffer;
    bool delayBufferResized;
    
    double hpfRange;
    float sampleRateInFloat, midBellGain, highShelfGain, knobValue;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobOhAudioProcessor)
};
