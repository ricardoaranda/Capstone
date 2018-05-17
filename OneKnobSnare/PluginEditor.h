/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "OtherLookAndFeel_V2.h"


//==============================================================================
/**
*/
class OneKnobSnareAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    OneKnobSnareAudioProcessorEditor (OneKnobSnareAudioProcessor&,
                                      AudioProcessorValueTreeState&);
    ~OneKnobSnareAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    Slider knob;
    OtherLookAndFeel_V2 otherLookAndFeel;
    
    ScopedPointer<SliderAttachment> knobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobSnareAudioProcessorEditor)
};
