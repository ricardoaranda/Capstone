
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobKickAudioProcessorEditor::OneKnobKickAudioProcessorEditor (OneKnobKickAudioProcessor& p,
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

OneKnobKickAudioProcessorEditor::~OneKnobKickAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobKickAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void OneKnobKickAudioProcessorEditor::resized()
{
    knob.setBounds (getLocalBounds().reduced(10));
}

