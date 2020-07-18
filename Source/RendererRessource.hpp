#pragma once

#include <cstdint>

#include "Utilities/Error.hpp"

namespace Soon
{
	enum class ERendererRessource : uint8_t
	{
		MESH = 0,
		TEXTURE = 1,
		BUFFER = 2
	};

	class RendererRessource
	{
	private:
		ERendererRessource m_Type;
	protected:
		uint32_t m_UniqueId = Soon::IdError;
		RendererRessource( ERendererRessource type, uint32_t id ); //TODO: ID 
	public:
		~RendererRessource( void );

		uint32_t GetId( void ) const;
	};
}