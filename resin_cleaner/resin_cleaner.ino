#include <LiquidCrystal_I2C.h>
#include <ResinManager.h>

#define PIN_START_STOP 7
#define PIN_MODE 6
#define PIN_INCREASE_SOUND 5
#define PIN_DECREASE_SOUND 4

#define PIN_PWM1 9
#define PIN_PWM2 10
#define PIN_SPEED 0

enum states {
  STATE_READY,
  STATE_EXECUTION,
  STATE_CLEARING,
  STATE_MAX
};

LiquidCrystal_I2C lcd(0x27, 20, 4);
ResinManager RM;
int state = STATE_READY;
int last_start_stop = 0;
int last_change = 0;

void draw_header() {
  lcd.setCursor(0,0);
  lcd.print("##");
  lcd.setCursor(3,0);
  lcd.print("Resin Cleaner");
  lcd.setCursor(18,0);
  lcd.print("##");
}

void draw_init() {
  lcd.setCursor(5,0);
  lcd.print("----------");
  lcd.setCursor(5,1);
  lcd.print("| PHTech |");
  lcd.setCursor(5,2);
  lcd.print("----------");
  lcd.setCursor(0,3);
  lcd.print("alvaroneay@gmail.com");
}

void draw() {  
  draw_header();
  lcd.setCursor(3,1);
  lcd.print("Mode");
  lcd.setCursor(13,1);
  lcd.print("Time");

  if (RM.get_mode() == CLEAN_MODE) {
    lcd.setCursor(0,2);
    lcd.print("-> Clean");
    lcd.setCursor(0,3);
    lcd.print("   Cure");
  } else {
    lcd.setCursor(0,2);
    lcd.print("   Clean");
    lcd.setCursor(0,3);
    lcd.print("-> Cure");
  }

  lcd.setCursor(14,2);
  lcd.print(RM.get_time());
}

void draw_mode_execution() {
  lcd.clear();
  draw_header();

  if (RM.get_mode() == CLEAN_MODE) {
    lcd.setCursor(3,1);
    lcd.print("Mode : Cleaning");
  } else {
    lcd.setCursor(3,1);
    lcd.print("Mode : Curing");
  }

  lcd.setCursor(3,2);
  lcd.print("Time Remaining");
  lcd.setCursor(6,3);
  lcd.print(RM.get_time_left());
}

void clear_time() {
  lcd.setCursor(14,2);
  lcd.print("   ");
}

void reduce_time() {
  if (RM.get_time() > 1) {
    RM.set_time(RM.get_time() - 1);
    draw();
    clear_time();
    delay(100);
  }
}

void increment_time() {
  if (RM.get_time() <= 300) {
    RM.set_time(RM.get_time() + 1);
    clear_time();
    draw();
    delay(100);
  }
}

void modify_mode() {
  if (RM.get_mode() == CLEAN_MODE) {
    RM.set_mode(CURE_MODE);
  } else {
    RM.set_mode(CLEAN_MODE);
  }
  
  delay(100);
}

void onRefresh() {
  lcd.setCursor(6,3);
  lcd.print(RM.get_time_left());
}

void onComplete() {
  lcd.setCursor(6,3);
  lcd.print("Complete!!!");
  delay(2000);
  state = STATE_CLEARING;
  lcd.clear();
  RM.stop();
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_START_STOP, INPUT);
  pinMode(PIN_MODE, INPUT);
  pinMode(PIN_INCREASE_SOUND, INPUT);
  pinMode(PIN_DECREASE_SOUND, INPUT);

  lcd.begin();
  lcd.backlight();

  RM.init_motor(PIN_PWM1, PIN_PWM2, PIN_SPEED);
  state = STATE_READY;

  draw_init();
  delay(3000);
  lcd.clear();
}

void loop() {
  int start_stop = digitalRead(PIN_START_STOP);
  int change_mode = digitalRead(PIN_MODE);
  int increase_sound = digitalRead(PIN_INCREASE_SOUND);
  int decrease_sound = digitalRead(PIN_DECREASE_SOUND);

  if (state == STATE_READY) {
    if (increase_sound == HIGH) {
      increment_time();
      draw();
    } else if (decrease_sound == HIGH) {
      reduce_time();
      draw();
    } else if (change_mode == HIGH && change_mode != last_change) {
      modify_mode();
      draw();
    } else if (start_stop == HIGH && start_stop != last_start_stop) {
      state = STATE_EXECUTION;
      RM.run(onComplete, onRefresh);
      draw_mode_execution();
    } else {
      draw();
    }
  } else if (state == STATE_EXECUTION) {
    if (start_stop == HIGH && start_stop != last_start_stop) {
      lcd.clear();
      RM.stop();
      state = STATE_CLEARING;
    } else {
      RM.start();
    }
  } else if (state == STATE_CLEARING) {
    RM.stop_motor();
    state = STATE_READY;
  }

  last_start_stop = start_stop;
  last_change = change_mode;
  delay(10);
}
