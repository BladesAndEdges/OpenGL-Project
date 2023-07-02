#pragma once

#include <string>
#include <windows.h>

class ProfileMarker
{
public:

	LARGE_INTEGER m_countsAtStart;
	LARGE_INTEGER m_countsAtEnd;
	LARGE_INTEGER m_clockCycles;

	ProfileMarker();

	ProfileMarker(const ProfileMarker&) = delete;
	ProfileMarker& operator=(const ProfileMarker&) = delete;

	void endTiming();
	void accumulate(LONGLONG& result);
	
	~ProfileMarker();
};

LONGLONG clockCyclesToMiliseconds(LONGLONG clockCycles);