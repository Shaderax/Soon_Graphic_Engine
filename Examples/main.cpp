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

/*
main()
{
	Mesh mesh;
	mesh.SetData();
	mesh.Render();
	mesh.UnRender();

	//1 mesh pour plusieurs mat; // Pluslieurs object
	Mesh mesh1;
	Mesh mesh2(mesh1); // Je recup l'idMesh et les datas, mais pas le material.

	//1 mat pour plusieurs mesh;
	Mesh mesh1;
	Mesh mesh2.SetMaterial(mesh1.GetMaterial());

	// Me faut de quoi Render || UnRender
	// Et de quoi AllocGpu et FreeGpu
}
*/
using namespace Soon;

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();

	VertexDescription vd;
	vd.AddVertexElement(VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	Mesh mesh(vd);
	mesh.SetVertexElement((uint8_t*)triangle.data(), triangle.size(),VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	// Loop
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			GraphicsInstance::GetInstance()->FillCommandBuffer();
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
