#pragma once

#pragma warning(push)
#pragma warning(error: 26495)

#include <time.h>
#include "CAudioSession.h"


struct Slider {
	// Represents an assignment of a view-controller for App volume to 


	// OS data coming to the slider
	bool connected;  // whether we expect there to be an audio session corresponding to our sid.
	LPWSTR sid;  // Session identifier. How we intend to match with audio sessions.
	CString label;
	float vuMeter; // range 0..1  Does not trigger systemUpdateTime
	float volumeFromSystem; // range 0..1

	// intent data coming from the slider
	float volumeIntent;  // range 0..1

	time_t sidUpdateTime;
	time_t systemVolumeUpdateTime;
	time_t dragStartTime;
};

#pragma warning(pop)