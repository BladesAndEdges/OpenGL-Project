#pragma once

// --------------------------------------------------------------------------------
enum class RendererType
{
	Forward,
	Deferred
};

// --------------------------------------------------------------------------------
class GraphicsConfiguations
{
public:

	GraphicsConfiguations();

	void update();

	RendererType getRendererType() const;

private:

	RendererType m_rendererType;
};

