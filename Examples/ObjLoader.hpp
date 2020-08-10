#include <vector>
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

using namespace Soon;

struct Indices
{
	uint32_t vertex;
	uint32_t normal;
	uint32_t texCoord;

	bool operator==(const Indices& other) const 
	{
	    return vertex == other.vertex && normal == other.normal && texCoord == other.texCoord;
	}
};
namespace std {
    template<> struct hash<Indices> {
        size_t operator()(Indices const& vertex) const {
            return ((hash<uint32_t>()(vertex.vertex) ^
                   (hash<uint32_t>()(vertex.normal) << 1)) >> 1) ^
                   (hash<uint32_t>()(vertex.texCoord) << 1);
        }
    };
}

std::vector<Mesh>* ObjLoader(std::string inInputFile, std::string inMaterialPath = "", std::string inPipeline = "")
{
	std::vector<Mesh>* meshArray = new std::vector<Mesh>();

	MeshVertexDescription vd;
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	//std::string inputfile = "./Ressources/Cobra/Shelby.obj";
	//std::string inputfile = "./Ressources/Sponza/sponza.obj";
	//std::string inputfile = "./Examples/Cube.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inInputFile.c_str(), inMaterialPath.c_str());
	//bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), "./Ressources/Sponza/");
	//bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;

	if (!err.empty())
		std::cerr << err << std::endl;

	if (!ret)
		exit(1);

	// Default material
	materials.push_back(tinyobj::material_t());

	std::unordered_map<Indices, uint32_t> mapper;
	std::unordered_map<std::string, Texture> textureMap;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		meshArray->push_back(std::move(Mesh(vd)));
		if (!inPipeline.empty())
			meshArray->back().GetMaterial().SetPipeline(inPipeline);
		meshArray->back().GetMaterial().CreateId();

		std::vector<float> v;
		std::vector<float> n;
		std::vector<float> t;

		std::vector<uint32_t> indicesBuffer;

		uint32_t id = 0;
		for (uint32_t index = 0 ; index < shapes[s].mesh.indices.size() ; index++)
		{
			Indices indices;
			tinyobj::index_t indexId = shapes[s].mesh.indices[index];
			int currentMaterialId = shapes[s].mesh.material_ids[index / 3];

			indices.vertex = indexId.vertex_index;
			indices.normal = indexId.normal_index;
			indices.texCoord = indexId.texcoord_index;

			if ((currentMaterialId < 0) ||
            	(currentMaterialId >= static_cast<int>(materials.size())))
			{
				// Invaid material ID. Use default material.
				currentMaterialId = materials.size() - 1;  // Default material is added to the last item in `materials`.
        	}

			std::string path = inMaterialPath;
			path += materials[currentMaterialId].ambient_texname;
			if (textureMap.count(path) == 0)
			{
				Texture texture;
				int channel, width, height;
				//unsigned char *data = stbi_load(path.c_str(), &width, &height, &channel, 4);
				unsigned char *data = nullptr;
				if (data != nullptr)
				{
					texture.SetData(data, width, height, TextureFormat(EnumTextureFormat::RGBA));
					texture.SetFilterMode(EnumFilterMode::NEAREST);
					stbi_image_free(data);
					textureMap[path] = std::move(texture);
				}
			}
			if (textureMap.count(path) > 0)
				meshArray->back().GetMaterial().SetTexture("latexture", textureMap[path]);
			glm::vec3 ambient(0.3f, 0.3f, 0.3f);
			meshArray->back().GetMaterial().Set("cr.ambient", &ambient);

			if (mapper.count(indices) == 0)
			{
				mapper[indices] = id;
				v.push_back(attrib.vertices[3 * indices.vertex + 0]);
				v.push_back(attrib.vertices[3 * indices.vertex + 1]);
				v.push_back(attrib.vertices[3 * indices.vertex + 2]);

				if (attrib.normals.size() > 0)
				{
					n.push_back(attrib.normals[3 * indices.normal + 0]);
					n.push_back(attrib.normals[3 * indices.normal + 1]);
					n.push_back(attrib.normals[3 * indices.normal + 2]);
				}

				if (attrib.texcoords.size() > 0)
				{
					if (indexId.texcoord_index >= 0 && indexId.texcoord_index < attrib.texcoords.size())
					{
						t.push_back(attrib.texcoords[2 * indices.texCoord + 0]);
						t.push_back(attrib.texcoords[2 * indices.texCoord + 1]);
					}
					else
					{
						t.push_back(0.0f);
						t.push_back(0.0f);
					}
				}
				++id;
			}
			indicesBuffer.push_back(mapper[indices]);
		}

		meshArray->back().SetVertexElement((uint8_t*)v.data(), v.size() / 3, MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
		meshArray->back().SetVertexElement((uint8_t*)n.data(), n.size() / 3, MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
		meshArray->back().SetVertexElement((uint8_t*)t.data(), t.size() / 2, MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));

		meshArray->back().SetIndexBuffer((uint32_t*)indicesBuffer.data(), indicesBuffer.size());
	}

	return meshArray;
}


