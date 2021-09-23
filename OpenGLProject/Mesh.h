#pragma once

#include "Material.h"

struct Mesh
{
	unsigned int firstIndex;
	unsigned int vertexCount;

	// Each individual mesh will have a pointer to the same material
	Material* material;
};