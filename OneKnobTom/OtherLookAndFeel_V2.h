/*
 ==============================================================================
 
 OtherLookAndFeel.h
 Created: 15 May 2018 9:56:38pm
 Author:  Ricardo Aranda
 
 ==============================================================================
 */

#pragma once

class OtherLookAndFeel_V2: public LookAndFeel_V4
{
public:
    void drawRotarySlider (Graphics &g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider &slider) override
    {
        auto diameter = jmin (width, height);
        auto radius = diameter / 2;
        auto centerX = width / 2;
        auto centerY = height / 2;
        auto rectX = centerX - radius;
        auto rectY = centerY - radius;
        auto angle = rotaryStartAngle + (sliderPosProportional * (rotaryEndAngle - rotaryStartAngle));
        
        auto lineW = 9.98746f;
        auto arcRadius = radius - lineW * 3.11454f;
        
        /********************** BACKGROUND **********************/
        ColourGradient gradientBackground {Colour (144, 144, 114 - (sliderPosProportional * 70)),
            (centerX - (sliderPosProportional * 12.0f)), (centerY - (sliderPosProportional * 12.0f)),
            Colour (10, 10, 10), centerX + 115.903f, centerY + 115.217f,
            true};
        
        g.setGradientFill (gradientBackground);
        g.fillRect (Rectangle<int> (x, y, width, height));
        /********************************************************/
        
        /********************** OUTTER CIRCLE **********************/
        auto bounds = Rectangle<int> (rectX, rectY, diameter, diameter).toFloat().reduced (47);
        
        float gradientX = static_cast<float>(rectX) + 60.0f;
        float gradientY = static_cast<float>(rectY);
        float gradientX2 = static_cast<float>(centerX + radius - 35);
        float gradientY2 = static_cast<float>(centerY + radius - 45);
        
        g.setGradientFill ({Colour (61, 61, 61), gradientX, gradientY, Colours::black, gradientX2, gradientY2, false});
        g.fillEllipse (bounds);
        
        // outter reflection
        g.setGradientFill ({Colour (234, 234, 234 - (sliderPosProportional * 100)), gradientX, gradientY, Colour (0, 0, 0), gradientX2 - 190, gradientY2 - 86, false});
        g.drawEllipse (bounds, 2);
        
        // dial fill
        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc (centerX, centerY, arcRadius, arcRadius,
                                    0.0f, rotaryStartAngle, angle, true);
            
            g.setColour (Colour (254, 237, 19));
            g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }
        /********************************************************/
        
        /********************** INNER EDGE **********************/
        auto innerDiameter = diameter * 0.69;
        auto innerRadius = innerDiameter / 2;
        rectX = centerX - innerRadius;
        rectY = centerY - innerRadius;
        
        auto innerBounds = Rectangle<int> (rectX, rectY, innerDiameter, innerDiameter).toFloat();
        
        g.setColour (Colours::black);
        g.fillEllipse (innerBounds);
        /********************************************************/
        
        /********************** inner circle **********************/
        innerDiameter *= 0.97;
        innerRadius = innerDiameter / 2;
        rectX = centerX - innerRadius;
        rectY = centerY - innerRadius;
        
        innerBounds = Rectangle<int> (rectX, rectY, innerDiameter, innerDiameter).toFloat();
        
        gradientX = static_cast<float>(rectX) + 60.0f;
        gradientY = static_cast<float>(rectY);
        gradientX2 = static_cast<float>(centerX + innerRadius - 35);
        gradientY2 = static_cast<float>(centerY + innerRadius - 45);
        
        // dial
        g.setGradientFill ({Colour (81, 81, 81 - (sliderPosProportional * 5.6)), gradientX, gradientY, Colours::black, gradientX2, gradientY2, false});
        g.fillEllipse (innerBounds);
        
        /********************** TICK MARK **********************/
        Path tick;
        auto pointerLength = radius * 0.182605f;
        auto pointerThickness = 3.52427f;
        tick.addRoundedRectangle (-pointerThickness * 0.5f, -radius + 80, pointerThickness, pointerLength, 0.5f);
        tick.applyTransform (AffineTransform::rotation (angle).translated (centerX, centerY));
        
        g.setColour (Colours::yellow);
        g.fillPath (tick);
        
        // opaque layer
        ColourGradient innerGradient {Colour (81, 81, 81), gradientX, gradientY, Colours::black, gradientX2, gradientY2, false};
        innerGradient.multiplyOpacity (0.3);
        g.setGradientFill (innerGradient);
        g.fillEllipse (innerBounds);
        
        // reflection
        g.setGradientFill ({Colour (170, 170, 170 - (sliderPosProportional * 45)), gradientX, gradientY, Colour (0, 0, 0), gradientX2 - 40, gradientY2 - 40, false});
        g.drawEllipse (innerBounds, 2);
        /********************************************************/
        /********************************************************/
    }
};


