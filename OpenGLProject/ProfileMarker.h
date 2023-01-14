#pragma once

#include <string>
#include <windows.h>

class ProfileMarker
{
public:

	LARGE_INTEGER frequency;
	LARGE_INTEGER countsAtStart;
	LARGE_INTEGER countsAtEnd;
	std::string name;

	ProfileMarker(const std::string& markerName);

	ProfileMarker(const ProfileMarker&) = delete;
	ProfileMarker& operator=(const ProfileMarker&) = delete;

	void endTiming();

	~ProfileMarker();
};