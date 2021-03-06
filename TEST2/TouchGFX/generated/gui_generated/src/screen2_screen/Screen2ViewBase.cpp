/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

Screen2ViewBase::Screen2ViewBase()
{
    CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    box1.setPosition(0, 0, 480, 272);
    box1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));

    circle1.setPosition(34, 79, 104, 104);
    circle1.setCenter(52, 52);
    circle1.setRadius(47);
    circle1.setLineWidth(6);
    circle1.setArc(0, 360);
    circle1.setCapPrecision(180);
    circle1Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    circle1.setPainter(circle1Painter);

    circleProgress1.setXY(34, 79);
    circleProgress1.setProgressIndicatorPosition(0, 0, 104, 104);
    circleProgress1.setRange(0, 100);
    circleProgress1.setCenter(52, 52);
    circleProgress1.setRadius(47);
    circleProgress1.setLineWidth(4);
    circleProgress1.setStartEndAngle(0, 360);
    circleProgress1.setCapPrecision(180);
    circleProgress1Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 0));
    circleProgress1.setPainter(circleProgress1Painter);
    circleProgress1.setValue(60);

    textArea1.setXY(74, 119);
    textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 0));
    textArea1.setLinespacing(0);
    textArea1.setWildcard(TypedText(T_SINGLEUSEID2).getText());
    textArea1.resizeToCurrentText();
    textArea1.setTypedText(TypedText(T_SINGLEUSEID1));

    add(box1);
    add(circle1);
    add(circleProgress1);
    add(textArea1);
}

void Screen2ViewBase::setupScreen()
{

}
