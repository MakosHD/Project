#include <Arduino.h>//Бібліотека для коректної роботи Visual Studio Code з Arduino
#include <FastLED.h>// Бібліотека для роботи з адресними світлодіодами

//#define DEBUG // Режим дебагу


//Кнопки
#define red_butt_pin 2
#define green_butt_pin 3
#define blue_butt_pin 4
#define white_butt_pin 5

//Піни світлодіоду
#define red_led 11
#define green_led 10
#define blue_led 9

//Зумер
#define buzzer 6

//Адресна світлодіодна стрічка
#define LED_PIN 13
#define LED_NUM 11
CRGB leds[LED_NUM];

//Тип колір. Зберігає значення кольору в RGB
struct Color
{
  byte R;
  byte G;
  byte B;

  bool operator==(Color color)
  {
    return R == color.R && G == color.G && B == color.B;
  }
  bool operator!=(Color color)
  {
    return R != color.R || G != color.G || B != color.B;
  }
};

//Тип у якому зберігається всі стани кнопок
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

//Кольори
Color const white = {255, 255, 255};
Color const red = {255, 0, 0};
Color const green = {0, 255, 0};
Color const blue = {0, 0, 255};
Color const yellow = {255, 255, 0};
Color const cyan = {0, 255, 255};
Color const pink = {255, 0, 255};
Color const black = {0, 0, 0};

Color const  colors[7] = {
    white,
    red,
    green,
    blue,
    yellow,
    cyan,
    pink};

Color const colors_rgb[3] = {
    red,
    green,
    blue};

Color led_color = black;

//Тимчасовий буфер стану кнопок
bool red_butt = true;
bool green_butt = true;
bool blue_butt = true;
bool white_butt = true;

bool red_butt_old = true;
bool green_butt_old = true;
bool blue_butt_old = true;
bool white_butt_old = true;

//Час останнього натискання білого кнопки
unsigned long white_butt_last_time = 0;

//Індекс вибраного режиму
int index = 1;

void setup()
{
  //Піни RGB світлодіоду
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  
  //Налаштування адресної світлодіодної стрічки
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(50);

  //Кнопки
  pinMode(red_butt_pin, INPUT_PULLUP);
  pinMode(green_butt_pin, INPUT_PULLUP);
  pinMode(blue_butt_pin, INPUT_PULLUP);
  pinMode(white_butt_pin, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600);
  #define debug_print(x) Serial.print(x)
  #define debug_println(x) Serial.println(x);
#else
  #define debug_print(x)
  #define debug_println(x)
#endif

  setColor(colors[0]);//Встановити колір першого режиму(білий)

  randomSeed(analogRead(0)); //Задати випадкове значення для генератора випадкових чисел.(а не послідовних)
}

void get_butt_status() //Оновити буфер кнопок
{
  buttons.R = !digitalRead(red_butt_pin);
  buttons.G = !digitalRead(green_butt_pin);
  buttons.B = !digitalRead(blue_butt_pin);

  red_butt = digitalRead(red_butt_pin);
  green_butt = digitalRead(green_butt_pin);
  blue_butt = digitalRead(blue_butt_pin);
  white_butt = digitalRead(white_butt_pin);
}

void win(int time = 3000, bool Buzzer = true) //Анімація перемоги
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

void lose(int time = 3000, bool Buzzer = true) //Анімація програшу
{
  unsigned long start_time = millis();
  if (Buzzer)
  {
    tone(buzzer, 700, 200); 
    delay(260);
    tone(buzzer, 400, 200); 
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

void loop()//Так зване меню
{
  white_butt = digitalRead(white_butt_pin); //В меню використовується тільки одна кнопка а саме біла. Для чого тоді взнавати інші кнопки?
  
  //Обробка натискання білої кнопки
  if (white_butt != white_butt_old) 
  {
    white_butt_old = white_butt;
    if (!white_butt && millis() - white_butt_last_time > 250)
    {
      white_butt_last_time = millis();
      while (true) //Цикл щоб перевірити чи точно користувач хоче вибрати саме цей режим в режим
      {
        if (digitalRead(white_butt_pin))
        {
          break; // Якщо кнопка була відпущена то ми вже не ждемо далі щоб розпочати режим
        }
        else if (millis() - white_butt_last_time > 3000) //Кнопка була натиснути більше 3 секунд тому запускаємо відповідний режим
        {
          transition(led_color, black, 150);//миж не хочемо щоб колір на початку режиму був такий же самий як в меню.
          start();//Запуск режиму
          break;//Режим закінчився і потрібно вийти з циклу
        }
      }
      #ifdef DEBUG
      Serial.println("White button pressed");
      Serial.println(index);
      #endif
      if (index == 6)// Всього тільки 6 режимів. 
        index = 1;
      else
        index++;
      debug_print(index);
      transition(led_color, colors[index - 1], 150);//Кожен режим має свій колір
      tone(buzzer, (100 * index) + 200, 110); // І звук
    }
  }
}

void start() //Запуск режиму
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

void learn()//Перший режим. Експерименти з кольорами. Залежно які кнопки нажаті ті кольори і горять
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
    if (!red_butt)
      {
        setColor(255, led_color.G, led_color.B);
      }
      else
      {
        setColor(0, led_color.G, led_color.B);
      }
    
      if (!green_butt)
      {
        setColor(led_color.R, 255, led_color.B);
      }
      else
      {
        setColor(led_color.R, 0, led_color.B);
      }

    if (!blue_butt)
      {
        setColor(led_color.R, led_color.G, 255);
      }
      else
      {
        setColor(led_color.R, led_color.G, 0);
      }
    
    if (white_butt != white_butt_old)
    {
      white_butt_old = white_butt;
      if (!white_butt)
      {
        white_butt_last_time = millis();
      }
    }
    if (millis() - white_butt_last_time > 5000 && !white_butt)//Якщо кнопка зажата більше 5 секунд то виходимо з режиму
    {
      break;//Вихід з режиму
    }
  }
}

void sandbox()//Другий режим. Користувач можез адати в яких пропорціях змішуються кольори
{
  Color user_color = black;
  while (true)
  {
    #ifdef DEBUG
    Serial.print(user_color.R);
    Serial.print(", ");
    Serial.print(user_color.G);
    Serial.print(", ");
    Serial.println(user_color.B);
    #endif

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
        if (millis() - start_time > 3000)
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
  debug_println("Second game");

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
    debug_print("color for i: ");
    debug_println(index);
    transition(led_color, black, 100);
    transition(led_color, pattern[index], 150);
    delay(2000);
    transition(led_color, black, 100);

    bool user_lost = false;

    for (int i = 0; i <= index;)
    {

      debug_println("waiting");

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

          debug_println("right");

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
    debug_println("add new color");
    index++;

    if (millis() - white_butt_last_time > 5000 && !white_butt)
    {
      debug_println("White button timeout. Exiting game.");
      return;
    }
  }

  debug_println("Game over!");
  setColor(black); // Reset LEDs to black
}

void setColor(int R, int G, int B) //встановити певний колір на світлодіоді
{

#ifdef DEBUG
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

void transition(Color start, Color end, int time) // перехід від одного кольору до іншого за вказаний проміжок часу
{
  if (time <= 0)
    return; // Захист від некоректного часу
  // Використання float для точних обчислень
  float R = start.R;
  float G = start.G;
  float B = start.B;
  //визначення величини на яку колір повинний змінитися за одиницю часу.
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
    delay(1); // Очікування одиниці часу
  }
}
