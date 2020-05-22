#include "Math/vec3.hpp"

#include "GraphicsInstance.hpp"
#include "GraphicsRenderer.hpp"

#include "Mesh.hpp"
#include "Materials/ShaderMaterial.hpp"

#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/ShowFps.hpp"

std::vector<vec3<float>> triangle =
{
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
	vd.AddVertexElement(VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	Mesh* mesh = new Mesh(vd);
	mesh->SetVertexElement((uint8_t*)triangle.data(), triangle.size(),VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	uint32_t* tab = new uint32_t[3];
	tab[0] = 0;
	tab[1] = 1;
	tab[2] = 2;

	mesh->SetIndexBuffer(tab, 3);

	mesh->Render();
	mesh->GetMaterial().SetVec3("cou.bondour", vec3<float>(0.2f, 0.1f, 0.0f));
	double lastTime = 0;
	// Loop
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		lastTime = ShowFPS(lastTime);
		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			GraphicsInstance::GetInstance()->FillCommandBuffer();
			GraphicsRenderer::GetInstance()->ResetChange();
		}

		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}

	delete mesh;

	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
