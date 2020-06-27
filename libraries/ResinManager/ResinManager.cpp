#include "ResinManager.h"
#include <LiquidCrystal_I2C.h>
#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h>

ResinManager::ResinManager() {
	_minutes = 0;
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
}

void ResinManager::start() {
	timer.run();
	if(!timer.isCounterCompleted()) {
		timer.start();
	}
}

void ResinManager::stop() {
	timer.stop();
	timer.restart();
}
