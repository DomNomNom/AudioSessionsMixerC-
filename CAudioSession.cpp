#include "pch.h"
#include "CAudioSession.h"

CAudioSession::CAudioSession()
{
	pSessionControl = NULL;
	pSessionControl2 = NULL;
}

CAudioSession::~CAudioSession()
{
	/*SAFE_RELEASE(pSessionControl);
	SAFE_RELEASE(pSessionControl2);*/
}
