#pragma once

#include "Math/vec3.hpp"
#include <string>

#include "Utilities/Error.hpp"

namespace Soon
{
	struct Material
	{
		Material( void )
		{

		}

		virtual ~Material( void )
		{

		}

/*
		void SetTexture( std::string name, Texture* texture )
		Texture* GetTexture( std::string name )

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

		float GetFloat( std::string name )
		{
			return 0.0f;//_floats[name]
		}
*/
		virtual void SetUniform( std::string name, void* data ) = 0;
		virtual void* GetUniform( std::string name ) = 0;
		
		void SetVec3( std::string name, vec3<float> vec )
		{
			SetUniform(name, &vec);
		}

		vec3<float> GetVec3( std::string name )
		{
			void* data = GetUniform(name);
			return vec3<float>();//_vec3s[name];
		}

		uint32_t			_id = IdError;
		std::string		m_PipelineName;
		//BasePipeline* _pipeline;
	};
}
