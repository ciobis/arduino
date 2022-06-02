#define ONE_TIME_CLOCK_PIN 10
#define CYCLIC_CLOCK_PIN 11
#define OUTPUT_PIN 12
#define YELLOW_LED_PIN 2
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 6
#define RED_LED_PIN 8
#define YELLOW_LED 1
#define GREEN_LED 2
#define BLUE_LED 4
#define RED_LED 8
#define CYCLIC_PULSES_COUNT 5

class ClockPulse {
  public:
    ClockPulse(int clockTime, int indicators) {
      this->clockTime = clockTime;
      this->indicators = indicators;
    }

    virtual void doPulse() {
      if (clockTime > 0) {
        digitalWrite(OUTPUT_PIN, HIGH);
        delay(this->clockTime);
      }    
  
      if (clockTime > 0) {
        digitalWrite(OUTPUT_PIN, LOW);
        delay(this->clockTime);
      }
    }

    void activate() {
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      
      if (this->indicators & YELLOW_LED) {
        digitalWrite(YELLOW_LED_PIN, HIGH);
      }

      if (this->indicators & GREEN_LED) {
        digitalWrite(GREEN_LED_PIN, HIGH);
      }

      if (this->indicators & BLUE_LED) {
        digitalWrite(BLUE_LED_PIN, HIGH);
      }

      if (this->indicators & RED_LED) {
        digitalWrite(RED_LED_PIN, HIGH);
      }
    }
  
    int clockTime = 0;
    int indicators = 0;
    bool active = false;
};

class Button {
  public:
    Button(int pinNo) {
      this->pinNo = pinNo;
    }
    
    bool clicked() {
      bool currentValue = digitalRead(this->pinNo);
      bool result = currentValue && !lastValue;
      lastValue = currentValue;
      
      return result;
    }

  private:
    bool lastValue = false;
    int pinNo = 0;
};


void setup() {
  pinMode(ONE_TIME_CLOCK_PIN, INPUT);
  pinMode(CYCLIC_CLOCK_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

Button* singleClockButton = new Button(ONE_TIME_CLOCK_PIN);
Button* cyclicClockButton = new Button(CYCLIC_CLOCK_PIN);

ClockPulse* pulse1000 = new ClockPulse(500,  YELLOW_LED);
ClockPulse* pulse100 = new ClockPulse(100, GREEN_LED);
ClockPulse* pulse10 = new ClockPulse(10, BLUE_LED);
ClockPulse* pulse1 = new ClockPulse(1, RED_LED);
ClockPulse* noPulse = new ClockPulse(0, 0);

ClockPulse* singlePulse = new ClockPulse(300, YELLOW_LED | GREEN_LED | BLUE_LED | RED_LED);

ClockPulse* pulses[CYCLIC_PULSES_COUNT] = {
  noPulse, pulse1000, pulse100, pulse10, pulse1
};

int activePulseNo = 0;
void loop() {
  if (singleClockButton->clicked()) {
    singlePulse->activate();
    singlePulse->doPulse();
    
    activePulseNo = 0;
  }

  if (cyclicClockButton->clicked()) {
    activePulseNo += 1;
    if (activePulseNo >= CYCLIC_PULSES_COUNT) {
      activePulseNo = 1;
    }
  }

  ClockPulse* pulse = pulses[activePulseNo];
  pulse->activate();
  pulse->doPulse();
}
