#include <stdint.h>

#include "Texture.hpp"

#include "vulkan/vulkan.h"
#include "Utilities/Error.hpp"

#include <cstring>

#include "GraphicsRenderer.hpp"

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
	Texture::Texture( void ) : m_AnisoLevel(1), m_Type(EnumTextureType::TEXTURE_2D), RendererRessource(ERendererRessource::TEXTURE, Soon::IdError)
	{
	}

	Texture::Texture( uint32_t width, uint32_t height, EnumTextureFormat format, EnumTextureType type ) : mWidth(width),
																					mHeight(height),
																					m_Format(format),
																					m_Type(type),
																					m_AnisoLevel(1),
																					RendererRessource(ERendererRessource::TEXTURE, Soon::IdError)
	{
		m_Data = std::shared_ptr<uint8_t>(new uint8_t[width * height * m_Format.GetSize() * (int)type](), std::default_delete<uint8_t[]>());
	}

	Texture::~Texture( void )
	{
	}

	Texture::Texture(const Texture& other) : RendererRessource(ERendererRessource::TEXTURE, other.GetId())
	{
		std::cout << "Copy Constructor" << std::endl;
		m_Data = other.m_Data;
		m_Filter = other.m_Filter;
		m_MipMapLevel = other.m_MipMapLevel;
		m_AnisoLevel = other.m_AnisoLevel;
		m_Format = other.m_Format;
		m_Type = other.m_Type;
		mWidth = other.mWidth;
		mHeight = other.mHeight;
	}

	Texture::Texture(Texture&& other) : RendererRessource(ERendererRessource::TEXTURE, Soon::IdError)
	{
		std::cout << "Move Constructor" << std::endl;
		m_Data = std::move(other.m_Data);
		m_Filter = other.m_Filter;
		m_MipMapLevel = std::exchange(other.m_MipMapLevel, 0);
		m_AnisoLevel = std::exchange(other.m_AnisoLevel, 1);
		m_Format = other.m_Format;
		m_Type = other.m_Type;
		mWidth = std::exchange(other.mWidth, 0);
		mHeight = std::exchange(other.mHeight, 0);

		m_UniqueId = std::exchange(other.m_UniqueId, Soon::IdError);
	}

	Texture& Texture::operator=(const Texture& other)
	{
		std::cout << "= operator" << std::endl;

		if (this == &other)
			return *this;
		
		m_Data = other.m_Data;
		m_Filter = other.m_Filter;
		m_MipMapLevel = other.m_MipMapLevel;
		m_AnisoLevel = other.m_AnisoLevel;
		m_Format = other.m_Format;
		m_Type = other.m_Type;
		mWidth = other.mWidth;
		mHeight = other.mHeight;

		if (m_UniqueId != Soon::IdError)
			GraphicsRenderer::GetInstance()->RemoveTexture(m_UniqueId);
		m_UniqueId = other.m_UniqueId;
		GraphicsRenderer::GetInstance()->AddTexture(m_UniqueId);

		return *this;
	}
	Texture& Texture::operator=(Texture&& other)
	{
		std::cout << "Move = operator" << std::endl;
		if (this == &other)
			return *this;
		

		m_Data = std::move(other.m_Data);
		m_Filter = other.m_Filter;
		m_MipMapLevel = std::exchange(other.m_MipMapLevel, 0);
		m_AnisoLevel = std::exchange(other.m_AnisoLevel, 1);
		m_Format = other.m_Format;
		m_Type = other.m_Type;
		mWidth = std::exchange(other.mWidth, 0);
		mHeight = std::exchange(other.mHeight, 0);

		m_UniqueId = std::exchange(other.m_UniqueId, Soon::IdError);

		return *this;
	}

	void Texture::SetPixel( uint32_t x, uint32_t y)
	{
		((uint32_t*)m_Data.get())[x + (mWidth * y)] = 1;
	}

	void Texture::SetData(void* data, uint32_t width, uint32_t height, TextureFormat format, EnumTextureType type)
	{
		mWidth = width;
		mHeight = height;
		m_Format = format;
		m_Type = type;

		m_Data = std::shared_ptr<uint8_t>(new uint8_t[width * height * m_Format.GetSize() * (uint8_t)type](), std::default_delete<uint8_t[]>());

		memcpy(m_Data.get(), data, mWidth * mHeight * m_Format.GetSize() * (uint8_t)type);
	}

	uint8_t* Texture::GetData( void ) const
	{
		return m_Data.get();
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

	void Texture::SetFilterMode(EnumFilterMode filter)
	{
		m_Filter = filter;
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
} // namespace Soon
