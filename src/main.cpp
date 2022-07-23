//#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "rocket.h"
#include "brod1.h"
#include "bullet.h"
#include "enemyBullet.h"
#include "life.h"
#include "rover.h"
#include "earth.h"
#include "ex.h"
#include "ex2.h"
#include "ricon.h"
#include "back2.h"
#include "sens.h"
#include "boom.h"
#include "gameOver.h"
#include "pitches.h"

// fire buttons
#define BUTTON_A_PIN 13
#define BUTTON_B_PIN 12
// joystick
#define BUTTON_UP_PIN 22
#define BUTTON_DOWN_PIN 21
#define BUTTON_LEFT_PIN 2
#define BUTTON_RIGHT_PIN 17
#define BUTTON_CENTER_PIN 15
// Onboard buttons
#define BUTTON_1_PIN 0
#define BUTTON_2_PIN 35
// output LEDs
#define LED_RED_PIN 33
#define LED_GREEN_PIN 25
#define LED_BLUE_PIN 26
// Buzzer
#define BUZZER_PIN 27
#define BUZZER_CHANNEL 0
// custom colors
#define TFT_GREY 0x5AEB
#define TFT_LIGHTBLUE 0x2D18

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

int score = 0;
float bulletX[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};
float bulletY[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};

float energyBulletX[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};
float energyBulletY[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};

float rocketX[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};
float rocketY[10] = {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20};
float rocketSpeed = 0.22;
int rockets = 3;

int counter = 0;
int rocketCounter = 0;
int enemyCounter = 0;
int level = 1;
float x = 10;
float y = 20;

float enemyY = 18;
float enemyX = 170;
float enemySpeed = 0.1;

int debounceFireButton = 0;   // pressDebounce for fire
int debounceRocketButton = 0; // pressDebounce for rockets

float speed = 0.42;

int blinkTime = 0;

int enemyHealth = 50;
int maximumHealth = enemyHealth;
int lives = 4;
int liveIndex[4] = {0, 0, 0, 0};
int rocketIndex[3] = {0, 0, 0};

int fireTime = 100;
int fireCount = 0;
float enemyBulletSpeed = 0.42;
int rocketDamage = 8; // rocket damage

int debounceSoundButton = 0;
bool sound = 1; // sound on or off

int stage = 0; // stage 0=start screen,//stage 1=playing stage //stage 3=game over

float spaceX[30];
float spaceY[30];

void buttonWait(int buttonPin)
{
  int buttonState = 0;
  while (1)
  {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH)
    {
      return;
    }
  }
}

void setup(void)
{

  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);      // fire2 B
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);      // fire1 A
  pinMode(LED_GREEN_PIN, OUTPUT);           // led2
  pinMode(LED_RED_PIN, OUTPUT);             // led1
  pinMode(LED_BLUE_PIN, OUTPUT);            // led3
  pinMode(BUTTON_CENTER_PIN, INPUT_PULLUP); // stick
  pinMode(BUTTON_1_PIN, INPUT);             // LORA built in buttons
  pinMode(BUTTON_2_PIN, INPUT);
  digitalWrite(LED_BLUE_PIN, 1);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 240, 135, back2);

  for (int i = 0; i < 30; i++)
  {
    spaceX[i] = random(5, 235);
    spaceY[i] = random(18, 132);
  }

  buttonWait(BUTTON_A_PIN);
  digitalWrite(LED_BLUE_PIN, 0);
}

void restart()
{
  counter = 0;
  rocketCounter = 0;
  enemyCounter = 0;
  level = 1;
  x = 10;
  y = 20;
  enemyY = 18;
  enemyX = 170;
  enemySpeed = 0.1;

  rockets = 3;
  rocketDamage = 8;
  lives = 4;
  score = 0;
  rocketIndex[0] = 0;
  rocketIndex[1] = 0;
  rocketIndex[2] = 0;
  liveIndex[0] = 0;
  liveIndex[1] = 0;
  liveIndex[2] = 0;
  liveIndex[3] = 0;
  enemyY = 44;
  speed = 0.42;
  enemyHealth = 50;
  maximumHealth = enemyHealth;
  enemyBulletSpeed = 0.42;
  rocketSpeed = 0.22;

  for (int i = 0; i < 10; i++)
  {
    bulletX[i] = -20;
    energyBulletX[i] = -20;
    rocketX[i] = -20;
  }
}

