#include <Wire.h>                  // This library allows you to communicate with I2C
#include <Adafruit_GFX.h>          // Adafruit GFX graphics core library
#include <Adafruit_SSD1306.h>      // Driver library for 'monochrome' 128x64 and 128x32 OLEDs

/*
  Showing number 0-9 on a Common Anode 7-segment LED display
  Displays the numbers 0-9 on the display, with one second inbetween.
    A
   ---
  F |   | B
  | G |
   ---
  E |   | C
  |   |
   ---
    D
  This example code is in the public domain.
*/

// Pin 2-8 is connected to the 7 segments of the display.
// pin 9 is for buzzer

#define pinA  2
#define pinB  3
#define pinC  4
#define pinD  5
#define pinE  6
#define pinF  7
#define pinG  8
#define BUZZER_PIN 9
#define D1  10
#define D2  11
#define D3  12
#define D4  13

// Define the PINS you're goint to use on your Arduino Nano
int controller1 = 0;               // ANALOG 0
int controller2 = 1;               // ANALOG 1
//int ledPin = 4;                    // DIGITAL 4
int btnPin = 13;                    // DIGITAL 13

// Define variables
int buttonState = 0;               // HIGH = Pressed
int gameState = 0;                 // 0 = Home, 1 = Game, 2 = End

int controllerValue1 = 0;          // variable to store the value coming from the potentiometer
int controllerValue2 = 0;          // variable to store the value coming from the potentiometer

int paddlePositionPlayer1 = 0;
int paddlePositionPlayer2 = 0;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

int ballX = 128 / 2;
int ballY = 64 / 2;
int ballSpeedX = 2;
int ballSpeedY = 1;

int maxScore;

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif



void setup() {
  pinMode(btnPin, INPUT);

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.clearDisplay();            // Status led on...
}

void loop()
{

  // Read controller value and calculate paddle position
  controllerValue1 = analogRead(controller1);
  controllerValue1 = map(controllerValue1, 0, 1024, 1024, 0);
  controllerValue2 = analogRead(controller2);
  controllerValue2 = map(controllerValue2, 0, 1024, 1024, 0);

  paddlePositionPlayer1 = controllerValue1 * (46.0 / 1023.0);
  paddlePositionPlayer2 = controllerValue2 * (46.0 / 1023.0);

  // Set button state
  buttonState = digitalRead(btnPin);

  if ((buttonState == HIGH  || controllerValue2 > 1000)&& (gameState == 0 || gameState == 2)) {
    gameState = 1;
    scorePlayer1 = 0;
    scorePlayer2 = 0;
    delay(100);
  } else if (buttonState == HIGH && (gameState == 1 || gameState == 2)) {
    gameState = 0;
    scorePlayer1 = 0;
    scorePlayer2 = 0;
    ballX = 128 / 2;
    ballY = 64 / 2;
    delay(100);
  }

  if (gameState == 0) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 18);
    display.println("PONG");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(32, 38);
    display.println("press start");
    display.setCursor(39, 50);
    display.print("goals: ");
    maxScore = map(controllerValue1, 0, 1024, 1, 30);
    display.println(maxScore);
    display.display();
    display.clearDisplay();
    ballSpeedX = 1;
    // play menu theme (totally uncopyrighted)
    tone(BUZZER_PIN, 510 ,100);  
    delay ( 450);  
    tone(BUZZER_PIN, 380 ,100);  
    delay ( 400);  
    tone(BUZZER_PIN, 320 ,100);  
    delay ( 500);  
    tone(BUZZER_PIN, 440 ,100);  
    delay ( 300);  
    tone(BUZZER_PIN, 480 ,80);  
    delay ( 330);  
    tone(BUZZER_PIN, 450 ,100);  
    delay ( 150);  
    tone(BUZZER_PIN, 430 ,100);  
    delay ( 300);  
    tone(BUZZER_PIN, 380 ,100);  
    delay ( 200);  
    tone(BUZZER_PIN, 660 ,80);  
    delay ( 200);  
    tone(BUZZER_PIN, 760 ,50);  
    delay ( 150);  
    tone(BUZZER_PIN, 860 ,100);  
    delay ( 300);  
    tone(BUZZER_PIN, 700 ,80);  
    delay ( 150);  
    tone(BUZZER_PIN, 760 ,50);  
    delay ( 350);  
    tone(BUZZER_PIN, 660 ,80);  
    delay ( 300);  
    tone(BUZZER_PIN, 520 ,80);  
    delay ( 150);  
    tone(BUZZER_PIN, 580 ,80);  
    delay ( 150);  
    tone(BUZZER_PIN, 480 ,80);  
    delay ( 500);
  }

  if (gameState == 1) {
    int cifraSx, cifraDx;
    cifraSx = scorePlayer1 / 10;
    cifraDx = scorePlayer1 % 10;
    ScriviNumeri(0, cifraSx);
    
    drawField(scorePlayer1, scorePlayer2);
    
    
    
    cifraSx = scorePlayer2 / 10;
    
    collisionControl();
    ScriviNumeri(1, cifraDx);
    drawBall();

    cifraDx = scorePlayer2 % 10;
    ScriviNumeri(2, cifraSx);
    display.display();

    ScriviNumeri(3, cifraDx);
    display.clearDisplay();
  }

  if (gameState == 2) {
    drawField(scorePlayer1, scorePlayer2);

    display.setTextSize(1);
    display.setTextColor(WHITE);

    if (scorePlayer1 == maxScore) {
      display.setCursor(15, 30);
    } else if (scorePlayer2 == maxScore) {
      display.setCursor(77, 30);
    }
    display.println("winner!");
    display.setCursor(65, 36);
    display.println("");
    display.display();
    display.clearDisplay();
  }

  if (gameState != 1)
  {
    digitalWrite(D1, 1);
    digitalWrite(D2, 1);
    digitalWrite(D3, 1);
    digitalWrite(D4, 1);
  }
}

