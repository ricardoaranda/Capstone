
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "OtherLookAndFeel_V2.h"


//==============================================================================
/**
*/
class OneKnobDrumBusAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    OneKnobDrumBusAudioProcessorEditor (OneKnobDrumBusAudioProcessor&,
                                        AudioProcessorValueTreeState&);
    ~OneKnobDrumBusAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    Slider knob;
    OtherLookAndFeel_V2 otherLookAndFeel;
    
    ScopedPointer<SliderAttachment> knobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobDrumBusAudioProcessorEditor)
};
