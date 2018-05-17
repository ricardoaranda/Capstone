
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobDrumBusAudioProcessorEditor::OneKnobDrumBusAudioProcessorEditor (OneKnobDrumBusAudioProcessor& p,
                                                                        AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState (vts)
{
    knob.setSliderStyle (Slider::Rotary);
    knob.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    knob.setLookAndFeel (&otherLookAndFeel);
    addAndMakeVisible (knob);
    knobAttachment = new SliderAttachment (valueTreeState, "knob", knob);
    
    setSize (500, 400);
}

OneKnobDrumBusAudioProcessorEditor::~OneKnobDrumBusAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobDrumBusAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void OneKnobDrumBusAudioProcessorEditor::resized()
{
    knob.setBounds (getLocalBounds().reduced (10));
}
