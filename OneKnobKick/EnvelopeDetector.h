/*
  ==============================================================================

    EnvelopeDetector.h
    Created: 15 Feb 2018 9:04:04pm
    Author:  Ricardo Aranda

  ==============================================================================
*/

#pragma once

#include <math.h>

class EnvelopeDetector
{
public:
    EnvelopeDetector();
    ~EnvelopeDetector();
    
    void initEnvelopeDetector (float);
    float getEnvelope (float inputSample, float attack, float release);
    
private:
    float envelope;
    float sampleRate;
};
