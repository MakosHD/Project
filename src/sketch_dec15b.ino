#include <Arduino.h>

#include <FastLED.h>

#define DEBUG


#define red_butt_pin 2
#define blue_butt_pin 3
#define green_butt_pin 4
#define white_butt_pin 5

#define red_led 9
#define green_led 10
#define blue_led 11

#define buzzer 8

#define LED_PIN 13
#define LED_NUM 11
CRGB leds[LED_NUM];

struct Color
{
  byte R;
  byte G;
  byte B;

  // ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ ЗАПАМЯТАТИ для чого це(полюбому спитають)
  //  Конструктор для ініціалізації кольору
  /*Ваші визначення кольорів припинили працювати через відсутність конструктора за замовчуванням у структурі Color. Коли Ви додаєте користувацький конструктор (наприклад, для перетворення Buttons у Color), компілятор більше не генерує конструктор за замовчуванням. Щоб вирішити цю проблему, можна додати конструктор для ініціалізації кольорів вручну:*/
  /*  Color(byte r = 0, byte g = 0, byte b = 0) : R(r), G(g), B(b) {}

    Color(const Buttons& buttons) : R(buttons.R ? 255 : 0), G(buttons.G ? 255 : 0), B(buttons.B ? 255 : 0) {}
  */
  bool operator==(Color color)
  {
    return R == color.R && G == color.G && B == color.B;
  }
  bool operator!=(Color color)
  {
    return R != color.R || G != color.G || B != color.B;
  }
};

struct Buttons
{
  bool R;
  bool G;
  bool B;

  bool operator==(Buttons button)
  {
    return R == button.R && G == button.G && B == button.B;
  }
  bool operator!=(Buttons button)
  {
    return R != button.R || G != button.G || B != button.B;
  }
};
Buttons buttons;

Color white = {255, 255, 255};
Color red = {255, 0, 0};
Color green = {0, 255, 0};
Color blue = {0, 0, 255};
Color yellow = {255, 255, 0};
Color cyan = {0, 255, 255};
Color pink = {255, 0, 255};
Color black = {0, 0, 0};

Color colors[7] = {
    white,
    red,
    green,
    blue,
    yellow,
    cyan,
    pink};

Color colors_rgb[3] = {
    red,
    green,
    blue};

Color led_color = black;

bool red_butt = true;
bool green_butt = true;
bool blue_butt = true;
bool white_butt = true;

bool red_butt_old = true;
bool green_butt_old = true;
bool blue_butt_old = true;
bool white_butt_old = true;

unsigned long white_butt_last_time = 0;

int index = 1;

void setup()
{
  // leds
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(50);

  // buttons
  pinMode(red_butt_pin, INPUT_PULLUP);
  pinMode(green_butt_pin, INPUT_PULLUP);
  pinMode(blue_butt_pin, INPUT_PULLUP);
  pinMode(white_butt_pin, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600);
#endif

  setColor(colors[0]);

  randomSeed(analogRead(0));
}

void get_butt_status()
{
  buttons.R = !digitalRead(red_butt_pin);
  buttons.G = !digitalRead(green_butt_pin);
  buttons.B = !digitalRead(blue_butt_pin);

  red_butt = digitalRead(red_butt_pin);
  green_butt = digitalRead(green_butt_pin);
  blue_butt = digitalRead(blue_butt_pin);
  white_butt = digitalRead(white_butt_pin);
}

void win(int time = 3000, bool Buzzer = true)
{
  unsigned long start_time = millis();
  if (Buzzer)
  {
    tone(buzzer, 500, 200);
    delay(260);
    tone(buzzer, 800, 200);
    delay(260);
    noTone(buzzer);
  }

  transition(led_color, black, 100);
  while (millis() - start_time < time)
  {
    transition(black, green, 150);
    transition(green, black, 150);
  }
}

void lose(int time = 3000, bool Buzzer = true)
{
  unsigned long start_time = millis();
  if (Buzzer)
  {
    tone(buzzer, 700, 200); // B5
    delay(260);
    tone(buzzer, 400, 200); // G5
    delay(260);
    noTone(buzzer);
  }
  transition(led_color, black, 100);
  while (millis() - start_time < time)
  {
    transition(black, red, 100);
    transition(red, black, 100);
  }
}

