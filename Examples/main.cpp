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

	// Loop over shapes
	//for (size_t s = 0; s < shapes.size(); s++)
	//{
	//	// Loop over faces(polygon)
	//	size_t index_offset = 0;
	//	for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
	//	{
	//		int fv = shapes[s].mesh.num_face_vertices[f];
	//		// Loop over vertices in the face.
	//		for (size_t v = 0; v < fv; v++)
	//		{
	//			// access to vertex
	//			tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
	//			tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
	//			tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
	//			tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
	//			tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
	//			tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
	//			tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
	//			tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
	//			tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
	//			// Optional: vertex colors
	//			// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
	//			// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
	//			// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
	//		}
	//		index_offset += fv;
//
	//		// per-face material
	//		shapes[s].mesh.material_ids[f];
	//	}
	//}
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

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();

	/**
	 * Texture 
	 */
	//Texture texture;
	//int channel, width, height;
	//unsigned char *data = stbi_load("/home/shaderax/Pictures/Alex.png", &width, &height, &channel, 4);
	//texture.SetData(data, width, height, TextureFormat(EnumTextureFormat::RGBA));
	//stbi_image_free(data);

	/**
	 * TEXTURE CUBE MAP
	 */
	//Texture* cubeMap = LoadTextureCubeMap();
	//Mesh* meshCubeMap = ObjLoader();
	//meshCubeMap->GetMaterial().SetPipeline("./Examples/Skybox.json");
	//meshCubeMap->Render();
	// TODO: CANNOT SETTEXTURE BEFORE RENDER
	//meshCubeMap->GetMaterial().SetTexture("texSampler", *cubeMap);
	/**
	 */

	/**
	 * MESH
	 */
	//Mesh* mesh = ObjLoader();
	//mesh->Render();
	//mesh->GetMaterial().SetTexture("latexture", texture);

	/**
	 *  COMPUTE
	 */
	ComputePipeline* pip = (ComputePipeline*)GraphicsRenderer::GetInstance()->AddPipeline("./Examples/TestParticle.json");

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
		//glfwGetCursorPos(GraphicsInstance::GetInstance()->GetWindow(), &x, &y);
		//mesh->GetMaterial().SetVec3("cou.bondour", vec3<float>(((float)x / 1280) * 2 - 1, ((float)y / 720) * 2 - 1, 0.0f));

/*
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
		*/

		if (GraphicsRenderer::GetInstance()->IsChange())
		{
			GraphicsRenderer::GetInstance()->DestroyInvalids();
			GraphicsInstance::GetInstance()->FillCommandBuffer();
			GraphicsRenderer::GetInstance()->ResetChange();
		}

		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}

	//delete mesh;
	//delete meshCubeMap;
	//delete cubeMap;

	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
