/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobSnareAudioProcessorEditor::OneKnobSnareAudioProcessorEditor (OneKnobSnareAudioProcessor& p,
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

OneKnobSnareAudioProcessorEditor::~OneKnobSnareAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobSnareAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void OneKnobSnareAudioProcessorEditor::resized()
{
    knob.setBounds (getLocalBounds().reduced (10));
}
