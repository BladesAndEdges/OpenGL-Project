#pragma once

#include "ProfileMarker.h"
#include<iostream>

// --------------------------------------------------------------------------------
ProfileMarker::ProfileMarker()
{
	QueryPerformanceCounter(&m_countsAtStart);
}

// --------------------------------------------------------------------------------
void ProfileMarker::endTiming()
{
	QueryPerformanceCounter(&m_countsAtEnd);
	m_clockCycles.QuadPart = m_countsAtEnd.QuadPart - m_countsAtStart.QuadPart;
	m_countsAtEnd.QuadPart = -1;
}

// --------------------------------------------------------------------------------
void ProfileMarker::accumulate(LONGLONG & result)
{
	result += m_clockCycles.QuadPart;
}

// --------------------------------------------------------------------------------
ProfileMarker::~ProfileMarker()
{
	if (m_countsAtEnd.QuadPart >= 0)
	{
		throw std::exception("Missing call to endTiming()!");
	}
}

// --------------------------------------------------------------------------------
LONGLONG clockCyclesToMiliseconds(LONGLONG clockCycles)
{
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
		std::cout << "Could not query the frequency!" << std::endl;
	}

	return (1000 * clockCycles) / frequency.QuadPart;
}
