#include <stdint.h>
#include <iostream>
#include "vulkan/vulkan.h"

#include <vector>
#include <functional>

#include "Utilities/Vertex.hpp"

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
		uint32_t		_mMinElements;
		uint8_t*		_mCpuBuffer;
		VmaAllocation				_mGpuMemory;
		std::vector<VkBuffer>		_mGpuBuffer;
		std::vector<Uniform>		_mUniforms;
		std::vector<Uniform>		_mUniqueUniforms;
		std::vector<UniformTexture> _mUniformsTexture;

		public:
		void AddUniform( Uniform uniform );
		void InitBuffers( void );
		void Set( std::string name, void* value );
		void DestroyAllUniforms( void );
		void UpdateToGPU( void );
	};
};