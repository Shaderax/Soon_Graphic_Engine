#include "Renderer/Vulkan/GraphicsInstance.hpp"
#include "Renderer/Vulkan/GraphicsRenderer.hpp"

using namespace Soon;

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();


	// Loop
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		/*
		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			GraphicsInstance::GetInstance()->FillCommandBuffer();
			GraphicsRenderer::GetInstance()->ResetChange();
		}
		*/

		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}


	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
