#pragma once

#include "Material.h"

struct Mesh
{
	unsigned int firstIndex;
	unsigned int indicesCount;

	const Material* material;
};