void loop()
{
  white_butt = digitalRead(white_butt_pin);
  if (white_butt != white_butt_old)
  {
    white_butt_old = white_butt;
    if (!white_butt && millis() - white_butt_last_time > 250)
    {
      white_butt_last_time = millis();
      while (true)
      {
        if (digitalRead(white_butt_pin))
        {
          break;
        }
        else if (millis() - white_butt_last_time > 3000)
        {
          setColor(black);
          start();
          transition(led_color, black, 150);
          break;
        }
      }

#ifdef DEBUG
      Serial.println("White button pressed");
      Serial.println(index);
#endif
      if (index == 7)
        index = 1;
      else
        index++;

      transition(led_color, colors[index - 1], 150);
      tone(buzzer, (100 * index) + 200, 110);
    }
  }
}

void start()
{
  switch (index)
  {
  case 1:
    learn();
    break;
  case 2:
    sandbox();
    break;
  case 3:
    first_game(false);
    break;
  case 4:
    first_game(true);
    break;
  case 5:
    second_game(false);
    break;
  case 6:
    second_game(true);
    break;
  }
}

void learn()
{
  while (true)
  {
#ifdef DEBUG
    Serial.print(digitalRead(red_butt_pin));
    Serial.print(", ");
    Serial.print(digitalRead(green_butt_pin));
    Serial.print(", ");
    Serial.println(digitalRead(blue_butt_pin));
#endif

    get_butt_status();
    if (red_butt != red_butt_old)
    {
      red_butt_old = red_butt;
      if (!red_butt)
      {
        setColor(255, led_color.G, led_color.B);
      }
      else
      {
        setColor(0, led_color.G, led_color.B);
      }
    }

    if (green_butt != green_butt_old)
    {
      green_butt_old = green_butt;
      if (!green_butt)
      {
        setColor(led_color.R, 255, led_color.B);
      }
      else
      {
        setColor(led_color.R, 0, led_color.B);
      }
    }

    if (blue_butt != blue_butt_old)
    {
      blue_butt_old = blue_butt;
      if (!blue_butt)
      {
        setColor(led_color.R, led_color.G, 255);
      }
      else
      {
        setColor(led_color.R, led_color.G, 0);
      }
    }
    if (white_butt != white_butt_old)
    {
      white_butt_old = white_butt;
      if (!white_butt)
      {
        white_butt_last_time = millis();
      }
    }
    if (millis() - white_butt_last_time > 5000 && !white_butt)
    {
      break;
    }
  }
}

void sandbox()
{
  Color user_color = black;
  while (true)
  {
    Serial.print(user_color.R);
    Serial.print(", ");
    Serial.print(user_color.G);
    Serial.print(", ");
    Serial.println(user_color.B);
    get_butt_status();
    setColor(user_color);
    if (red_butt != red_butt_old)
    {
      red_butt_old = red_butt;
      if (!red_butt)
        user_color.R += 25;
    }

    if (green_butt != green_butt_old)
    {
      green_butt_old = green_butt;
      if (!green_butt)
        user_color.G += 25;
    }

    if (blue_butt != blue_butt_old)
    {
      blue_butt_old = blue_butt;
      if (!blue_butt)
        user_color.B += 25;
    }

    if (user_color.R == 19)
      user_color.R = 0;
    if (user_color.G == 19)
      user_color.G = 0;
    if (user_color.B == 19)
      user_color.B = 0;

    if (white_butt != white_butt_old)
    {
      white_butt_old = white_butt;
      if (!white_butt)
      {
        white_butt_last_time = millis();
      }
    }
    if (millis() - white_butt_last_time > 5000 && !white_butt)
      return;
  }
}

