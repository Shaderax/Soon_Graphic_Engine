#include "Math/vec3.hpp"
#include "Math/vec2.hpp"

#include "GraphicsInstance.hpp"
#include "GraphicsRenderer.hpp"

#include "Mesh.hpp"
#include "Materials/ShaderMaterial.hpp"

#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/ShowFps.hpp"

#include <chrono>
#include <ctime>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//std::vector<vec3<float>> triangle =
	//{0.0, -0.500000, 0.000000},
	//{0.500000, 0.500000, 0.000000},
	//{-0.500000, 0.500000, 0.000000}

std::vector<vec2<float>> triangle =
{
	{0.0, -0.500000},
	{0.500000, 0.500000},
	{-0.500000, 0.500000}
};

std::vector<vec2<float>> texCoord = 
{
	{0.0, -1.0},
	{1.0, 1.0},
	{-1.0, 1.0}
};

using namespace Soon;

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();

	/**
	 * Texture 
	 */
	Texture texture;
	int channel, width, height;
	//unsigned char *data = stbi_load("/home/shaderax/Pictures/Alex.png", &width, &height, &channel, 4);
	unsigned char *data = stbi_load("/home/shaderax/Pictures/Aude2.png", &width, &height, &channel, 4);
	texture.SetData(data, width,height, TextureFormat(EnumTextureFormat::RGBA));
	//GraphicsRenderer::GetInstance()->AddTexture(&texture);
	stbi_image_free(data);


	std::ifstream i("/home/shaderax/Documents/Project/Soon_Graphic_Engine/Examples/test.json");
	json j;
	i >> j;
	std::cout << j["pi"] << std::endl;

	VertexDescription vd;
	//vd.AddVertexElement(VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	vd.AddVertexElement(VertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	Mesh* mesh = new Mesh(vd);
	//mesh->SetVertexElement((uint8_t*)triangle.data(), triangle.size(),VertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	mesh->SetVertexElement((uint8_t*)triangle.data(),
							triangle.size(),
							VertexElement(EnumVertexElementSementic::POSITION,
							VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));

	mesh->SetVertexElement((uint8_t*)texCoord.data(),
							texCoord.size(),
							VertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	uint32_t* tab = new uint32_t[3];
	tab[0] = 0;
	tab[1] = 1;
	tab[2] = 2;

	mesh->SetIndexBuffer(tab, 3);

	mesh->Render();
	mesh->GetMaterial().SetTexture("latexture", texture);
	mesh->GetMaterial().SetVec3("cou.bondour", vec3<float>(0.2f, 0.1f, 0.0f));
	double lastTime = 0;
	bool did = false;
	double time = glfwGetTime();
	double x = 0.0f;
	double y = 0.0f;
	// Loop
	std::cout << "Begin Loop" << std::endl;
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		lastTime = ShowFPS(lastTime);
		glfwGetCursorPos(GraphicsInstance::GetInstance()->GetWindow(), &x, &y);
		mesh->GetMaterial().SetVec3("cou.bondour", vec3<float>(((float)x / 1280) * 2 - 1, ((float)y / 720) * 2 - 1, 0.0f));

		if (glfwGetTime() - time > 5.0f)
		{
			time = glfwGetTime();
			if (!did)
			{
				did = true;
				mesh->UnRender();
			}
			else
			{
				mesh->Render();
				did = false;
			}
		}

		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			GraphicsRenderer::GetInstance()->DestroyInvalids();
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
