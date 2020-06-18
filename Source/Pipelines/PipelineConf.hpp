#pragma once

#include "Pipelines/PipelineJson.hpp"
#include <unordered_map>

 #include <functional>

namespace Soon
{
    enum class EPipelineType : uint8_t
	{
		GRAPHIC = 0,
		COMPUTE = 1
	};

	enum class EPipelineStage : uint8_t
	{
		VERTEX = 1,
		FRAGMENT = 2,
		TESSELLATION = 4,
		COMPUTE = 8,
		GEOMETRY = 16
	};

	VkShaderStageFlagBits PipelineStageToVk( EPipelineStage stage )
	{
		switch (stage)
		{
			case EPipelineStage::VERTEX:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case EPipelineStage::FRAGMENT:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case EPipelineStage::TESSELLATION:
				return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case EPipelineStage::COMPUTE:
				return VK_SHADER_STAGE_COMPUTE_BIT;
			case EPipelineStage::GEOMETRY:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
			default :
				break;
		};
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	struct PipelineStage
	{
		EPipelineStage type;
		std::string shaderPath;
	};

	/**
	 */
	class GenericProperty
	{
	private:
		void* m_DataPtr = nullptr;
		uint32_t m_Size = 0;
	public:
		void* GetData( void )
		{
			return m_DataPtr;
		}
	};

	template<typename T>
	class PropertyWrapper : public GenericProperty
	{
	private:
		T m_Data;
	public:
		PropertyWrapper( void )
		{
			m_DataPtr = &m_Data;
		}

		void Set(T value)
		{
			m_Data = value;
		}
	};
	/**
	 */

	struct PipelineProperty
	{
		std::function<GenericProperty*(std::string)> funct;
		void* data;
		uint32_t size;
	};

	extern std::unordered_map<std::string, PipelineProperty> Properties;

	class PipelineConf
	{
	private:
		std::vector<uint32_t> m_UniqueSets;
		std::vector<PipelineStage> m_Stages;
	protected:
		std::unordered_map<std::string, PipelineProperty>	m_Properties;
        EPipelineType m_PipelineType;
	public:
		using PropertiesIterator = std::unordered_map<std::string, PipelineProperty>::iterator;

		PipelineConf( EPipelineType type ) : m_PipelineType(type)
		{

		}

		EPipelineType GetType( void )
		{
			return m_PipelineType;
		}

		template<typename T>
		void SetProperty(std::string name, T value)
		{
			if (m_Properties.find(name) == m_Properties.end())
				return ; // TODO: Not FOUND
			
			memcpy(m_Properties[name].data, &value, m_Properties[name].size);
		}

		template<>
		void SetProperty(std::string name, std::string value)
		{
			if (m_Properties.find(name) == m_Properties.end())
				return ; // TODO: Not FOUND
			
			memcpy(m_Properties[name].data, (m_Properties[name].funct(value)->GetData()), m_Properties[name].size);
		}

		void AddUniqueSet(uint32_t set);
		void AddStageInfo( PipelineStage stage );
		bool IsUniqueSet( uint32_t set );

		const std::vector<PipelineStage> GetStages( void ) const
		{
			return m_Stages;
		}
	};
} // namespace Soon
