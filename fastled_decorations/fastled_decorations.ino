#include "FastLED.h"

#define MODES_COUNT 4
#define MODE_BUTTON_PIN 3
#define SPEED_PIN A3

/*
 * Color representation
 */
struct HSV {
   int hue;
   int saturation;
   int brightness;
};


/*
 * Class to abstract out FastLed library
 */
class Canvas {
  public:
    Canvas(int size) {
      this->size = size;
    }
  
    virtual void setPixel(int position, HSV color) = 0;
    virtual void show() = 0;
    int size = 0;
};

/*
 * FastLed implementation for canvas
 */
class FastLedCanvas: public Canvas {
  public:
    FastLedCanvas(int size): Canvas(size){
      FastLED.addLeds<NEOPIXEL, 12>(this->leds, size); 
    }
    
    void setPixel(int position, HSV color) {
      leds[position] = CHSV(color.hue, color.saturation, color.brightness);
    }

    void show() {
      FastLED.show();
    }

    CRGB leds[255];
};




/*
 * Class for abstracting light effects. Implemtations should only care about draw function
 */
class LightShow {
  public:
    virtual void draw(Canvas* canvas) = 0;
    void pause() {
      int value = analogRead(SPEED_PIN);
      if (value > 100) {
        value = 100;
      }
  
      int delayValue = minSpeed + (int)((maxSpeed - minSpeed) * value) / 100;
      delay(delayValue);
    }

  protected:
    int minSpeed = 10;
    int maxSpeed = 200;
};

class CyclingLightShow: public LightShow {
  public:
    void draw(Canvas* canvas) {
      for (int x = 0; x < canvas->size; x++) {
        canvas->setPixel(x, backgroundColor);
      }
    
      for (int x = 0; x < runningLength; x++) {
        int pos = position + x;
        if (pos >= canvas->size) {
          pos = pos - canvas->size;
        }

        canvas->setPixel(pos, runningColor);
      }
    
      position++;
      if (position == canvas->size) {
        position = 0;
      }
    }

  private:
    int position = 0;
    int runningLength = 18;
    HSV runningColor = {240, 200, 255};
    HSV backgroundColor = {0, 128, 255};
};



class RainbowShow: public LightShow {
  public:
    void draw(Canvas* canvas) {
      for (int x = 0; x < canvas->size; x++) {
        canvas->setPixel(x, {hue, 255, 255});
      }
    
      hue++;
    }

  private:
    unsigned char hue = 0;
};

class WhiteStrobeShow: public LightShow {
  public:
    void draw(Canvas* canvas) {
      HSV color = on ? colorOn : colorOff;
      on = !on;
      
      for (int x = 0; x < canvas->size; x++) {
        canvas->setPixel(x, color);
      }
    }

  private:
    HSV colorOn = {0, 0, 255};
    HSV colorOff = {0, 0, 0};
    bool on = true;
};

class RainbowStrobeShow: public LightShow {
  public:
    void draw(Canvas* canvas) {
      HSV color = colorOff;
      if (on) {
        color = {hue, 255, 255};
        hue++;
      }
      on = !on;
      
      for (int x = 0; x < canvas->size; x++) {
        canvas->setPixel(x, color);
      }
    }

  private:
    HSV colorOn = {0, 0, 255};
    HSV colorOff = {0, 0, 0};
    bool on = true;
    unsigned char hue = 0;
};



class Button {
  public:
    bool check() {
      bool currentValue = digitalRead(MODE_BUTTON_PIN);
      bool result = currentValue && !lastValue;
      lastValue = currentValue;
      
      return result;
    }

  private:
    bool lastValue = false;
};


CyclingLightShow cyclingShow;
RainbowShow rainbowShow;
WhiteStrobeShow whiteStrobeShow;
RainbowStrobeShow rainbowStrobeShow;
LightShow* modes[MODES_COUNT] = {
  &cyclingShow,
  &rainbowShow,
  &whiteStrobeShow,
  &rainbowStrobeShow
};

Canvas* canvas = new FastLedCanvas(36);
Button modeButton;
int currentMode = 0;

void setup() {}

void loop() {
  LightShow* currentShow = modes[currentMode];
  currentShow->draw(canvas);
  canvas->show();
  currentShow->pause();

  if (modeButton.check()) {
    currentMode++;
    if (currentMode == MODES_COUNT) {
      currentMode = 0;
    }
  }
}
