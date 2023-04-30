#pragma once

#include <glad.h>
#include <stdint.h>

// --------------------------------------------------------------------------------
class UniformBuffer
{
public: 

	UniformBuffer(const uint32_t uniformBlockIndex, const uint32_t sizeInBytes, const void* data, const char* uniqueBufferName);
	~UniformBuffer();

	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	void useBuffer() const;
	void update(const void* data) const;

private:

	uint32_t m_uniformBlockIndex;
	uint32_t m_name;
	uint32_t m_sizeInBytes;
};

