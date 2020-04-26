#pragma once

#include "Math/vec3.hpp"
#include <unordered_map>
#include <string>

#include "Renderer/Texture.hpp"
#include "Renderer/Pipelines/BasePipeline.hpp"

#include "Renderer/Vulkan/GraphicsRenderer.hpp"

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

		void SetTexture( std::string name, Texture* texture )
		{
			//_pipeline->Set<Texture*>();
			//_textures[name] = texture;
		}

		Texture* GetTexture( std::string name )
		{
			Texture* tt = nullptr;
			try
			{
				//tt = _textures.at(name);
			}
			catch (const std::out_of_range& oor) {
				std::cerr << "For " << name << " Out of Range Get Texture error: " << oor.what() << '\n';
			}
			return tt;
		}

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

		void SetVec3( std::string name, vec3<float> vec )
		{
			//_vec3s[name] = vec;
		}

/*
		vec3<float>& GetVec3( std::string name )
		{
			return vec3<float>();//_vec3s[name];
		}
		*/
		// Maybe not and use shaderpipeline
			template<class T>
				void SetPipeline( void )
				{
					if (_pipeline)
						_pipeline->RemoveFromPipeline(_id);
					_pipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
				}

		uint32_t			_id;
		BasePipeline* _pipeline;
	};
}
