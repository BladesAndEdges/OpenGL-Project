#pragma once

// --------------------------------------------------------------------------------
class ModelCacher
{
public:

	static bool tryReadFromCache(const char* filePath);
	static void writeToCache(const char* filePath);
};

