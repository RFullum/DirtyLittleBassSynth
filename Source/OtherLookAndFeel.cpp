/*
  ==============================================================================

    OtherLookAndFeel.cpp
    Created: 27 Dec 2020 6:36:38pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OtherLookAndFeel.h"


/// Constructor
OtherLookAndFeel::OtherLookAndFeel() {}

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
    float tickWidth = 5.0f;
    
    Colour pluginBackground       = Colour( (uint8)125, (uint8)46,  (uint8)0   );
    Colour sectionColourLightBlue = Colour( (uint8)184, (uint8)210, (uint8)222 );
    // Rectangle the slider will go inside
    Rectangle<float> dialArea (radiusX, radiusY, diameter, diameter);
    
    g.setColour(pluginBackground);
    g.fillEllipse(dialArea);
    
    Path dialTick;      // Drawable path to draw the dial tick
    dialTick.addRectangle(0.0f, -radius, tickWidth, radius * 0.33f);
    
    g.setColour(sectionColourLightBlue);
    g.fillPath( dialTick, AffineTransform::rotation(tickAngle).translated(centerX, centerY) );
}

/*
void OtherLookAndFeel::drawLinearSlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                                        float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &slider)
{
    
}
*/
