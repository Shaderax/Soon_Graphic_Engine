#include "Mesh.hpp"

#include <assert.h>

namespace Soon
{
	Mesh::Mesh(MeshVertexDescription v) : RendererRessource(ERendererRessource::MESH, Soon::IdError)
	{
		m_VertexDescription = v;
		m_Material = std::make_shared<GraphicMaterial>();
	}

	Mesh::Mesh(const Mesh &other) : RendererRessource(ERendererRessource::MESH, other.GetId())
	{
		m_VertexDescription = other.m_VertexDescription;
		mVertexData = other.mVertexData;
		mIndices = other.mIndices;
		mNumIndices = other.mNumIndices;
		mNumVertex = other.mNumVertex;
		mVertexTotalSize = other.mVertexTotalSize;
		m_Material = other.m_Material;
	}

	Mesh::Mesh(Mesh &&other) : RendererRessource(ERendererRessource::MESH, Soon::IdError)
	{
		m_VertexDescription = other.m_VertexDescription;
		mVertexData = std::move(other.mVertexData);
		mIndices = std::move(other.mIndices);
		mNumIndices = std::exchange(other.mNumIndices, 0);
		mNumVertex = std::exchange(other.mNumVertex, 0);;
		mVertexTotalSize = std::exchange(other.mVertexTotalSize, 0);
		m_Material = other.m_Material;

		m_Material = std::move(other.m_Material);
		m_UniqueId = other.m_UniqueId;

		other.m_UniqueId = Soon::IdError;
	}

	Mesh& Mesh::operator=(Mesh&& other)
	{
		if (this == &other)
			return *this;
		
		m_VertexDescription = other.m_VertexDescription;
		mVertexData = std::move(other.mVertexData);
		mIndices = std::move(other.mIndices);
		mNumIndices = std::exchange(other.mNumIndices, 0);
		mNumVertex = std::exchange(other.mNumVertex, 0);;
		mVertexTotalSize = std::exchange(other.mVertexTotalSize, 0);
		m_Material = std::move(other.m_Material);

		if (m_UniqueId != Soon::IdError)
			GraphicsRenderer::GetInstance()->RemoveMesh(m_UniqueId);

		m_UniqueId = std::exchange(other.m_UniqueId, Soon::IdError);

		return *this;
	}

	Mesh& Mesh::operator=(const Mesh& other)
	{
		if (this == &other)
			return *this;

		m_VertexDescription = other.m_VertexDescription;
		mVertexData = other.mVertexData;
		mIndices = other.mIndices;
		mNumIndices = other.mNumIndices;
		mNumVertex = other.mNumVertex;
		mVertexTotalSize = other.mVertexTotalSize;
		m_Material = other.m_Material;

		if (m_UniqueId != Soon::IdError)
			GraphicsRenderer::GetInstance()->RemoveMesh(m_UniqueId);
		m_UniqueId = other.m_UniqueId;
		GraphicsRenderer::GetInstance()->AddBuffer(m_UniqueId);
		
		return *this;
	}

	Mesh::~Mesh(void)
	{
	}

	void Mesh::SetVertexElement(uint8_t *data, uint32_t size, MeshVertexElement elem)
	{
		assert((m_VertexDescription.HasElement(elem.sementic) && "Has Element"));

		//std::cout << (void*)data << std::endl;
		//std::cout << size << std::endl;

		if (!mVertexData.get())
		{
			mNumVertex = size;
			mVertexTotalSize = mNumVertex * m_VertexDescription.GetVertexStrideSize();
			mVertexData = std::shared_ptr<uint8_t>(new uint8_t[mVertexTotalSize](), std::default_delete<uint8_t[]>());
		}
		//std::cout << std::endl
		//		  << "Elem : " << (int)elem.sementic << std::endl;
		uint32_t elementSize = elem.type.GetTypeSize(); // element size, 12
		//std::cout << "elementSize : " << elementSize << std::endl;
		//std::cout << "_numVertx : " << mNumVertex << std::endl;
		//std::cout << "Allocate Size : " << mNumVertex * elem.type.row * elem.type.column << std::endl;
		uint32_t offset = m_VertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
		//std::cout << "offset : " << offset << std::endl;
		uint32_t strideSize = m_VertexDescription.GetVertexStrideSize(); // Ex : 12oct
		//std::cout << "strideSize : " << strideSize << std::endl;

		for (uint32_t index = 0; index < size; index++)
			memcpy(mVertexData.get() + (index * strideSize) + offset, data + (index * elementSize), elementSize);
	}

	// Material Handle
	void Mesh::SetMaterial(GraphicMaterial &material)
	{
		m_Material = std::make_shared<GraphicMaterial>(material);
	}

	GraphicMaterial& Mesh::GetMaterial()
	{
		return (*m_Material.get());
	}

	void Mesh::Render(void)
	{
		if (!m_Material.get()->HasValidVertexDescription(m_VertexDescription))
		{
			std::cout << "Non Valid Mesh To Pipeline" << std::endl;
			MeshVertexDescription des = ((GraphicPipeline*)m_Material.get()->GetPipeline())->GetMeshVertexDescription();
			std::cout << "Pipeline" << std::endl;
			for (auto& a : des.data)
			{
				DEBUG(a.mOffset, " ", (int)a.sementic, " ", (int)a.type.baseType, " ", a.type.column, " ", a.type.row);
			}
			std::cout << "Mesh" << std::endl;
			for (auto& a : m_VertexDescription.data)
			{
				DEBUG(a.mOffset, " ", (int)a.sementic, " ", (int)a.type.baseType, " ", a.type.column, " ", a.type.row);
			}

			return;
			// TODO: ERROR ?
		}
		if (m_UniqueId == Soon::IdError)
			AllocGpu();
		m_Material.get()->Render(m_UniqueId);
	}
	//TODO SETMESH

	void Mesh::UnRender(void)
	{
		m_Material.get()->UnRender();
	}

	void Mesh::AllocGpu(void)
	{
		if (m_UniqueId == Soon::IdError)
			m_UniqueId = GraphicsRenderer::GetInstance()->AddMesh(this, m_UniqueId);
	}

	void Mesh::SetIndexBuffer(uint32_t *indexData, uint32_t size)
	{
		// Need to free indexData
		mIndices = std::shared_ptr<uint32_t>(new uint32_t[size], std::default_delete<uint32_t[]>());
		memcpy(mIndices.get(), indexData, size * sizeof(uint32_t));
		mNumIndices = size;
	}
} // namespace Soon