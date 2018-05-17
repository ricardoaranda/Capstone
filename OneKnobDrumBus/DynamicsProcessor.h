/*
 ==============================================================================
 
 DynamicsProcessor.h
 Created: 19 Feb 2018 6:50:29pm
 Author:  Aranda9
 
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

