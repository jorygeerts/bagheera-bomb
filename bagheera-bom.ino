#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD is connected on SCL and SDA, which I guess 0x27 means?
LiquidCrystal_I2C lcd(0x27, 20, 4);

// The countdown is 3 hours
int armedCountDownSecs = 60 * 60 * 3;
int alertCountDownSecs = 93;
char countdownTxt[12];

// Time keeping
int ranSecs;
int curSec;

// Buttons (sorta)
int speedupBtn = A4;
int endBtn = A5;

int ledStart = 5;

const int ArmedStage = 1;
const int AlertStage = 2;
const int DisarmedStage = 3;

int ActiveStage = ArmedStage;

struct color {
  int r;
  int g;
  int b;
};

color yellow = color{r: 128, g: 128, b: 0};
color orange = color{r: 128, g: 254, b: 0};
color red = color{r: 254, g: 50, b: 50};
color purple = color{r: 50, g: 200, b: 100};

void setup()
{
  Serial.begin(9600);

  // Configure the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Configure RGB pins
  pinMode(ledStart+0, OUTPUT);
  pinMode(ledStart+1, OUTPUT);
  pinMode(ledStart+2, OUTPUT);
  pinMode(ledStart+3, OUTPUT);
  digitalWrite(ledStart+3, HIGH); // Common anode, so put a signal on the common port

  // Configure buttons
  pinMode(speedupBtn, INPUT);
  pinMode(endBtn, INPUT);

  // Setup initial display values
  setLcdTxt(0, "Apparaat gereed:");
  setColor(ledStart, yellow);

  Serial.println("Setup done");
}


void loop()
{
  switch (ActiveStage) {
    case ArmedStage:
      actArmed(speedupBtn, armedCountDownSecs, yellow, orange);
      break;
    case AlertStage:
      actArmed(endBtn, alertCountDownSecs, orange, red);
      break;
    case DisarmedStage:
      actDisarmed();
      break;
  }
  
  Serial.println();
  delay(100);
}

void actArmed(int progressOn, int countDown, color colSteady, color colFlash) {

  //  Check if progression is needed
  if (analogRead(progressOn) < 1000) {
    alertCountDownSecs = (millis() / 1000) + alertCountDownSecs;
    curSec = 0;

    setLcdTxt(0, "Apparaat ALARM");

    ActiveStage++;
    return;
  }

  Serial.print(" curSec: ");
  Serial.print(curSec);
  Serial.print(" ranSecs: ");
  Serial.print(ranSecs);

  ranSecs = millis() / 1000;

  //  If we're on the same second, do nothing
  Serial.print(" curSec: ");
  Serial.print(curSec);
  Serial.print(" ranSecs: ");
  Serial.print(ranSecs);
  
  if (curSec == ranSecs) {
    setColor(ledStart, colSteady);
    return;
  }

  curSec = ranSecs;

  Serial.print(" curSec: ");
  Serial.print(curSec);
  Serial.print(" ranSecs: ");
  Serial.print(ranSecs);  

  int secsTillFin = countDown - ranSecs;

  int runHours = secsTillFin/3600;
  int secsRemaining = secsTillFin%3600;
  int runMinutes = secsRemaining/60;
  int runSeconds = secsRemaining%60;

  sprintf(countdownTxt, "  %02d:%02d:%02d  ", runHours, runMinutes, runSeconds);
  setLcdTxt(1, countdownTxt);
  setColor(ledStart, colFlash);

  Serial.print(" A4: ");
  Serial.print(analogRead(speedupBtn));

  Serial.print(" A5: ");
  Serial.print(analogRead(endBtn));

}

void actDisarmed() {
  setColor(ledStart, purple);
  setLcdTxt(0, "Apparaat");
  setLcdTxt(1, "uitgeschakeld");

  delay(1000000);
}

void setLcdTxt(int line, char text[]) {
  for (int i = 0; i < strlen(text); i++ ) {
    lcd.setCursor(i, line);
    lcd.print(text[i]);
  }

  for (int i = strlen(text); i < 20; i++ ) {
    lcd.setCursor(i, line);
    lcd.print(" ");
  }
}

void setColor(int startPin, color c)
{
  analogWrite(startPin + 0, 255 - c.r);
  analogWrite(startPin + 1, 255 - c.g);
  analogWrite(startPin + 2, 255 - c.b);
}


