#include "ResinManager.h"
#include <LiquidCrystal_I2C.h>
#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h>

ResinManager::ResinManager() {
	_minutes = 1;
	_mode = CLEAN_MODE;
}

void ResinManager::set_mode(resin_mode mode) {
	_mode = mode;
}

void ResinManager::set_time(int minutes) {
	_minutes = minutes;
}

resin_mode ResinManager::get_mode() {
	return _mode;
}

int ResinManager::get_time() {
	return _minutes;
}

String ResinManager::get_time_left() {
	return timer.getCurrentTime();
}

void ResinManager::run(void (*onComplete)(), void (*onRefresh)()) {
	timer.setCounter(_minutes / 60, _minutes % 60, 0, timer.COUNT_DOWN, onComplete);
	timer.setInterval(onRefresh, 1000);

	if (_mode == CLEAN_MODE) {
		_motor_speed = 250;
	} else {
		_motor_speed = 25;
	}
}

void ResinManager::start() {
	timer.run();
	if(!timer.isCounterCompleted()) {
		timer.start();

		if (_minutes == 1) {
			if (timer.getCurrentSeconds() == 30)
				change_direction();
		} else {
			if (timer.getCurrentMinutes() != 0) {
				if (timer.getCurrentSeconds() == 0)
					change_direction();
			}
		}

		if (_motor_dir) {
			analogWrite(_pinPWM2, 0);
			analogWrite(_pinPWM1, _motor_speed);
		} else {
			analogWrite(_pinPWM1, 0);
			analogWrite(_pinPWM2, _motor_speed);
		}
	}
}

void ResinManager::stop() {
	timer.stop();
	timer.restart();
}

void ResinManager::init_motor(int pinPWM1, int pinPWM2, int pinSpeed) {
	pinMode(pinPWM1, OUTPUT);
	pinMode(pinPWM2, OUTPUT);
	pinMode(pinSpeed, INPUT);

	_pinPWM1 = pinPWM1;
	_pinPWM2 = pinPWM2;
	_pinSpeed = pinSpeed;
}

void ResinManager::change_direction() {
	if (_motor_dir == true) {
		_motor_dir = false;
	} else {
		_motor_dir = true;
	}
}

void ResinManager::modify_speed(int speed) {
	_motor_speed = speed;
}

void ResinManager::stop_motor() {
	analogWrite(_pinPWM1, 0);
	analogWrite(_pinPWM2, 0);
}

int ResinManager::get_actual_speed() {
	return analogRead(_pinSpeed) / 4;
}