void ScriviNumeri(int cifra, int val)
{
  digitalWrite(D1, 1);
  digitalWrite(D2, 1);
  digitalWrite(D3, 1);
  digitalWrite(D4, 1);
  if (cifra == 0)
    digitalWrite(D1, 0);
  if (cifra == 1)
    digitalWrite(D2, 0);
  if (cifra == 2)
    digitalWrite(D3, 0);
  if (cifra == 3)
    digitalWrite(D4, 0);

  switch (val)
  {
    case 0:
      Zero();
      break;
    case 1:
      Uno();
      break;
    case 2:
      Due();
      break;
    case 3:
      Tre();
      break;
    case 4:
      Quattro();
      break;
    case 5:
      Cinque();
      break;
    case 6:
      Sei();
      break;
    case 7:
      Sette();
      break;
    case 8:
      Otto();
      break;
    case 9:
      Nove();
      break;
  }
  delay(1);
}

void Zero()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 1);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, LOW);
}

void Uno()
{
  digitalWrite(pinA, 0);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 0);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 0);
  digitalWrite(pinG, LOW);

}
void Due()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 0);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 1);
  digitalWrite(pinF, 0);
  digitalWrite(pinG, 1);

}
void Tre()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 0);
  digitalWrite(pinG, 1);

}
void Quattro()
{
  digitalWrite(pinA, 0);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 0);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, 1);

}
void Cinque()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 0);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, 1);

}
void Sei()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 0);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 1);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, 1);

}
void Sette()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 0);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 0);
  digitalWrite(pinG, LOW);

}
void Otto()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 1);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, 1);

}
void Nove()
{
  digitalWrite(pinA, 1);
  digitalWrite(pinB, 1);
  digitalWrite(pinC, 1);
  digitalWrite(pinD, 1);
  digitalWrite(pinE, 0);
  digitalWrite(pinF, 1);
  digitalWrite(pinG, 1);

}

void drawField(int score1, int score2) {
  display.fillRect(0, round(paddlePositionPlayer1), 2, 18, 1);
  display.fillRect(126, round(paddlePositionPlayer2), 2, 18, 1);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(55, 0);
  if (score1>9)
    display.setCursor(51,0);
  display.print(score1);
  display.print(":");
  display.print(score2);

  display.fillRect(63, 12, 1, 5, 1);
  display.fillRect(63, 22, 1, 5, 1);
  display.fillRect(63, 32, 1, 5, 1);
  display.fillRect(63, 42, 1, 5, 1);
  display.fillRect(63, 52, 1, 5, 1);
  display.fillRect(63, 62, 1, 5, 1);
}

void collisionControl() {
  //bounce from top and bottom
  if (ballY >= 64 - 2 || ballY <= 0) {
    ballSpeedY *= -1;
  }

  //score points if ball hits wall behind player
  if (ballX >= 128 || ballX <= 0) {
    if (ballSpeedX > 0) {
      ballSpeedX = 1;
      scorePlayer1++;
      ballX = 128 / 4;
    }
    if (ballSpeedX < 0) {
      ballSpeedX = -1;
      scorePlayer2++;
      ballX = 128 / 4 * 3;
    }
    if (scorePlayer1 == maxScore || scorePlayer2 == maxScore) {
      gameState = 2;
    }
  }

  //bounce from player1
  if (((ballX >= 128 - 2 - 2 && ballX <= 128 - 2) || (ballX + ballSpeedX <= 2)) && ballSpeedX < 0) {
    // play player1 tone
    tone(BUZZER_PIN, 510 ,100);
    if (ballY > round(paddlePositionPlayer1) - 2 && ballY < round(paddlePositionPlayer1) + 18)
    {
      if (abs(ballSpeedX) == 8)
      {
        ballSpeedX *= -1;
      }
      else
        ballSpeedX *= -2;

      ballX = 4;
      if (ballY <= round(paddlePositionPlayer1) + 9)
      {
        if (ballY <= round(paddlePositionPlayer1) + 6)
          ballSpeedY = -2;
        else
          ballSpeedY = -1;
      }
      else
      {
        if (ballY <= round(paddlePositionPlayer1) + 12)
          ballSpeedY = 1;
        else
          ballSpeedY = 2;
      }
    }
  }
  //bounce from player2
  if (((ballX >= 128 - 2 - 2 && ballX <= 128 - 2) || (ballX + ballSpeedX >= 126)) && ballSpeedX > 0)
  {
    // play player2 tone
    tone(BUZZER_PIN, 380 ,100);
    if (ballY > round(paddlePositionPlayer2) - 2 && ballY < round(paddlePositionPlayer2) + 18)
    {
      if (abs(ballSpeedX) == 8)
      {
        ballSpeedX *= -1;
      }
      else
        ballSpeedX *= -2;

      ballX = 125;
      if (ballY <= round(paddlePositionPlayer2) + 9)
      {
        if (ballY <= round(paddlePositionPlayer2) + 4)
          ballSpeedY = -2;
        else
          ballSpeedY = -1;
      }
      else
      {
        if (ballY <= round(paddlePositionPlayer2) + 13)
          ballSpeedY = 1;
        else
          ballSpeedY = 2;
      }
    }

  }
}

void drawBall() {

  display.fillRect(ballX, ballY, 2, 2, 1);
  Serial.println(ballSpeedY);
  ballX += ballSpeedX;
  ballY += ballSpeedY;
}