////////////////

/*

Mesh LoadCube(void)
{
	MeshVertexDescription vd;
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	Mesh mesh(vd);
	mesh.GetMaterial().CreateId();
	std::string inputfile = "./Examples/Cube.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;

	if (!err.empty())
		std::cerr << err << std::endl;

	if (!ret)
		exit(1);

	std::unordered_map<Indices, uint32_t> mapper;

	std::vector<float> v;
	std::vector<float> n;
	std::vector<float> t;
	std::vector<uint32_t> indicesBuffer;

	uint32_t id = 0;
	for (uint32_t index = 0 ; index < shapes[0].mesh.indices.size() ; index++)
	{
		Indices indices;
		tinyobj::index_t indexId = shapes[0].mesh.indices[index];

		indices.vertex = indexId.vertex_index;
		indices.normal = indexId.normal_index;
		indices.texCoord = indexId.texcoord_index;

		if (mapper.count(indices) == 0)
		{
			mapper[indices] = id;
			v.push_back(attrib.vertices[3 * indices.vertex + 0]);
			v.push_back(attrib.vertices[3 * indices.vertex + 1]);
			v.push_back(attrib.vertices[3 * indices.vertex + 2]);

			if (attrib.normals.size() > 0)
			{
				n.push_back(attrib.normals[3 * indices.normal + 0]);
				n.push_back(attrib.normals[3 * indices.normal + 1]);
				n.push_back(attrib.normals[3 * indices.normal + 2]);
			}

			if (attrib.texcoords.size() > 0)
			{
				if (indexId.texcoord_index >= 0 && indexId.texcoord_index < attrib.texcoords.size())
				{
					t.push_back(attrib.texcoords[2 * indices.texCoord + 0]);
					t.push_back(attrib.texcoords[2 * indices.texCoord + 1]);
				}
				else
				{
					t.push_back(0.0f);
					t.push_back(0.0f);
				}
			}
			++id;
		}
	indicesBuffer.push_back(mapper[indices]);
	}

	mesh.SetVertexElement((uint8_t*)v.data(), v.size() / 3, MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	mesh.SetVertexElement((uint8_t*)n.data(), n.size() / 3, MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	mesh.SetVertexElement((uint8_t*)t.data(), t.size() / 2, MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));

	mesh.SetIndexBuffer((uint32_t*)indicesBuffer.data(), indicesBuffer.size());

	return mesh;
}


std::vector<Mesh>* ObjLoader(void)
{
	std::vector<Mesh>* meshArray = new std::vector<Mesh>();;

	MeshVertexDescription vd;
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));
	vd.AddVertexElement(MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));

	std::string inputfile = "./Ressources/Cobra/Shelby.obj";
	//std::string inputfile = "./Ressources/Sponza/sponza.obj";
	//std::string inputfile = "./Examples/Cube.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), "./Ressources/Cobra/");
	//bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), "./Ressources/Sponza/");
	bool ggg = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;

	if (!err.empty())
		std::cerr << err << std::endl;

	if (!ret)
		exit(1);

	// Default material
	materials.push_back(tinyobj::material_t());

	std::unordered_map<Indices, uint32_t> mapper;
	std::unordered_map<std::string, Texture> textureMap;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		meshArray->push_back(std::move(Mesh(vd)));
		meshArray->back().GetMaterial().SetPipeline("./Examples/postProcessing.json");
		meshArray->back().GetMaterial().CreateId();

		std::vector<float> v;
		std::vector<float> n;
		std::vector<float> t;

		std::vector<uint32_t> indicesBuffer;

		uint32_t id = 0;
		for (uint32_t index = 0 ; index < shapes[s].mesh.indices.size() ; index++)
		{
			Indices indices;
			tinyobj::index_t indexId = shapes[s].mesh.indices[index];
			int currentMaterialId = shapes[s].mesh.material_ids[index / 3];

			indices.vertex = indexId.vertex_index;
			indices.normal = indexId.normal_index;
			indices.texCoord = indexId.texcoord_index;

			if ((currentMaterialId < 0) ||
            	(currentMaterialId >= static_cast<int>(materials.size())))
			{
				// Invaid material ID. Use default material.
				currentMaterialId = materials.size() - 1;  // Default material is added to the last item in `materials`.
        	}

			std::string path = "./Ressources/Cobra/";
			path += materials[currentMaterialId].ambient_texname;
			if (textureMap.count(path) == 0)
			{
				Texture texture;
				int channel, width, height;
				//unsigned char *data = stbi_load(path.c_str(), &width, &height, &channel, 4);
				unsigned char *data = nullptr;
				if (data != nullptr)
				{
					texture.SetData(data, width, height, TextureFormat(EnumTextureFormat::RGBA));
					texture.SetFilterMode(EnumFilterMode::NEAREST);
					stbi_image_free(data);
					textureMap[path] = std::move(texture);
				}
			}
			if (textureMap.count(path) > 0)
				meshArray->back().GetMaterial().SetTexture("latexture", textureMap[path]);
			glm::vec3 ambient(0.3f, 0.3f, 0.3f);
			meshArray->back().GetMaterial().Set("cr.ambient", &ambient);

			if (mapper.count(indices) == 0)
			{
				mapper[indices] = id;
				v.push_back(attrib.vertices[3 * indices.vertex + 0]);
				v.push_back(attrib.vertices[3 * indices.vertex + 1]);
				v.push_back(attrib.vertices[3 * indices.vertex + 2]);

				if (attrib.normals.size() > 0)
				{
					n.push_back(attrib.normals[3 * indices.normal + 0]);
					n.push_back(attrib.normals[3 * indices.normal + 1]);
					n.push_back(attrib.normals[3 * indices.normal + 2]);
				}

				if (attrib.texcoords.size() > 0)
				{
					if (indexId.texcoord_index >= 0 && indexId.texcoord_index < attrib.texcoords.size())
					{
						t.push_back(attrib.texcoords[2 * indices.texCoord + 0]);
						t.push_back(attrib.texcoords[2 * indices.texCoord + 1]);
					}
					else
					{
						t.push_back(0.0f);
						t.push_back(0.0f);
					}
				}
				++id;
			}
			indicesBuffer.push_back(mapper[indices]);
		}

		meshArray->back().SetVertexElement((uint8_t*)v.data(), v.size() / 3, MeshVertexElement(EnumVertexElementSementic::POSITION, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
		meshArray->back().SetVertexElement((uint8_t*)n.data(), n.size() / 3, MeshVertexElement(EnumVertexElementSementic::NORMAL, VertexElementType(EnumVertexElementType::FLOAT, 1, 3)));
		meshArray->back().SetVertexElement((uint8_t*)t.data(), t.size() / 2, MeshVertexElement(EnumVertexElementSementic::TEXCOORD, VertexElementType(EnumVertexElementType::FLOAT, 1, 2)));

		meshArray->back().SetIndexBuffer((uint32_t*)indicesBuffer.data(), indicesBuffer.size());
	}

	return meshArray;
}
*/