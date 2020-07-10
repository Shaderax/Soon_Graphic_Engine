#pragma once

#include <stdint.h>

#include "vulkan/vulkan.h"
#include "Utilities/Error.hpp"

#include <cstring>

#include "RendererRessource.hpp"

namespace Soon
{
	class GraphicsRenderer;

	enum class EnumTextureFormat : uint8_t
	{
		UNDEFINED = 0,
		G = 1,
		GA = 2,
		RGB = 3,
		RGBA = 4
	};

	class TextureFormat
	{
	public:
		EnumTextureFormat mFormat;
		TextureFormat(EnumTextureFormat format);
		TextureFormat(void);


		uint32_t GetSize( void );

		bool operator==(TextureFormat& rhs);
		bool operator!=(TextureFormat& rhs);
	};

	/**
	 * FILTER MODE
	 */
	enum class EnumFilterMode : uint8_t
	{
		NEAREST = 0,
		LINEAR = 1
	};

	/**
	 * TextureType
	 */
	enum class EnumTextureType : uint8_t
	{
		TEXTURE_2D = 1,
		TEXTURE_CUBE = 6
	};

	/**
	 * TEXTURE
	 */
	class Texture : RendererRessource
	{
	private:
		uint8_t* m_Data;
		EnumFilterMode	m_Filter;
		uint8_t		m_MipMapLevel;
		uint8_t		m_AnisoLevel;
		TextureFormat	m_Format;
		EnumTextureType m_Type;

	public:
		uint32_t mWidth;
		uint32_t mHeight;

		Texture( uint32_t width, uint32_t height, EnumTextureFormat format, EnumTextureType type );
		Texture( void );
		~Texture( void );

		void SetPixel( uint32_t x, uint32_t y);

		void SetData(void* data, uint32_t width, uint32_t height, TextureFormat format, EnumTextureType type = EnumTextureType::TEXTURE_2D);

		uint8_t* GetData( void ) const;

		void Allocate( void );
		void Free( void );
		EnumFilterMode GetFilterMode( void );
		uint8_t GetAnisotropLevel( void );
		TextureFormat GetFormat( void );
		uint8_t GetMipMapLevel( void );
		uint8_t GetArrayLayer( void );
		EnumTextureType GetType( void );
		uint32_t GetId( void );

		friend GraphicsRenderer;
	};
} // namespace Soon