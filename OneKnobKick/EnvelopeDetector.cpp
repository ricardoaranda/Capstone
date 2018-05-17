/*
  ==============================================================================

    EnvelopeDetector.cpp
    Created: 15 Feb 2018 9:04:04pm
    Author:  Ricardo Aranda

  ==============================================================================
*/

#include "EnvelopeDetector.h"

EnvelopeDetector::EnvelopeDetector()
{
}

EnvelopeDetector::~EnvelopeDetector()
{
}

void EnvelopeDetector::initEnvelopeDetector (float sr)
{
    sampleRate = sr;
    envelope = 0.0f;
}

float EnvelopeDetector::getEnvelope (float inputSample, float attack, float release)
{
    float temp = fabsf (inputSample);
    
    float attackCoef = expf (log (0.01) / (attack * sampleRate * 0.001));
    float releaseCoef = expf (log (0.01) / (release * sampleRate * 0.001));
    
    if (temp > envelope)
        envelope = attackCoef * (envelope - temp) + temp;
    else
        envelope = releaseCoef * (envelope - temp) + temp;
    
    return envelope;
}
