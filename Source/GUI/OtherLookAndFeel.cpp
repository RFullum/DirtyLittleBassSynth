/*
  ==============================================================================

    OtherLookAndFeel.cpp
    Created: 27 Dec 2020 6:36:38pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OtherLookAndFeel.h"


/// Constructor
OtherLookAndFeel::OtherLookAndFeel() :
    dialColor( Colour( (uint8)125, (uint8)46,  (uint8)0   ) ),
    tickColor( Colour( (uint8)184, (uint8)210, (uint8)222 ) ),
    backColor( Colour( (uint8)255, (uint8)255, (uint8)255 ) )
{}

/// Sets the dial and tick colors
void OtherLookAndFeel::setColors(Colour& dial, Colour& tick, Colour& back)
{
    dialColor = dial;
    tickColor = tick;
    backColor = back;
}

/**
 Customizing the drawRotarySlider() method from LookAndFeel_V4
 */
void OtherLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                                        float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
{
    // Local Variables
    float reducer  = 0.75f;
    float diameter = jmin(width * reducer, height * reducer);   // Lesser of two creates squre & circle from rect & ellipse
    float radius   = diameter * 0.5f;
    float centerX  = x + (width / 2.0f);     // Finds center offset by x value to actually center it
    float centerY  = y + (height / 2.0f);    // Ditto for centerY
    float radiusX  = centerX - radius;
    float radiusY  = centerY - radius;
    /*
     - The difference between rotaryEndAngle and rotaryStartAngle is the total distance travelable in radians.
     - Multiplying that by slider position says what percentage of the way around that distance you are.
     - Plus the initial rotaryStartAngle to account for the space between 0 and the rotaryStartAngle.
     */
    float tickAngle = rotaryStartAngle + ( sliderPos * (rotaryEndAngle - rotaryStartAngle) );
    float tickWidth = diameter * 0.15f;     // Ticks proportional to dial size 
    
    // Dial Mid Ring (Offsets the radius so it centers over Outer Ring)
    float diameter2 = diameter * 0.9f;
    float radiusX2  = radiusX + ( (diameter - diameter2) * 0.5f );
    float radiusY2  = radiusY + ( (diameter - diameter2) * 0.5f );
    
    // Dial Inner (Offsets the radius so it centers over Outer Ring)
    float diameter3 = diameter2 * 0.9f;
    float radiusX3  = radiusX + ( (diameter - diameter3) * 0.5f );
    float radiusY3  = radiusY + ( (diameter - diameter3) * 0.5f );
    
    // Rectangle the slider will go inside
    Rectangle<float> dialArea( radiusX, radiusY, diameter, diameter );
    Rectangle<float> dialArea2 ( radiusX2, radiusY2, diameter2, diameter2 );
    Rectangle<float> dialArea3 ( radiusX3, radiusY3, diameter3, diameter3 );
    
    g.setColour   ( backColor );
    g.fillEllipse ( dialArea  );
    
    g.setColour   ( tickColor );
    g.fillEllipse ( dialArea2 );
    
    g.setColour   ( dialColor );
    g.fillEllipse ( dialArea3 );
    
    // The radius of the inner dial area to put the tick directly on its edge
    float radius3 = diameter3 * 0.5f;
    
    Path dialTick;      // Drawable path to draw the dial tick
    dialTick.addEllipse ( 0.0f, -radius3 + 5.0f, tickWidth, tickWidth );
    
    g.setColour ( tickColor );
    g.fillPath  ( dialTick, AffineTransform::rotation( tickAngle ).translated( centerX, centerY ) );
}


/// Draws linear slider with a square thumb, only as wide as the track
void OtherLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                          : Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                 slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : (float) y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float) width * 0.5f,
                         slider.isHorizontal() ? (float) height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float) width * 0.5f,
                               slider.isHorizontal() ? (float) height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float) width * 0.5f,
                         slider.isHorizontal() ? (float) height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        //auto thumbWidth = getSliderThumbRadius (slider);

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            //g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
            g.fillRect( Rectangle<float> (static_cast<float> (trackWidth), 6.0f).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin (trackWidth, (slider.isHorizontal() ? (float) height : (float) width) * 0.4f);
            auto pointerColour = slider.findColour (Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                             jmax (0.0f, (float) y + (float) height * 0.5f - trackWidth * 2.0f),
                             trackWidth * 2.0f, pointerColour, 2);

                drawPointer (g, maxSliderPos - trackWidth,
                             jmin ((float) (y + height) - trackWidth * 2.0f, (float) y + (float) height * 0.5f),
                             trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, (float) x + (float) width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, 1);

                drawPointer (g, jmin ((float) (x + width) - trackWidth * 2.0f, (float) x + (float) width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}