void first_game(bool multi_color_mode)
{
  while (true)
  {
    unsigned long start_time = millis();
    Buttons prev;

    Color system_color;
    if (multi_color_mode)
    {
      system_color = colors[random(7)];
    }
    else
    {
      system_color = colors_rgb[random(3)];
    }

    transition(led_color, system_color, 150);

    while (true)
    {
      get_butt_status();
      Color user_color;
      if (!red_butt)
        user_color.R = 255;
      else
        user_color.R = 0;
      if (!green_butt)
        user_color.G = 255;
      else
        user_color.G = 0;
      if (!blue_butt)
        user_color.B = 255;
      else
        user_color.B = 0;

      if (user_color == system_color)
      {
        win(3000);
        break;
      }
      if (buttons == prev && buttons != (Buttons){false, false, false})
      {
        if (millis() - start_time > 5000)
        {
          lose(3000);
          break;
        }
      }
      else
      {
        prev = buttons;
        start_time = millis();
      }
      if (white_butt != white_butt_old)
      {
        white_butt_old = white_butt;
        if (!white_butt)
        {
          white_butt_last_time = millis();
        }
      }
      if (millis() - white_butt_last_time > 5000 && !white_butt)
      {
        return;
      }
    }
  }
}

void second_game(bool multi_color_mode)
{
  unsigned long start_time;
  setColor(black);
  Serial.println("Second game started");

  Color pattern[100];
  int index = 0;

  while (true)
  {
    if (multi_color_mode)
    {
      pattern[index] = colors[random(7)];
    }
    else
    {
      pattern[index] = colors_rgb[random(3)];
    }

    Serial.print("Selected color for index: ");
    Serial.println(index);
    transition(led_color, black, 100);
    transition(led_color, pattern[index], 150);
    delay(2000);
    transition(led_color, black, 100);

    bool user_lost = false;

    for (int i = 0; i <= index;)
    {
      Serial.println("Waiting for user input...");
      start_time = millis();
      bool correct_input = false;

      while (millis() - start_time < 3000)
      {
        get_butt_status();
        Color user_color;
        user_color.R = (buttons.R ? 255 : 0);
        user_color.G = (buttons.G ? 255 : 0);
        user_color.B = (buttons.B ? 255 : 0);

        transition(led_color, user_color, 100);

        if (user_color == pattern[i])
        {
          Serial.println("Correct!");
          tone(buzzer, 500, 200);
          delay(260);
          tone(buzzer, 800, 200);
          delay(260);
          noTone(buzzer);

          correct_input = true;
          i++;
          break;
        }
      }

      if (!correct_input)
      {
        user_lost = true;
        break;
      }
    }
    if (user_lost)
    {
      lose(3000);
      return;
    }
    Serial.println("Pattern completed. Adding new color.");
    index++;

    if (millis() - white_butt_last_time > 5000 && !white_butt)
    {
      Serial.println("White button timeout. Exiting game.");
      return;
    }
  }

  Serial.println("Game over!");
  setColor(black); // Reset LEDs to black
}

void setColor(int R, int G, int B)
{

#ifdef DEBU
  char buffer[50];
  sprintf(buffer, "%d, %d, %d", R, G, B);
  Serial.print("Color is ");
  Serial.println(buffer);
#endif

  FastLED.showColor(CRGB(R, G, B));
  analogWrite(red_led, map(R, 0, 255, 255, 0));
  analogWrite(green_led, map(G, 0, 255, 255, 0));
  analogWrite(blue_led, map(B, 0, 255, 255, 0));
  led_color.R = R;
  led_color.G = G;
  led_color.B = B;
}

void setColor(Color color)
{
  setColor(color.R, color.G, color.B);
}

void transition(Color start, Color end, int time)
{
  if (time <= 0)
    return; // Захист від некоректного часу
  // Використання float для точних обчислень
  float R = start.R;
  float G = start.G;
  float B = start.B;

  float R_step = (float)(end.R - start.R) / time;
  float G_step = (float)(end.G - start.G) / time;
  float B_step = (float)(end.B - start.B) / time;

  for (int i = 0; i < time; i++)
  {
    R += R_step;
    G += G_step;
    B += B_step;

    // Округлення та обмеження значень
    int R_int = constrain((int)round(R), 0, 255);
    int G_int = constrain((int)round(G), 0, 255);
    int B_int = constrain((int)round(B), 0, 255);

    // Встановлення кольору
    setColor(R_int, G_int, B_int);
    delay(1); // Очікування для плавного переходу
  }
}

