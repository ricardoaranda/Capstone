/*
  ==============================================================================

    DynamicsProcessor.h
    Created: 16 Feb 2018 4:35:31pm
    Author:  Ricardo Aranda

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <math.h>

class DynamicsProcessor
{
public:
    DynamicsProcessor();
    ~DynamicsProcessor();
    
    float calcCompressorGain (float detector, float threshold, float ratio);
};
