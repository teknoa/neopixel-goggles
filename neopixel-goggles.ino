#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            0

#define BUTTONPIN      1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      32

#define DELAY 50
int wait;

byte pixelarray[NUMPIXELS * 3];

byte fnidx = 0;

int coloridx = 1;

#define MAXFNIDX  0

enum STATE{
  INIT,
  RUN,
  NONE
};

STATE state;
Adafruit_NeoPixel pixels;

byte buttonstate;


void setup() {
  // put your setup code here, to run once:
  pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  state = INIT;

  fnidx = 1;
  pinMode(1, INPUT);
  buttonstate = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(state == INIT) {
    switch(fnidx) {
      case 0:
        func1init();
        break;
      case 1:
        func2init();
        break;    }
  }
  if(state == RUN) {
    switch(fnidx) {
      case 0:
        func1main();
        break;
      case 1:
        func2main();
        break;
    }
    writeArray();
  }
  if(digitalRead(1) == HIGH && buttonstate == 0) {
    if(++coloridx - 3 == 0) {
      fnidx = 1 - fnidx;
      coloridx = 0;
    }
    buttonstate = 1;
    state = INIT;
  }
  if(digitalRead(1) == LOW && buttonstate == 1) {
    buttonstate = 0;
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

/**
 * Dims the pixels for one step
 * To go dark, call it multiple times
 */
void dimStep(byte step, byte minval) {
    //dim each pixel
  for(byte i = 0; i < NUMPIXELS * 3; i++) {
    //for(byte c = 0; c < 3; c++) {
      if(pixelarray[i] > minval){
        if(pixelarray[i]-step > pixelarray[i]  //overflow?
          || pixelarray[i]-step < minval)
          pixelarray[i] = minval;
        else
          pixelarray[i]-=step;
      }      
    //}
  }
}

#define NUMFUN1LEDS 4
#define BRIGHTNESS 100

byte pos[NUMFUN1LEDS];

void func1init() {
  resetArray();
  byte itv = NUMPIXELS / NUMFUN1LEDS;
  for(byte i = 0; i < NUMFUN1LEDS; i++) {
    pos[i] = i*itv;
    pixelarray[pos[i] * 3 + coloridx] = BRIGHTNESS;
  }

  state = RUN;
  
}

void func1main(){
  if(wait++ < DELAY)
    return;

  //digitalWrite(1, HIGH);
  wait = 0;

  dimStep(30, 0);

  //set our N pixels to good old brightness
  for(byte i = 0; i < NUMFUN1LEDS; i++) {
    pos[i]++;
    if(pos[i] > 31) 
      pos[i] = 0;
    pixelarray[pos[i] * 3 + coloridx] = BRIGHTNESS;
  }
  //digitalWrite(1, LOW);
}

int randomWait = 0;
int waitSetPixel = 0;

void func2init() {
  resetArray();

  // sets all pixels to same brightness
  for(byte i = 0; i < NUMPIXELS; i++) {
    pixelarray[i * 3 + coloridx] = BRIGHTNESS; // sets red one
  }
  writeArray();
  
  randomSeed(analogRead(0));

  randomWait = random(500);
  waitSetPixel = 0;
  state = RUN;
  
}


void func2main(){
  
  waitSetPixel++; // wait time for resetting a pixel to brightness
  wait++;         // wait time for this method to act
  
  if(wait < DELAY)
    return;

  //digitalWrite(1, HIGH);
  wait = 0;

  dimStep(2, 1);

  //digitalWrite(1, LOW);
  
  if(waitSetPixel++ < DELAY + randomWait)
    return;

  waitSetPixel = 0;
  randomWait = random(300);
  
  pixelarray[random(NUMPIXELS) * 3 + coloridx] = BRIGHTNESS; 
  
  
}
