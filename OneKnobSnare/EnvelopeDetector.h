/*
  ==============================================================================

    EnvelopeDetector.h
    Created: 19 Feb 2018 6:50:10pm
    Author:  Aranda9

  ==============================================================================
*/

#pragma once

#include <math.h>

class EnvelopeDetector
{
public:
    EnvelopeDetector();
    ~EnvelopeDetector();
    
    void initEnvelopeDetector (float sampleRate);
    float getEnvelope (float inputSample, float attack, float release);
    
private:
    float envelope;
    float sampleRate;
};
