#pragma once

#include "Math/vec3.hpp"
#include <string>

#include "Utilities/Error.hpp"

#include "Texture.hpp"

namespace Soon
{
	class BasePipeline;

	class Material
	{
	protected:
		uint32_t 		_id = Soon::IdError;
		BasePipeline*	m_Pipeline = nullptr;
	public:

		Material(void);
		virtual ~Material(void);

		/*
		void SetFloat( std::string name, float value, uint32_t arrayId = 0 )
		{
			//_floats[name] = value;
		}

		void SetInt( std::string name, int value )
		{
			//_pipeline->Set<int>(name, VertexElementType(EnumVertexElementType::INT, 1, 1), value, _id);
		}

		// int[2]
		void SetArray( std::string name, uint8_t* data, VertexElementType info )
		{
			//_pipeline.Set<uint8_t*>(name, info, data);
		}
*/
		void SetVec3(std::string name, vec3<float> vec);
		vec3<float> GetVec3(std::string name);
		void SetUniform(std::string name, void *data);
		void* GetUniform(std::string name);
		void SetTexture(std::string name, Texture &texture);
		BasePipeline* GetPipeline(void) const;
		void SetPipeline(std::string name);
		void CreateId(void);
		void Set(std::string name, void* value);
		uint32_t GetId(void);

		// TODO: Put All Desconstruct as virtual
	};
} // namespace Soon
