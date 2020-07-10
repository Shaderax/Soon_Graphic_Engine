#include <stdint.h>

#include "Texture.hpp"

#include "vulkan/vulkan.h"
#include "Utilities/Error.hpp"

#include <cstring>

namespace Soon
{
	/**
	 * TEXTURE FORMAT
	 */
	TextureFormat::TextureFormat( EnumTextureFormat format )
	{
		mFormat = format;
	}
	
	TextureFormat::TextureFormat( void )
	{
		mFormat = EnumTextureFormat::UNDEFINED;
	}

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

	/**
	 * TEXTURE
	 */
	Texture::Texture( void ) : m_Data(nullptr), m_AnisoLevel(1), m_Type(EnumTextureType::TEXTURE_2D), RendererRessource(ERendererRessource::TEXTURE, Soon::IdError)
	{
	}

	Texture::Texture( uint32_t width, uint32_t height, EnumTextureFormat format, EnumTextureType type ) : mWidth(width),
																					mHeight(height),
																					m_Format(format),
																					m_Type(type),
																					m_AnisoLevel(1),
																					RendererRessource(ERendererRessource::TEXTURE, Soon::IdError)
	{

		m_Data = new uint8_t[width * height * m_Format.GetSize()]();
	}

	Texture::~Texture( void )
	{
		//TODO: SHARED
		if (m_Data != nullptr)
			delete[] m_Data;
	}

	void Texture::SetPixel( uint32_t x, uint32_t y)
	{
		((uint32_t*)m_Data)[x + (mWidth * y)] = 1;
	}

	void Texture::SetData(void* data, uint32_t width, uint32_t height, TextureFormat format, EnumTextureType type)
	{
		mWidth = width;
		mHeight = height;
		m_Format = format;
		m_Type = type;

		if (m_Data != nullptr)
			delete[] m_Data;
		
		m_Data = new uint8_t[mWidth * mHeight * m_Format.GetSize() * (uint8_t)type]();

		memcpy(m_Data, data, mWidth * mHeight * m_Format.GetSize() * (uint8_t)type);
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