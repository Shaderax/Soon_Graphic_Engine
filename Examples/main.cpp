#include "Math/vec3.hpp"

#include "Renderer/Vulkan/GraphicsInstance.hpp"
#include "Renderer/Vulkan/GraphicsRenderer.hpp"

#include "Renderer/Mesh.hpp"
#include "Materials/ShaderMaterial.hpp"

#include "Renderer/Pipelines/NewDefaultPipeline.hpp"

std::vector<vec3<float>> triangle = {
{0.0, -0.500000, 0.000000},
{0.500000, 0.500000, 0.000000},
{-0.500000, 0.500000, 0.000000}
};

using namespace Soon;

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();

	VertexDescription vd;
	std::cout << "Mesh Vertex Element" << std::endl;
	vd.AddVertexElement(VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	Mesh mesh(vd);
	mesh.SetVertexElement((uint8_t*)triangle.data(), triangle.size(),VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	uint32_t* tab = new uint32_t[3];
	tab[0] = 0;
	tab[1] = 1;
	tab[2] = 2;

	mesh.SetIndexBuffer(tab, 3);

	mesh.Render();
	// Loop
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			//GraphicsInstance::GetInstance()->FillCommandBuffer();
			GraphicsRenderer::GetInstance()->ResetChange();
		}

		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}


	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
