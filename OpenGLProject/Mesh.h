#pragma once

#include "Material.h"

struct Mesh
{
    unsigned int firstIndex;
	unsigned int vertexCount;

	const Material* material;
};