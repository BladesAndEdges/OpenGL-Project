#include "UniformBuffer.h"

#include <assert.h>

// --------------------------------------------------------------------------------
UniformBuffer::UniformBuffer(const uint32_t uniformBlockIndex, const uint32_t sizeInBytes, const void* data,
																						const char* uniqueBufferName) : m_uniformBlockIndex(uniformBlockIndex),
																														m_sizeInBytes(sizeInBytes)
{
	assert(m_uniformBlockIndex != GL_INVALID_INDEX);

	glCreateBuffers(1, &m_name);
	glNamedBufferData(m_name, m_sizeInBytes , data, GL_DYNAMIC_DRAW);

	glObjectLabel(GL_BUFFER, m_name, -1, uniqueBufferName);
}

// --------------------------------------------------------------------------------
UniformBuffer::~UniformBuffer()
{
	if (m_name != 0)
	{
		glDeleteBuffers(1, &m_name);
	}
}

// --------------------------------------------------------------------------------
void UniformBuffer::useBuffer() const
{
	glBindBufferRange(GL_UNIFORM_BUFFER, m_uniformBlockIndex, m_name, 0, m_sizeInBytes);
}

// --------------------------------------------------------------------------------
void UniformBuffer::update(const void* data) const
{
	glNamedBufferData(m_name, m_sizeInBytes, data, GL_DYNAMIC_DRAW);
}
