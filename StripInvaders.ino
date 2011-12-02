//StripInvader (c) 2011 Michael Vogt <michu@neophob.com> // pixelinvaders.ch
//

#include <SPI.h>
#include <Ethernet.h>
#include <ArdOSC.h>
#include "WS2801.h"

//*************************/
// OSC Stuff
#define OSC_MSG_SET_R "/knbr"
#define OSC_MSG_SET_G "/knbg"
#define OSC_MSG_SET_B "/knbb"

#define OSC_MSG_CHANGE_MODE "/mode"

//*************************/
// WS2801
//how many pixels
#define NR_OF_PIXELS 64
//output pixels
int dataPin = 2;       
int clockPin = 3;  

static uint8_t DELAY = 34;

//*************************/
// Network settings
byte myMac[] = { 0xAF, 0xFE, 0x00, 0xBE, 0x00, 0x01 };
byte myIp[]  = { 192, 168, 111, 222 };
int  serverPort  = 10000;

//*************************/
// Misc
#define MAX_NR_OF_MODES 2

WS2801 strip = WS2801(NR_OF_PIXELS, dataPin, clockPin);
int ledPin =  9;
uint8_t oscR, oscG, oscB, mode;
OSCServer server;

#define USE_SERIAL_DEBUG 1

/**
 *  SETUP
 */
void setup(){ 
 mode=0;
 Serial.begin(19200);
 
 Ethernet.begin(myMac ,myIp); 
 server.begin(serverPort);
 
 //set callback function
 server.addCallback(OSC_MSG_SET_R, &oscCallbackR); //PARAMETER: 1, float value 0..1
 server.addCallback(OSC_MSG_SET_G, &oscCallbackG); //PARAMETER: 1, float value 0..1
 server.addCallback(OSC_MSG_SET_B, &oscCallbackB); //PARAMETER: 1, float value 0..1
 server.addCallback(OSC_MSG_CHANGE_MODE, &oscCallbackChangeMode); //PARAMETER: None, just a trigger

 //init
 oscR = 255;
 oscG = 255;
 oscB = 255;
 
  //ws2801 start strips 
 strip.begin();

 setupLines();
 pinMode(ledPin, OUTPUT);  
 
 //we-are-ready indicator
 synchronousBlink();
 delay(50);
 synchronousBlink();
 delay(50);
 synchronousBlink();
 
#ifdef USE_SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println("Hello World!");
#endif
}

/**
 *  LOOP
 */  
void loop(){
/*  switch (mode) {
    case 0:
          loopLines();
          break;
    case 1:
          loopStars();    
          break;
  }
*/  
}

//convert a float value to a byte value
uint8_t getRgbValueFromFloat(float f) {
  f *= 255.0f;
  return byte(f);
}

//just blink
void synchronousBlink() {
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);  
}

//*************************/
// OSC callback
void oscCallbackR(OSCMessage *_mes){
    Serial.println("AAA");
    
  oscR = getRgbValueFromFloat( _mes->getArgFloat(0) );
  synchronousBlink();
  
#ifdef USE_SERIAL_DEBUG
  Serial.print("R: ");
  Serial.println(oscR);
#endif 
}

void oscCallbackG(OSCMessage *_mes){
    Serial.println("vVVVV");
  oscG = getRgbValueFromFloat( _mes->getArgFloat(0) );
  synchronousBlink();  

#ifdef USE_SERIAL_DEBUG
  Serial.print("G: ");
  Serial.println(oscG);
#endif
}

void oscCallbackB(OSCMessage *_mes){
      Serial.println("VVVVVVB");
  oscB = getRgbValueFromFloat( _mes->getArgFloat(0) );
  synchronousBlink();  
  
#ifdef USE_SERIAL_DEBUG
  Serial.print("B: ");
  Serial.println(oscB);
#endif  
}

void oscCallbackChangeMode(OSCMessage *_mes){
      Serial.println("AAAS");
  if (mode<MAX_NR_OF_MODES) {
    mode++;
  } else {
    mode = 0; 
  }
  
  switch (mode) {
    case 0:
          setupLines();
          break;
    case 1:
          setupStars();    
          break;
  }  

  synchronousBlink();
  
#ifdef USE_SERIAL_DEBUG
  Serial.print("mode: ");
  Serial.println(mode);
#endif  
  
}

//*************************/
// Helper Functions

//TODO: verify parameter Passing by value
void setTintPixelColor(uint16_t i, uint32_t c) {
  uint16_t b = c & 0xff;
  c >>= 8;
  uint16_t g = c & 0xff;
  c >>= 8;
  uint16_t r = c & 0xff;

  if (oscR = 255 && oscG == 255 && oscB == 255) {
    //no tint effect, no calculations needed
  } else {
    //apply tint effect and SWAP color according to real cabeling
    uint16_t rr=r;
    r = b*(oscR+1) >> 8;
    g = g*(oscG+1) >> 8;
    b = rr*(oscB+1) >> 8;
  }
  
  uint32_t tintCol = r & 255;
  tintCol <<= 8;
  tintCol |= g & 255;
  tintCol <<= 8;
  tintCol |= b & 255;
  strip.setPixelColor(i, tintCol);
}


// Create a 24 bit color value from R,G,B
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}
