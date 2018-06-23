#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            0

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      32

#define DELAY 50
int wait;

byte pixelarray[NUMPIXELS * 3];

byte fnidx = 0;
#define MAXFNIDX  0

enum STATE{
  INIT,
  RUN,
  NONE
};

STATE state;
Adafruit_NeoPixel pixels;
void setup() {
  // put your setup code here, to run once:
  pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  state = INIT;

  fnidx = 0;
  pinMode(1, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(state == INIT) {
    switch(fnidx) {
      case 0:
        func1init();
        break;
    }
  }
  if(state == RUN) {
    switch(fnidx) {
      case 0:
        func1main();
        break;
    }
    writeArray();
  }

}

void resetArray() {
  for(byte i = 0; i < NUMPIXELS * 3; i++) {
    pixelarray[i] = 0;
  }
}

void writeArray() {
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(
      pixelarray[3*i],
      pixelarray[3*i+1],
      pixelarray[3*i+2]
      ));
  }
  pixels.show();
}

#define NUMFUN1LEDS 4
#define BRIGHTNESS 150

byte pos[NUMFUN1LEDS];

void func1init() {
  resetArray();
  byte itv = NUMPIXELS / NUMFUN1LEDS;
  for(byte i = 0; i < NUMFUN1LEDS; i++) {
    pos[i] = i*itv;
    pixelarray[pos[i] * 3] = BRIGHTNESS;
  }

  state = RUN;
  
}

void func1main(){
  if(wait++ < DELAY)
    return;

  digitalWrite(1, HIGH);
  wait = 0;
  //dim each pixel
  for(byte i = 0; i < NUMPIXELS * 3; i++) {
    //for(byte c = 0; c < 3; c++) {
      if(pixelarray[i] > 0){
        if(pixelarray[i]-20 > pixelarray[i]) //overflow?
          pixelarray[i] = 0;
        else
          pixelarray[i]-=20;
      }      
    //}
  }

  //set our N pixels to good old brightness
  for(byte i = 0; i < NUMFUN1LEDS; i++) {
    pos[i]++;
    if(pos[i] > 31) 
      pos[i] = 0;
    pixelarray[pos[i] * 3] = BRIGHTNESS;
  }
  digitalWrite(1, LOW);
}

