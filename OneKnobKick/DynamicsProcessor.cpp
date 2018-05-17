/*
  ==============================================================================

    DynamicsProcessor.cpp
    Created: 16 Feb 2018 4:35:31pm
    Author:  Ricardo Aranda

  ==============================================================================
*/

#include "DynamicsProcessor.h"

DynamicsProcessor::DynamicsProcessor()
{
}

DynamicsProcessor::~DynamicsProcessor()
{
}

float DynamicsProcessor::calcCompressorGain(float detector, float threshold, float ratio)
{
    float CS = 1.0 -  1.0 / ratio;
    float gainFactor = CS * (threshold - detector);
    gainFactor = jmin (0.0f, gainFactor);
    
    return powf (10.0f, (gainFactor / 20.0f));
}

