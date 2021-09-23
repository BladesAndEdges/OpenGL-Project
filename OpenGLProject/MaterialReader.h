#pragma once

#include <string>
#include <unordered_map>
#include "Material.h"

class MaterialReader
{
private:

	std::unordered_map<std::string, Material> m_Materials;

public:

	MaterialReader();

	void parseMaterialFile(const std::string& fileName);

	std::unordered_map<std::string, Material> getMaterials() const;

	std::unordered_map<std::string, Material>& getReferenceToMaterials() const;
};

