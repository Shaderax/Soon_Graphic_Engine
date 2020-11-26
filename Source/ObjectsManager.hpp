#include <vector>
#include "Utilities/Error.hpp"

namespace Soon
{
	template<typename T>
	class ObjectRenderer : public T
	{
	public:
		uint32_t mId;
	};

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
				m_Objects[id] = ObjectRenderer<T>();
				return m_Objects[id];
			}
			else
			{
				m_Objects.emplace_back();
				m_Objects.back().mId = m_Objects.size() - 1;
				return static_cast<T&>(m_Objects.back());
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
		std::vector<ObjectRenderer<T>> m_Objects;
		std::vector<uint32_t> m_FreeIds;
	};
} // namespace Soon

/*
struct RenderFrameBuffer
{
	Framebuffer framebuffer;
	std::vector<RenderRenderpass> renderpass;
};

struct RenderRenderPass
{
	RenderPass renderpass;
	std::vector<Subpass> subpass;
};

struct RenderSubpass
{
	Subpass subpass;
	std::vector<BasePipeline> pipeliens;
};

class Renderer
{
	std::unordered_map<RenderPass, uint32_t> m_MapRenderpass;
	ObjectManager<Framebuffer> m_Framebuffer;
	ObjectsManager<RenderPass> m_Renderpass;
	ObjectsManager<Subpass> m_Subpass;
	ObjectsManager<BasePipeline> m_Renderpass;

	std::vector<RenderFramebuffer> m_BasePipeline;

	BasePipeline* AddPipeline("./Ex/Kek.json")
	{
		PipelineConf* conf = ReadPipelineJson(name);
		BasePipeline* pip = new ComputePipeline(conf);
		pip->Init(); // Ici c'est load. // Need to tej Init()
		return pip;
	}
};

-----------
Comment ça se passe genre avant de créer la VkRenderPass je veux savoir si y'en a deja une comme ça donc je veux faire un hash de ma atuel qui me prends pas en compte la Vk
et 'ai une map de render pass de hash to *renderpass.

C'est possible d'avoir 2 shadow map mais la 2e c'est la prépasse depth et genre la render pass est la meme mais pas la frame buffer ?
	Alors on est dans une autre boucle d'une framebuffer avec une meme renderpass, mais pas avaec les mêmes BasePipeline.

Du coup je peux avoir plsierus render pass identique mais qui ont des framebuffer different, sinon ca veut dire que ca se fait dans la pipeline mais fautdrait les tier 
pour avoir un begin renderpass etc

Framebuffer
	vec Renderpass
		vec Subpass
			vec Pipeline

Renderpass = renderpass
	got subpass but no pipelines

SubPass = subpass
	got all inside but not pipeline ?

2 Framebuffer qui ont la meme renderpass ??
Framebuffer[0]->RenderPass[0]
Framebuffer[1]->RenderPass[0]

RenderPass[0]
	SubPass[0]
		Pipelines[0]

Pip = Renderer->AddPipeline("Kek.json");
				{
					conf = ReadPipelineJson(); // faut aussi la conf de la renderpass, le framebuffer est a part et les sub sont dans la renderp
					Pip = new BasePipeline(conf);
				}
Pip->Init(); // Need Tej Init()
{

}


--------------------- QA ---------------------

Tej Init pour set la renderpass a l'ext
Faut aussi connaitre la conf de la renderpass demandé par la pipeline.
peut on mettre n'importe quelle pipeline dans une renderpass ou inverse.

Et pour les computes ? genre elles ont pas forcement de framebuffer. Bah c'est meme pas ça elles en ont pas du tout.
*/
