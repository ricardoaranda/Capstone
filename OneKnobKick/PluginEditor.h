
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "OtherLookAndFeel_V2.h"


//==============================================================================
/**
*/
class OneKnobKickAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::Listener Listener;
    
    OneKnobKickAudioProcessorEditor (OneKnobKickAudioProcessor&, AudioProcessorValueTreeState&);
    ~OneKnobKickAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    
    Slider knob;
    ScopedPointer<SliderAttachment> knobAttachment;
    ScopedPointer<Listener> knobListener;
    OtherLookAndFeel_V2 otherLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobKickAudioProcessorEditor)
};
