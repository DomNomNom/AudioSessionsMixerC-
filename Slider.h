#pragma once

#include <time.h>
#include "CAudioSession.h"


struct Slider {
	// Represents an assignment of a view-controller for App volume to 


	// OS data coming to the slider
	int pid;
	CString label;
	float vuMeter; // range 0..1  Does note trigger systemUpdateTime
	float volumeFromSystem; // range 0..1
	time_t systemUpdateTime;

	// intent data coming from the slider
	float volumeIntent;  // range 0..1
	time_t intentUpdateTime;
};