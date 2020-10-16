#include <vector>
#include "Utilities/Error.hpp"

namespace Soon
{
	template <typename T>
	class ObjectsManager
	{
	public:
		ObjectsManager(void);
		~ObjectsManager(void);

		ObjectsManager(const ObjectsManager &obj);
		ObjectsManager(const ObjectsManager &&obj);

		ObjectsManager &operator=(const ObjectManager &obj);
		ObjectsManager &operator=(ObjectManager &&obj);

		T &Get(std::uint32_t id);

		template <typename... N>
		T& GetNew(N... args)
		{
			std::uint32_t id;

			if (!m_FreeIds.empty())
			{
				id = m_FreeIds.back();
				m_FreeIds.pop_back();
			}
			else
			{
				id = m_ObjectsCounter;
				++m_ObjectsCounter;
				m_Objects.resize(m_ObjectsCounter);
			}

			MeshRenderer mr;
			mr.count = 1;
			// TODO: Delete mVertexData for all in Gpu
			mr.bufferRenderer.vertex = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->mVertexData.get(), mesh->mVertexTotalSize);
			mr.bufferRenderer.indices = GraphicsInstance::GetInstance()->CreateIndexBuffer(mesh->mIndices.get(), mesh->mNumIndices);
			_meshs[meshId] = mr;

			std::cout << "MeshId: " << meshId << ", Count: " << _meshs[meshId].count << std::endl;

			return (meshId);
			m_Objects.emplace_back(args);
			return m_Objects.back();
		}

	private:
		std::vector<T> m_Objects;
		std::uint32_t m_ObjectsCounter = 0;
		std::vector<uint32_t> m_FreeIds;
	};
} // namespace Soon

/*
class RenderPass
{
	ObjectsManager<Subpass> m_Subpass;
};

class Subpass
{
	ObjectsManager<BasePipeline> pipelines;
};

class GraphicRenderer
{
	std::unordered_map<RenderPass, RenderPass*> mapRenderP;
	ObjectsManager<RenderPass> m_Renderpass;

	BasePipeline* AddPipeline("./Ex/Kek.json")
	{
		PipelineConf* conf = ReadPipelineJson(name);
		BasePipeline* pip = new ComputePipeline(conf);
		pip->Init(); // Ici c'est load.
		return pip;
	}
};

-----------
Quand est attribué ma RenderPass ?
Elle change en fonction du nombre de subpass, ya les load etc
Donc je dois overload les strucs et faire un overload de == ou faire du hash
*/