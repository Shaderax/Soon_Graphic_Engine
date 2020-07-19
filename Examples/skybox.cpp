#include "Math/vec3.hpp"
#include "Math/vec2.hpp"

#include "GraphicsInstance.hpp"
#include "GraphicsRenderer.hpp"

#include "Mesh.hpp"
#include "Materials/GraphicMaterial.hpp"

#include "Utilities/ShowFps.hpp"

#include <chrono>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"

#include "MeshVertex.hpp"
#include "Camera.hpp"

using namespace Soon;

/**
 * OBJ LOADER
 */

Mesh* ObjLoader(void)
{
	MeshVertexDescription vd;
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	Mesh* mesh = new Mesh(vd);

	std::string inputfile = "./Examples/Cube.obj";
	//std::string inputfile = "./Examples/Cube.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
	if (!warn.empty())
	{
		std::cout << warn << std::endl;
	}

	if (!err.empty())
	{
		std::cerr << err << std::endl;
	}

	if (!ret)
	{
		exit(1);
	}
	mesh->SetVertexElement((uint8_t*)attrib.vertices.data(), attrib.vertices.size() / 3, MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	mesh->SetVertexElement((uint8_t*)attrib.texcoords.data(), attrib.texcoords.size() / 2, MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	mesh->SetVertexElement((uint8_t*)attrib.normals.data(), attrib.normals.size() / 3, MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	std::vector<uint32_t> indices;
	for (uint32_t index = 0 ; index < shapes[0].mesh.indices.size() ; index++)
		indices.push_back(shapes[0].mesh.indices[index].vertex_index);
	mesh->SetIndexBuffer(indices.data(), indices.size());

	return mesh;
}

Texture* LoadTextureCubeMap( void )
{
	Texture* textureCubeMap = new Texture();
	std::vector<std::string> paths = 
	{
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_bk.tga"},
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_dn.tga"},
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_ft.tga"},
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_lf.tga"},
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_rt.tga"},
		{"/home/shaderax/Documents/Project/Soon_Graphic_Engine/Ressources/Skybox/hw_lagoon/lagoon_up.tga"}
	};
	int channel, width, height;
	uint32_t offset = 0;
	std::vector<uint8_t> imageData;
	bool initVec = false;

	for (uint32_t index = 0 ; index < 6 ; index++)
	{
		unsigned char *data = stbi_load(paths[index].c_str(), &width, &height, &channel, 4);
		if (!initVec && (initVec = true))
			imageData.resize(width * height * 4 * 6);
		memcpy(imageData.data() + offset, data, width * height * 4);
		stbi_image_free(data);
		offset += width * height * 4;
	}

	textureCubeMap->SetData(imageData.data(), width, height, TextureFormat(EnumTextureFormat::RGBA), EnumTextureType::TEXTURE_CUBE);

	return textureCubeMap;
}

void Rotate(float x, float y, float z, Quaternion* _rot)
{
	if (x)
		*(_rot) *= Quaternion(vec3<float>(1.0f, 0.0f, 0.0f), x);
	if (y)
		*(_rot) *= Quaternion(vec3<float>(0.0f, 1.0f, 0.0f), y);
	if (z)
		*(_rot) *= Quaternion(vec3<float>(0.0f, 0.0f, 1.0f), z);
}

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();


	Mesh* mesh = ObjLoader();
	mesh->Render();
	mat4<float> model;
	mesh->GetMaterial().GetPipeline()->SetUnique("uc.view", &model);
	mesh->GetMaterial().GetPipeline()->SetUnique("uc.proj", &model);
	model(0, 0) = 0.1f;
	model(1, 1) = 0.1f;
	model(2, 2) = 0.1f;
	mesh->GetMaterial().GetPipeline()->Set("um.model", &model, mesh->GetId());
	/**
	 * TEXTURE CUBE MAP
	 */
	Texture* cubeMap = LoadTextureCubeMap();
	Mesh* meshCubeMap = ObjLoader();
	meshCubeMap->GetMaterial().SetPipeline("./Examples/Skybox.json");
	meshCubeMap->Render();
	meshCubeMap->GetMaterial().SetTexture("texSampler", *cubeMap);
	// TODO: CANNOT SETTEXTURE BEFORE RENDER
	/**
	 */

	double lastTime = 0;
	Camera3D camera;
	mat4<float> tmpMat;

	// Loop
	std::cout << "Begin Loop" << std::endl;
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		lastTime = ShowFPS(lastTime);

		tmpMat = camera.GetViewMatrix();
		mesh->GetMaterial().GetPipeline()->SetUnique("uc.view", &(tmpMat));
		tmpMat(3, 0) = 0;
		tmpMat(3, 1) = 0;
		tmpMat(3, 2) = 0;
		meshCubeMap->GetMaterial().GetPipeline()->SetUnique("uc.view", &(tmpMat));

		tmpMat = camera.GetProjectionMatrix();
		meshCubeMap->GetMaterial().GetPipeline()->SetUnique("uc.proj", &(tmpMat));
		mesh->GetMaterial().GetPipeline()->SetUnique("uc.proj", &(tmpMat));

		/**/
			vec3<float> dir;
			float rot = 0.0f;

			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_W))
				dir.z += 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_S))
				dir.z -= 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_A))
				dir.x -= 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_D))
				dir.x += 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_SPACE))
				dir.y += 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_LEFT_CONTROL))
				dir.y -= 0.01f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_LEFT))
				rot = 1.0f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_RIGHT))
				rot = -1.0f;

			camera.m_Pos += dir;
			Rotate(0.0f, rot, 0.0f, &(camera.m_Rotation));
							//Rotate(0.0f, rot, 0.0f);
		/**/

		GraphicsRenderer::GetInstance()->Update();
		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}

	delete meshCubeMap;
	delete cubeMap;

	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