void newLevel()
{
  level++;
  speed = speed + 0.05;
  enemyBulletSpeed = enemyBulletSpeed + 0.1;
  enemyHealth = 50 + (level * 5);
  maximumHealth = enemyHealth;
  enemySpeed = 0.05 + (0.035 * level);

  rockets = 3;
  rocketDamage = 8 + (level * 2);
  rocketSpeed = 0.22 + (level * 0.02);
  rocketIndex[0] = 0;
  rocketIndex[1] = 0;
  rocketIndex[2] = 0;

  enemyY = 44;

  for (int i = 0; i < 10; i++)
  {
    bulletX[i] = -20;
    energyBulletX[i] = -20;
    rocketX[i] = -20;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.print("Level " + String(level));
  tft.setCursor(0, 22, 2);

  tft.println("Enemy speed : " + String(enemySpeed));
  tft.println("Enemy health : " + String(enemyHealth));
  tft.println("Enemy bullet speed : " + String(enemyBulletSpeed));
  tft.println("Remaining lives: " + String(lives));
  tft.println("My speed : " + String(speed));
  tft.println("Rocket damage : " + String(rocketDamage));
  tft.println("Rocket speed : " + String(rocketSpeed));

  tft.pushImage(170, 5, 55, 54, earth[level - 1]);
  tft.pushImage(170, 61, 72, 72, sens);
  delay(2600);

  buttonWait(BUTTON_A_PIN); // wait until button a is pressed.............

  tft.fillScreen(TFT_BLACK);

  tft.drawLine(0, 16, 240, 16, TFT_LIGHTBLUE);
  tft.drawLine(0, 134, 240, 134, TFT_LIGHTBLUE);

  tft.setCursor(200, 0, 2);
  tft.print(score);

  tft.fillRect(120, 3, 70, 7, TFT_GREEN);
  tft.drawRect(119, 2, 72, 9, TFT_GREY);
}

void loop()
{

  if (stage == 0)
  {
    restart();
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 240, 135, back2);
    buttonWait(BUTTON_A_PIN); // wait until button a is pressed.............
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.print("Level " + String(level));
    tft.setCursor(0, 22, 2);

    tft.println("Enemy speed : " + String(enemySpeed));
    tft.println("Enemy health : " + String(enemyHealth));
    tft.println("Enemy bullet speed : " + String(enemyBulletSpeed));
    tft.println("Remaining lives: " + String(lives));
    tft.println("My speed : " + String(speed));
    tft.println("Rocket damage : " + String(rocketDamage));
    tft.println("Rocket speed : " + String(rocketSpeed));

    tft.pushImage(170, 5, 55, 54, earth[level - 1]);
    tft.pushImage(170, 61, 72, 72, sens);
    delay(1000);

    buttonWait(BUTTON_A_PIN); // wait until button a is pressed.............

    tft.fillScreen(TFT_BLACK);

    tft.drawLine(0, 16, 240, 16, TFT_LIGHTBLUE);
    tft.drawLine(0, 134, 240, 134, TFT_LIGHTBLUE);

    tft.setCursor(200, 0, 2);
    tft.print(score);

    tft.fillRect(120, 3, 70, 7, TFT_GREEN);
    tft.drawRect(119, 2, 72, 9, TFT_GREY);

    stage = 1;
  }

  if (stage == 1)
  {                                                   // playing stage
    if (digitalRead(BUTTON_DOWN_PIN) == 0 and y < 94) // Move down
      y = y + speed;

    if (digitalRead(BUTTON_UP_PIN) == 0 and y > 18) // Move up
      y = y - speed;

    if (digitalRead(BUTTON_RIGHT_PIN) == 0 and x < 125) // Move right
      x = x + speed;

    if (digitalRead(BUTTON_LEFT_PIN) == 0 and x > 0) // Move right
      x = x - speed;

    if (digitalRead(BUTTON_A_PIN) == 0) // fire button A button
    {
      if (debounceFireButton == 0)
      {
        debounceFireButton = 1;

        bulletX[counter] = x + 34;
        bulletY[counter] = y + 15;
        counter = counter + 1;
      }
    }
    else
      debounceFireButton = 0;

    if (digitalRead(BUTTON_B_PIN) == 0 && rockets > 0) // Rocket button B button
    {
      if (debounceRocketButton == 0)
      {
        debounceRocketButton = 1;
        rockets--;
        rocketX[rocketCounter] = x + 34;
        rocketY[rocketCounter] = y + 14;
        rocketCounter = rocketCounter + 1;
        rocketIndex[rockets] = -100;
        tft.fillRect(70 + (rockets * 14), 0, 8, 14, TFT_BLACK);
      }
    }
    else
      debounceRocketButton = 0;

    if (digitalRead(BUTTON_2_PIN) == 0) // button 35 , on and off sound
    {
      if (debounceSoundButton == 0)
      {
        debounceSoundButton = 1;
        sound = !sound;
      }
    }
    else
      debounceSoundButton = 0;

    for (int i = 0; i < 30; i++)
    { // drawStars..........................................
      tft.drawPixel(spaceX[i], spaceY[i], TFT_BLACK);
      spaceX[i] = spaceX[i] - 0.5;
      tft.drawPixel(spaceX[i], spaceY[i], TFT_GREY);
      if (spaceX[i] < 0)
      {
        tft.drawPixel(spaceX[i], spaceY[i], TFT_BLACK);

        spaceX[i] = 244;
      }
    }

    tft.pushImage(x, y, 49, 40, brod1);
    tft.pushImage(enemyX, enemyY, 55, 54, earth[level - 1]);

    for (int i = 0; i < 10; i++)
    { // firing bullets
      if (bulletX[i] > 0)
      {
        tft.pushImage(bulletX[i], bulletY[i], 8, 8, bullet);
        bulletX[i] = bulletX[i] + 0.6;
      }
      if (bulletX[i] > 240)
        bulletX[i] = -30;
    }

    for (int i = 0; i < 10; i++)
    { // firing rockets
      if (rocketX[i] > 0)
      {
        tft.pushImage(rocketX[i], rocketY[i], 24, 12, rocket);
        rocketX[i] = rocketX[i] + rocketSpeed;
      }
      if (rocketX[i] > 240)
        rocketX[i] = -30;
    }

    // delay(1);
    int tr = 0;

    for (int j = 0; j < 10; j++) // did my bullet hit enemy
    {
      if (bulletX[j] > enemyX + 20 && bulletY[j] > enemyY + 2 && bulletY[j] < enemyY + 52)
      {
        tft.pushImage(bulletX[j], bulletY[j], 12, 12, ex2);
        if (sound == 1)
        {
          tone(BUZZER_PIN, NOTE_C5, 12);
          noTone(BUZZER_PIN);
        }
        else
        {
          delay(12);
        }
        tft.fillRect(bulletX[j], bulletY[j], 12, 12, TFT_BLACK);
        bulletX[j] = -50;
        score = score + 1;
        tft.setCursor(200, 0, 2);
        tft.print(score);
        enemyHealth--;
        tr = map(enemyHealth, 0, maximumHealth, 0, 70);
        tft.fillRect(120, 3, 70, 7, TFT_BLACK);
        tft.fillRect(120, 3, tr, 7, TFT_GREEN);

        if (enemyHealth <= 0)
        {
          tft.pushImage(enemyX, enemyY, 55, 55, boom);
          tone(BUZZER_PIN, NOTE_E4, 100);
          tone(BUZZER_PIN, NOTE_D4, 80);
          tone(BUZZER_PIN, NOTE_G5, 100);
          tone(BUZZER_PIN, NOTE_C3, 80);
          tone(BUZZER_PIN, NOTE_F4, 280);
          noTone(BUZZER_PIN);
          delay(700);
          newLevel();
        }
        digitalWrite(LED_GREEN_PIN, 1);
        blinkTime = 1;
      }
    }

    for (int j = 0; j < 10; j++) // did my ROCKET hit enemy
    {
      if (rocketX[j] + 18 > enemyX && rocketY[j] > enemyY + 2 && rocketY[j] < enemyY + 52)
      {
        tft.pushImage(rocketX[j], rocketY[j], 24, 24, explosion);
        if (sound == 1)
        {
          tone(BUZZER_PIN, NOTE_C3, 40);
          noTone(BUZZER_PIN);
        }
        else
        {
          delay(40);
        }
        tft.fillRect(rocketX[j], rocketY[j], 24, 24, TFT_BLACK);
        // delay(30);

        rocketX[j] = -50;
        score = score + 12;
        tft.setCursor(200, 0, 2);
        tft.print(score);
        enemyHealth = enemyHealth - rocketDamage;
        tr = map(enemyHealth, 0, maximumHealth, 0, 70);
        tft.fillRect(120, 3, 70, 7, TFT_BLACK);
        tft.fillRect(120, 3, tr, 7, TFT_GREEN);

        if (enemyHealth <= 0)
        {
          tft.pushImage(enemyX, enemyY, 55, 55, boom);
          tone(BUZZER_PIN, NOTE_E4, 100);
          tone(BUZZER_PIN, NOTE_D4, 80);
          tone(BUZZER_PIN, NOTE_G5, 100);
          tone(BUZZER_PIN, NOTE_C3, 80);
          tone(BUZZER_PIN, NOTE_F4, 280);
          noTone(BUZZER_PIN);
          delay(700);
          newLevel();
        }
        digitalWrite(LED_GREEN_PIN, 0);
        blinkTime = 1;
      }
    }

    for (int j = 0; j < 10; j++) // Am I hit
    {
      if (energyBulletX[j] < x + 30 && energyBulletX[j] > x + 4 && energyBulletY[j] > y + 4 && energyBulletY[j] < y + 36)
      {
        energyBulletX[j] = -50;
        liveIndex[lives - 1] = -40;
        tft.fillRect((lives - 1) * 14, 0, 14, 14, TFT_BLACK);
        lives--;
        if (lives == 0)
        {
          tft.pushImage(x, y, 55, 55, boom);
          tone(BUZZER_PIN, NOTE_G4, 100);
          tone(BUZZER_PIN, NOTE_B4, 80);
          tone(BUZZER_PIN, NOTE_C5, 100);
          tone(BUZZER_PIN, NOTE_A4, 80);
          tone(BUZZER_PIN, NOTE_F4, 280);
          noTone(BUZZER_PIN);
          delay(500);
          tft.fillScreen(TFT_BLACK);
          stage = 2;
        }

        digitalWrite(LED_RED_PIN, 1);
        blinkTime = 1;
        if (sound == 1)
        {
          tone(BUZZER_PIN, NOTE_C6, 4);
          noTone(BUZZER_PIN);
        }
        else
        {
          delay(4);
        }
      }
    }

    enemyY = enemyY + enemySpeed;
    if (enemyY > 80)
      enemySpeed = enemySpeed * -1;

    if (enemyY < 18)
      enemySpeed = enemySpeed * -1;

    if (blinkTime > 0)
      blinkTime++;

    if (blinkTime > 2)
    {
      digitalWrite(LED_GREEN_PIN, 0);
      digitalWrite(LED_RED_PIN, 0);
      blinkTime = 0;
    }

    for (int i = 0; i < 10; i++)
    { // enemy shoots
      if (energyBulletX[i] > -10)
      {
        tft.pushImage(energyBulletX[i], energyBulletY[i], 7, 7, enemyBullet);
        energyBulletX[i] = energyBulletX[i] - enemyBulletSpeed;
      }
    }

    for (int i = 0; i < 4; i++) // draw life
      tft.pushImage(i * 14, liveIndex[i], 12, 11, life);
    for (int i = 0; i < 3; i++) // draw ricon
      tft.pushImage(70 + (i * 14), rocketIndex[i], 8, 14, ricon);

    fireCount++;
    if (fireTime == fireCount)
    {
      energyBulletX[enemyCounter] = enemyX + 5;
      energyBulletY[enemyCounter] = enemyY + 24;
      fireCount = 0;
      fireTime = random(110 - (level * 15), 360 - (level * 30));
      enemyCounter++;
    }
    if (counter == 9)
      counter = 0;

    if (rocketCounter == 3)
      rocketCounter = 0;

    if (enemyCounter == 9)
      enemyCounter = 0;
  }
  if (stage == 2) // game over stage
  {

    tft.fillScreen(TFT_BLACK);
    tft.pushImage(0, 0, 240, 135, gameOver);
    tft.setCursor(24, 54, 2);
    tft.print("Score : " + String(score));
    tft.setCursor(24, 69, 2);
    tft.print("Level : " + String(level));
    buttonWait(BUTTON_A_PIN); // wait until button a is pressed.............
    stage = 0;
  }
}
