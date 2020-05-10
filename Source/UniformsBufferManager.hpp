#include <stdint.h>
#include <iostream>
#include "vulkan/vulkan.h"

#include <vector>
#include <functional>

#include "Vertex.hpp"

namespace Soon
{
	struct UniformVar
	{
		std::string _name;
		VertexElementType _type;
		uint32_t _size;
		uint32_t _offset;
	};

	struct Uniform
	{
		std::string _name;
		uint32_t _size;
		uint32_t _binding;
		uint32_t _set;
		std::vector<UniformVar> _members;
		//std::vector<UniformSets> _us;
		std::function<void(int)> _updateFunct;
		bool isUnique;
	};

	struct UniformTexture
	{
		UniformVar _data;
		uint32_t _binding;
		uint32_t _set;
		//std::vector<Image> _i;
		//std::vector<ImageRenderer> _iR;
		std::vector<VkDescriptorSet> _dS;
		std::function<void(int)> _updateFunct;
		bool isUnique;
	};

	class UniformsBufferManager
	{
	private:
		uint32_t		m_NumElement = 0;
		uint32_t		m_MinElements = 100;
		uint8_t*		m_CpuBuffer = nullptr;
		uint32_t		m_SizeUniformBlock = 0;
		std::vector<VmaAllocation>	m_GpuMemory;
		std::vector<VkBuffer>		m_GpuBuffer;
		std::vector<Uniform>		m_Uniforms;
		std::vector<Uniform>		m_UniqueUniforms;
		std::vector<UniformTexture> m_UniformsTexture;

	public:
		void InitBuffers( void );
		void DestroyAllUniforms( void );
		void AddUniform( Uniform uniform );
		void AddUniformTexture( UniformTexture uniform );
		void UpdateToGPU( uint32_t currentImg );
		void Set( std::string name, void* value );
	};
};