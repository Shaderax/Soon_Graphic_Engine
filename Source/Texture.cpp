#include <stdint.h>

#include "Texture.hpp"

#include "vulkan/vulkan.h"
#include "Utilities/Error.hpp"

#include <cstring>

namespace Soon
{
	uint32_t TextureFormat::GetSize( void )
	{
		return ((uint8_t)mFormat);
	}
	
	bool TextureFormat::operator==(TextureFormat& rhs)
	{
		return (mFormat == rhs.mFormat);
	}

	bool TextureFormat::operator!=(TextureFormat& rhs)
	{
		return !(*this == rhs);
	}

	Texture::Texture( void ) : m_Data(nullptr), m_UniqueId(Soon::IdError)
	{
	}

	void Texture::SetData(void* data, uint32_t width, uint32_t height, TextureFormat format)
	{
		mWidth = width;
		mHeight = height;
		m_Format = format;

		if (m_Data != nullptr)
			delete m_Data;
			
		memcpy(m_Data, data, mWidth * mHeight * m_Format.GetSize());
	}

	uint8_t* Texture::GetData( void ) const
	{
		return m_Data;
	}

	void Texture::Allocate( void )
	{
	}

	void Texture::Free( void )
	{
	}

	EnumFilterMode Texture::GetFilterMode( void )
	{
		return m_Filter;
	}

	uint8_t Texture::GetAnisotropLevel( void )
	{
		return m_AnisoLevel;
	}

	TextureFormat Texture::GetFormat( void )
	{
		return m_Format;
	}

	uint8_t Texture::GetMipMapLevel( void )
	{
		return m_MipMapLevel;
	}

	uint8_t Texture::GetArrayLayer( void )
	{
		return (uint8_t)m_Type;
	}

	EnumTextureType Texture::GetType( void )
	{
		return m_Type;
	}

	uint32_t Texture::GetId( void )
	{
		return (m_UniqueId);
	}

} // namespace Soon