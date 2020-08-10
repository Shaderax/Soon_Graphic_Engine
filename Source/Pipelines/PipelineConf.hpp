#pragma once

#include <unordered_map>
#include <functional>

#include "vulkan/vulkan.h"

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

	VkShaderStageFlagBits PipelineStageToVk( EPipelineStage stage );

	struct PipelineStage
	{
		EPipelineStage type;
		std::string shaderPath;
	};

	/**
	 */
	class GenericProperty
	{
	protected:
		void* m_DataPtr = nullptr;
	public:
		void* GetData( void )
		{
			return m_DataPtr;
		}
		virtual ~GenericProperty() { }
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
		std::string mJsonPath;
	protected:
		std::unordered_map<std::string, PipelineProperty>	m_Properties;
        EPipelineType m_PipelineType;
	public:
		using PropertiesIterator = std::unordered_map<std::string, PipelineProperty>::iterator;

		PipelineConf( EPipelineType type );


		EPipelineType GetType( void )
		{
			return m_PipelineType;
		}

		void SetJsonPath( std::string name );
		std::string GetJsonPath( void ) const;

		virtual ~PipelineConf( void ) {}

		void SetProperty(std::string name, std::string value);

		void AddUniqueSet(uint32_t set);
		void AddStageInfo( PipelineStage stage );
		bool IsUniqueSet( uint32_t set );

		const std::vector<PipelineStage> GetStages( void ) const
		{
			return m_Stages;
		}
	};
} // namespace Soon
