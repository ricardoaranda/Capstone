
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobOhAudioProcessorEditor::OneKnobOhAudioProcessorEditor (OneKnobOhAudioProcessor& p,
                                                              AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState (vts)
{
    knob.setSliderStyle (Slider::RotaryVerticalDrag);
    knob.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    knob.setLookAndFeel (&otherLookAndFeel);
    addAndMakeVisible (knob);
    knobAttachment = new SliderAttachment (valueTreeState, "knob", knob);
    
    setSize (500, 400);
}

OneKnobOhAudioProcessorEditor::~OneKnobOhAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobOhAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void OneKnobOhAudioProcessorEditor::resized()
{
    knob.setBounds (getLocalBounds().reduced (10));
}
