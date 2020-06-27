#ifndef RESIN_MANAGER_H
#define RESIN_MANAGER_H

#include <Countimer.h>

enum resin_mode {
	CLEAN_MODE,
	CURE_MODE,
	MAX_MODE
};

class ResinManager {
public:
	ResinManager();

	void set_mode(resin_mode mode);
	void set_time(int minutes);
	resin_mode get_mode();
	int get_time();
	String get_time_left();
	void start();
	void stop();
	void run(void (*onComplete)(), void (*onRefresh)());
private:
	Countimer timer;
	resin_mode _mode;
	int _minutes;
};

#endif
