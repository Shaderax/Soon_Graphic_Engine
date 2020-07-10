#include "Mesh.hpp"

#include <assert.h>

namespace Soon
{
	Mesh::Mesh(MeshVertexDescription v) : RendererRessource(ERendererRessource::MESH, Soon::IdError)
	{
		m_VertexDescription = v;
	}

	Mesh::Mesh(const Mesh &other) : RendererRessource(ERendererRessource::MESH, other.GetId())
	{
	}

	Mesh::Mesh(const Mesh &&other) : RendererRessource(ERendererRessource::MESH, other.GetId())
	{
		//_vertices = other._vertices;
		//_indices = other._indices;
		// Set Vertex Description.
	}

	Mesh::~Mesh(void)
	{
		if (m_UniqueId != IdError)
			FreeGpu();
	}

	// Loader Free ?

	void Mesh::SetVertexElement(uint8_t *data, uint32_t size, MeshVertexElement elem)
	{
		assert((m_VertexDescription.HasElement(elem.sementic) && "Has Element"));

		if (!mVertexData.get())
		{
			mNumVertex = size;
			mVertexTotalSize = mNumVertex * m_VertexDescription.GetVertexStrideSize();
			mVertexData = std::shared_ptr<uint8_t>(new uint8_t[mVertexTotalSize](), std::default_delete<uint8_t[]>());
		}
		std::cout << std::endl
				  << "Elem : " << (int)elem.sementic << std::endl;
		uint32_t elementSize = elem.type.GetTypeSize(); // element size, 12
		std::cout << "elementSize : " << elementSize << std::endl;
		std::cout << "_numVertx : " << mNumVertex << std::endl;
		std::cout << "Allocate Size : " << mNumVertex * elem.type.row * elem.type.column << std::endl;
		uint32_t offset = m_VertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
		std::cout << "offset : " << offset << std::endl;
		uint32_t strideSize = m_VertexDescription.GetVertexStrideSize(); // Ex : 12oct
		std::cout << "strideSize : " << strideSize << std::endl;

		for (uint32_t index = 0; index < size; index++)
			memcpy(mVertexData.get() + (index * strideSize) + offset, data + (index * elementSize), elementSize);
	}

	// Material Handle
	void Mesh::SetMaterial(GraphicMaterial &material)
	{
		m_Material = material;
	}

	GraphicMaterial &Mesh::GetMaterial()
	{
		return (m_Material);
	}

	void Mesh::Render(void)
	{
		if (!m_Material.HasValidVertexDescription(m_VertexDescription))
		{
			std::cout << "Non Valid Mesh To Pipeline" << std::endl;
			return;
			// TODO: ERROR ?
		}
		if (m_UniqueId == Soon::IdError)
			AllocGpu();
		m_Material.Render(m_UniqueId);
	}
	//TODO SETMESH

	void Mesh::UnRender(void)
	{
		m_Material.UnRender();
	}

	void Mesh::AllocGpu(void)
	{
		m_UniqueId = GraphicsRenderer::GetInstance()->AddMesh(this, m_UniqueId);
	}

	void Mesh::FreeGpu(void)
	{
//		if (GraphicsRenderer::GetInstanceOrNull() != nullptr)
//			GraphicsRenderer::GetInstance()->RemoveMesh(m_UniqueId);
//		m_UniqueId = Soon::IdError;
	}

	void Mesh::SetIndexBuffer(uint32_t *indexData, uint32_t size)
	{
		// Need to free indexData
		mIndices = std::shared_ptr<uint32_t>(new uint32_t[size], std::default_delete<uint32_t[]>());
		memcpy(mIndices.get(), indexData, size * sizeof(uint32_t));
		mNumIndices = size;
	}
} // namespace Soon