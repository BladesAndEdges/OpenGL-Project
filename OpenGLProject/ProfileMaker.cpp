#pragma once

#include "ProfileMarker.h"
#include<iostream>

ProfileMarker::ProfileMarker(const std::string& markerName) : name(markerName)
{
	if (!QueryPerformanceFrequency(&frequency))
	{
		std::cout << "Could not query the frequency for Performance Counter " << name << std::endl;
	}

	QueryPerformanceCounter(&countsAtStart);
}

void ProfileMarker::endTiming()
{
	QueryPerformanceCounter(&countsAtEnd);
	std::cout << "PERFORMANCE COUNTER << " << name << " >> : | " << (countsAtEnd.QuadPart - countsAtStart.QuadPart) / frequency.QuadPart << " seconds |" << std::endl;
	countsAtEnd.QuadPart = -1;
}

ProfileMarker::~ProfileMarker()
{
	if (countsAtEnd.QuadPart >= 0)
	{
		std::cout << "PERFORMANCE COUNTER " << name << "MISSING CORRESPONDING endTiming()!" << std::endl;
	}
}