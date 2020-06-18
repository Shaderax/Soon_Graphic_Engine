#pragma once

#include "Math/vec3.hpp"
#include <string>

#include "Utilities/Error.hpp"

namespace Soon
{
	struct Material
	{
		uint32_t 		_id = Soon::IdError;
		std::string		m_PipelineName;
		BasePipeline*	m_Pipeline = nullptr;

		Material(void)
		{
		}

		virtual ~Material(void)
		{
		}

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
		virtual void SetUniform(std::string name, void *data) = 0;
		virtual void *GetUniform(std::string name) = 0;

		void SetVec3(std::string name, vec3<float> vec)
		{
			SetUniform(name, &vec);
		}

		vec3<float> GetVec3(std::string name)
		{
			void *data = GetUniform(name);
			return vec3<float>(); //_vec3s[name];
		}

		void SetUniform(std::string name, void *data)
		{
			if (m_Pipeline && _id != IdError)
				m_Pipeline->Set(name, data, _id);
		}

		void* GetUniform(std::string name)
		{
			if (m_Pipeline && _id != IdError)
				return m_Pipeline->Get(name, _id);
		}

		void SetTexture(std::string name, Texture &texture)
		{
			if (m_Pipeline && _id != IdError)
			{
				GraphicsRenderer::GetInstance()->AddTexture(&texture);
				std::cout << "Set Texture : " << texture.GetId() << std::endl;
				m_Pipeline->SetTexture(name, _id, texture.GetId());
			}
		}

		const BasePipeline* GetPipeline(void) const
		{
			return (m_Pipeline);
		}

		void SetPipeline(std::string name)
		{
			if (m_Pipeline)
				m_Pipeline->RemoveId(_id);
			m_Pipeline = GraphicsRenderer::GetInstance()->AddPipeline(name);
		}
	};
} // namespace Soon
