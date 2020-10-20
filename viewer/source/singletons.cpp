#include "singletons.hpp"

std::unique_ptr<Renderer> renderer_wrapper = nullptr;

void Singletons::create_renderer()
{
	renderer_wrapper = std::make_unique<Renderer>();
}

Renderer& Singletons::renderer()
{
	return *renderer_wrapper;
}