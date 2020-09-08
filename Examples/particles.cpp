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

#include "Configure.h"

using namespace Soon;

/**
 * OBJ LOADER
 */

Mesh* ObjLoader(void)
{
	MeshVertexDescription vd;
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	//vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	Mesh* mesh = new Mesh(vd);

	std::string inputfile = "./Examples/Cube copy.obj";
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
	//mesh->SetVertexElement((uint8_t*)attrib.normals.data(), attrib.normals.size() / 3, MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	std::vector<uint32_t> indices;
	for (uint32_t index = 0 ; index < shapes[0].mesh.indices.size() ; index++)
		indices.push_back(shapes[0].mesh.indices[index].vertex_index);
	mesh->SetIndexBuffer(indices.data(), indices.size());

	return mesh;
}

#include "Utilities/Log.hpp"

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();

	DEBUG(42);
	Log("Coucou");

	/**
	 * Texture 
	 */
	Texture texture;
	int channel, width, height;
	unsigned char *data = stbi_load("/home/shaderax/Documents/Project/Late_Night/Ressources/Pixel/sprite_15.png", &width, &height, &channel, 4);
	texture.SetData(data, width, height, TextureFormat(EnumTextureFormat::RGBA));
	texture.SetFilterMode(EnumFilterMode::NEAREST);
	stbi_image_free(data);

	/**
	 * MESH
	 */
	Mesh* mesh = ObjLoader();
	mesh->AllocGpu();

	/**
	 *  COMPUTE
	 */
	uint32_t idComp;
	uint32_t idGr;
	uint32_t idInit;
	uint32_t numParticles = 40;

	/*INIT*/
	ComputePipeline* initPip = (ComputePipeline*)GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/InitParticles.json");
	initPip->SetProcessFrequency(EProcessFrequency::ONCE);
	idInit = initPip->CreateNewId();
	//initPip->Set("Particles.num", &numParticles, idInit); // TODO:
	initPip->SetRuntimeAmount("Particles.particles", numParticles, idInit);
	initPip->SetWorkGroup(idInit, numParticles, 1, 1);
	initPip->Process(idInit);
	initPip->Dispatch();
//
	UniformRuntime& uniform = initPip->GetUniformRuntime("Particles");
	GpuBuffer& buffer = uniform.mBuffers[idInit];
	buffer.m_Offset = 16;
//
	/*ParticleLoop*/
	ComputePipeline* pip = (ComputePipeline*)GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/TestParticle.json");
	idComp = pip->CreateNewId();
			//GpuBuffer& buffer = initPip->GetRuntimeBuffer();
	pip->SetRuntimeBuffer("Particles", buffer, idComp);
	UniformRuntime& uniformPa = pip->GetUniformRuntime("Particles");
	pip->SetRuntimeAmount("Particles.particles", numParticles, idComp);
	pip->SetWorkGroup(idComp, numParticles, 1, 1);
	pip->Process(idComp);

	/*Graphic*/
	VertexDescription description = uniformPa.GetVertexDescription({"particles.position"}, idInit);
	//std::cout << description.strideSize << std::endl;
	//std::cout << description.mBaseOffset << std::endl;
	GraphicPipeline* grPip = (GraphicPipeline*)GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/NewDefaultPipeline.json");
	idGr = grPip->CreateNewId();
	grPip->SetMeshId(idGr, mesh->GetId());
	grPip->SetAttributeDescriptionOffset(1, description);
	grPip->SetBindingVertexInput(idGr, 1, buffer);
	grPip->SetNumInstance(idGr, numParticles);
	GraphicsRenderer::GetInstance()->AddTexture(texture);
	grPip->Render(idGr);
	grPip->SetTexture("latexture", idGr, texture.GetId());

	double lastTime = 0;

	// Loop
	Log("Begin Loop");
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		lastTime = ShowFPS(lastTime);

		GraphicsRenderer::GetInstance()->Update();
		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}

	delete mesh;

	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
