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

	void Material::CreateId(void)
	{
		if (m_Pipeline && _id == Soon::IdError)
			_id = m_Pipeline->CreateNewId();
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
	void Material::SetVec3(std::string name, glm::vec3 vec)
	{
		SetUniform(name, &vec);
	}

	glm::vec3 Material::GetVec3(std::string name)
	{
		void *data = GetUniform(name);

		glm::vec3 outVector;
		memcpy(&outVector, data, sizeof(glm::vec3));

		return outVector;
	}

	void Material::SetUniform(std::string name, void *data)
	{
		if (m_Pipeline && _id != IdError)
			m_Pipeline->Set(name, data, _id);
	}

	void* Material::GetUniform(std::string name)
	{
		if (m_Pipeline && _id != IdError)
			return m_Pipeline->Get(name, _id);
	}

	void Material::SetTexture(std::string name, Texture &texture)
	{
		if (m_Pipeline && _id != IdError)
		{
			GraphicsRenderer::GetInstance()->AddTexture(texture);
			m_Pipeline->SetTexture(name, _id, texture.GetId());
			// TODO: REMOVE TEXTURE ?
		}
		else if (m_Pipeline && _id == IdError)
		{
			GraphicsRenderer::GetInstance()->AddTexture(texture);
			_id = m_Pipeline->CreateNewId();
			m_Pipeline->SetTexture(name, _id, texture.GetId());
		}
	}

	BasePipeline *Material::GetPipeline(void) const
	{
		return (m_Pipeline);
	}

	void Material::SetPipeline(std::string name)
	{
		if (m_Pipeline && _id != Soon::IdError)
			m_Pipeline->RemoveId(_id);
		_id = Soon::IdError;
		m_Pipeline = GraphicsRenderer::GetInstance()->AddPipeline(name);
	}

	// TODO Should be private ?
	void Material::Set(std::string name, void* value)
	{
		if (m_Pipeline && _id != Soon::IdError)
			m_Pipeline->Set(name, value, _id);
	}

	uint32_t Material::GetId(void)
	{
		return _id;
	}
	
} // namespace Soon
