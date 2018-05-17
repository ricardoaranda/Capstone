
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneKnobTomAudioProcessorEditor::OneKnobTomAudioProcessorEditor (OneKnobTomAudioProcessor& p,
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

OneKnobTomAudioProcessorEditor::~OneKnobTomAudioProcessorEditor()
{
}

//==============================================================================
void OneKnobTomAudioProcessorEditor::paint (Graphics& g)
{
//    g.fillAll (Colours::darkgrey.darker().darker().darker().darker());
//
//    g.setColour (Colours::white);
    g.fillAll(Colours::black);
}

void OneKnobTomAudioProcessorEditor::resized()
{
    knob.setBounds (getLocalBounds().reduced(10));
}
