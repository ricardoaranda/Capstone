
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "OtherLookAndFeel_V2.h"


//==============================================================================
/**
*/
class OneKnobOhAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    OneKnobOhAudioProcessorEditor (OneKnobOhAudioProcessor&,
                                   AudioProcessorValueTreeState&);
    ~OneKnobOhAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    Slider knob;
    ScopedPointer<SliderAttachment> knobAttachment;
    OtherLookAndFeel_V2 otherLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobOhAudioProcessorEditor)
};
