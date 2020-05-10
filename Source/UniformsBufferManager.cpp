#include "UniformsBufferManager.hpp"
#include "GraphicsRenderer.hpp"

namespace Soon
{
	void UniformsBufferManager::AddUniform(Uniform uniform)
	{
		m_Uniforms.push_back(uniform);
	}

	void UniformsBufferManager::AddUniformTexture( UniformTexture uniform )
	{

	}

	void UniformsBufferManager::InitBuffers(void)
	{
		uint32_t swapChainSize = GraphicsInstance::GetInstance()->GetSwapChainSize();
		m_GpuMemory.resize(swapChainSize);
		m_GpuBuffer.resize(swapChainSize);
	}

	void UniformsBufferManager::Set(std::string name, void *value)
	{
	}

	void UniformsBufferManager::DestroyAllUniforms(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (uint32_t idChain = 0; idChain < GraphicsInstance::GetInstance()->GetSwapChainSize(); idChain++)
			vkDestroyBuffer(device, m_GpuBuffer[idChain], nullptr);
	}

	void UniformsBufferManager::UpdateToGPU( uint32_t currentImg )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkMapMemory(device, _uniforms[0]._us[0]._uniformRender.bufferMemory[currentImg], 0, _uniforms[0]._members[0]._size, 0, &data);
		memcpy(data, &vec, _uniforms[0]._members[0]._size);
		vkUnmapMemory(device, _uniforms[0]._us[0]._uniformRender.bufferMemory[currentImg]);
	}
}; // namespace Soon