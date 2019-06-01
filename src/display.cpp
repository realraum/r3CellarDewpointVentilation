/***********************************************************/

#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <stdint.h>
#include <math.h>
// Include the UI lib
#include "OLEDDisplayUi.h"

#include "display.h"
// Include custom images
#include "images.h"


SSD1306Wire  display(0x3c, 5, 4);

OLEDDisplayUi ui ( &display );

int screenW = 128;
int screenH = 64;
int oledCenterX = screenW/2;
int oledStartY = 16;
int oledCenterY = ((screenH-oledStartY)/2)+oledStartY;   // top yellow part is 16 px height
int oledRadius = 23;


extern float sensors_tempIn_;
extern float sensors_tempOut_;
extern float sensors_rhIn_;
extern float sensors_rhOut_;
extern float sensors_dpIn_;
extern float sensors_dpOut_;
extern uint32_t sensors_lastupdate_;
extern uint32_t venting_ventingTime_s_;
extern uint32_t venting_notVentingTime_s_;
extern uint32_t venting_last_change_s_;
extern bool venting_onoff_;


String twoDigits(int digits){
  if(digits < 10) {
    String i = '0'+String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}

void overlayVent(OLEDDisplay *display, OLEDDisplayUiState* state) {

}

void showSensorOK(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  uint32_t line=0;
  const uint32_t lh=25;
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  // display->drawStringMaxWidth(oledCenterX , oledStartY+lh*line++, "IN | OUT");
  display->drawStringMaxWidth(0 , oledStartY+lh*line++, screenW,String("IN: ")+(!isnan(sensors_dpIn_)?"Ok":"FAIL"));
  display->drawStringMaxWidth(0 , oledStartY+lh*line++, screenW,String("OUT: ")+(!isnan(sensors_dpOut_)?"Ok":"FAIL"));

}

void showFrameBME280Values(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  uint32_t line=0;
  const uint32_t lh=25;
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_24);
  // display->drawStringMaxWidth(oledCenterX , oledStartY+lh*line++, "IN | OUT");
  display->drawStringMaxWidth(oledCenterX , oledStartY+lh*line++, screenW,twoDigits((int)sensors_tempIn_) + "°C|"  + twoDigits((int)sensors_tempOut_)+"°C");  
  display->drawStringMaxWidth(oledCenterX , oledStartY+lh*line++, screenW,twoDigits((int)sensors_rhIn_) + "rH|"  + twoDigits((int)sensors_rhOut_)+"rH");  
}

void showFrameDewPointAndFan(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
  uint32_t line=0;
  const uint32_t lh=25;
  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawStringMaxWidth(0,oledStartY+lh*line++, screenW,(venting_onoff_)?"Fan ON":"Fan Off");
  display->drawStringMaxWidth(0,oledStartY+lh*line++, screenW,twoDigits((int)sensors_dpIn_) +((sensors_dpIn_<=sensors_dpOut_)?" <= ":" >= ")+ twoDigits((int)sensors_dpOut_));  
}

void showFrameFanRuntime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
  uint32_t line=0;
  const uint32_t lh=25;
  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawStringMaxWidth(0,oledStartY+lh*line++,screenW, "tOn: "+String(venting_ventingTime_s_)+"s");
  display->drawStringMaxWidth(0,oledStartY+lh*line++,screenW, "tOff:"+String(venting_ventingTime_s_)+"s");
}


//define Frames
FrameCallback frames[] = { showSensorOK, showFrameBME280Values, showFrameDewPointAndFan, showFrameFanRuntime};

// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { overlayVent };
int overlaysCount = 1;

uint32_t taskDisplay()
{
  return ui.update();
}

void initDisplay(void)
{
  ui.setTargetFPS(60);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);
  // first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);
  // transition that is used
  ui.setFrameAnimation(SLIDE_LEFT);
  // Add frames
  ui.setFrames(frames, frameCount);
  // Add overlays
  ui.setOverlays(overlays, overlaysCount);
  ui.init();

  display.flipScreenVertically();

}