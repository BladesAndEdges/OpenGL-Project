#pragma 

#include "ResourceCompiler.h"

class TextureCompiler : public ResourceCompiler
{
public:

	TextureCompiler();

	uint32_t getVersionNumber() const override;

	void compile(ResourceCompilationContext& resourceCompilationContext) const override;
};

