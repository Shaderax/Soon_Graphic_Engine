#include "Math/vec3.hpp"
#include <string>

#include "Utilities/Error.hpp"

#include "Materials/Material.hpp"
#include "Texture.hpp"
#include "GraphicsRenderer.hpp"

namespace Soon
{
	Material::Material(void)
	{
	}

	Material::~Material(void)
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
	void Material::SetVec3(std::string name, vec3<float> vec)
	{
		SetUniform(name, &vec);
	}

	vec3<float> Material::GetVec3(std::string name)
	{
		void *data = GetUniform(name);
		return vec3<float>(); //_vec3s[name];
	}

	void Material::SetUniform(std::string name, void *data)
	{
		if (m_Pipeline && _id != IdError)
			m_Pipeline->Set(name, data, _id);
	}

	void *Material::GetUniform(std::string name)
	{
		if (m_Pipeline && _id != IdError)
			return m_Pipeline->Get(name, _id);
	}

	void Material::SetTexture(std::string name, Texture &texture)
	{
		if (m_Pipeline && _id != IdError)
		{
			GraphicsRenderer::GetInstance()->AddTexture(&texture);
			std::cout << "Set Texture : " << texture.GetId() << std::endl;
			m_Pipeline->SetTexture(name, _id, texture.GetId());
			// TODO: REMOVE TEXTURE ?
		}
	}

	const BasePipeline *Material::GetPipeline(void) const
	{
		return (m_Pipeline);
	}

	void Material::SetPipeline(std::string name)
	{
		if (m_Pipeline && _id != Soon::IdError)
			m_Pipeline->RemoveId(_id);
		m_Pipeline = GraphicsRenderer::GetInstance()->AddPipeline(name);
	}
} // namespace Soon
