#include <LiquidCrystal_I2C.h>
#include <ResinManager.h>

#define PIN_START_STOP 7
#define PIN_MODE 6
#define PIN_INCREASE_SOUND 5
#define PIN_DECREASE_SOUND 4

enum states {
  STATE_READY,
  STATE_EXECUTION,
  STATE_CLEARING,
  STATE_MAX
};

LiquidCrystal_I2C lcd(0x27, 20, 4);
ResinManager RM;
int state = STATE_READY;
bool stop_start_blocked = false;
bool change_blocked = false;

void draw_header() {
  lcd.setCursor(0,0);
  lcd.print("##");
  lcd.setCursor(3,0);
  lcd.print("Resin Cleaner");
  lcd.setCursor(18,0);
  lcd.print("##");
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
  if (RM.get_time() > 0) {
    RM.set_time(RM.get_time() - 1);
    draw();
    clear_time();
    delay(500);
  }
}

void increment_time() {
  if (RM.get_time() <= 300) {
    RM.set_time(RM.get_time() + 1);
    clear_time();
    draw();
    delay(500);
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
  state = STATE_READY;
  lcd.clear();
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_START_STOP, INPUT);
  pinMode(PIN_MODE, INPUT);
  pinMode(PIN_INCREASE_SOUND, INPUT);
  pinMode(PIN_DECREASE_SOUND, INPUT);

  lcd.begin();
  lcd.backlight();

  RM.set_time(1);
  state = STATE_READY;
}

void loop() {
  int start_stop = digitalRead(PIN_START_STOP);
  int change_mode = digitalRead(PIN_MODE);
  int increase_sound = digitalRead(PIN_INCREASE_SOUND);
  int decrease_sound = digitalRead(PIN_DECREASE_SOUND);

  if (start_stop == LOW && stop_start_blocked == true) {
    stop_start_blocked = false;
  }

  if (change_mode == LOW && change_blocked == true) {
    change_blocked = false;
  }

  if (state == STATE_READY) {
    if (increase_sound == HIGH) {
      increment_time();
      draw();
    } else if (decrease_sound == HIGH) {
      reduce_time();
      draw();
    } else if (change_mode == HIGH && change_blocked == false) {
      modify_mode();
      change_blocked = true;
      draw();
    } else if (start_stop == HIGH && stop_start_blocked == false) {
      state = STATE_EXECUTION;
      RM.run(onComplete, onRefresh);
      draw_mode_execution();
      stop_start_blocked = true;
      delay(100);
    } else {
      draw();
    }
  } else if (state == STATE_EXECUTION) {
    if (start_stop == HIGH && stop_start_blocked == false) {
      lcd.clear();
      RM.stop();
      state = STATE_CLEARING;
      stop_start_blocked = true;
    } else {
      RM.start();
    }
  } else if (state == STATE_CLEARING) {
    state = STATE_READY;
  }

  delay(10);
}
