#include "UniformsBufferManager.hpp"
#include "Renderer/Vulkan/GraphicsRenderer.hpp"

namespace Soon
{
	void UniformsBufferManager::AddUniform(Uniform uniform)
	{
		_mUniforms.push_back(uniform);
	}

	void UniformsBufferManager::InitBuffers(void)
	{
	}

	void UniformsBufferManager::Set(std::string name, void *value)
	{
	}

	void UniformsBufferManager::DestroyAllUniforms(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (uint32_t idChain = 0; idChain < GraphicsInstance::GetInstance()->GetSwapChainSize(); idChain++)
			vkDestroyBuffer(device, _mGpuBuffer[idChain], nullptr);
	}

	void UniformsBufferManager::UpdateToGPU( void )
	{

	}
}; // namespace Soon