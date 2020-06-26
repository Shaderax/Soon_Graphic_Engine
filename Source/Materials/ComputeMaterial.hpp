#pragma once

#include "Pipelines/ComputePipeline.hpp"
#include "Materials/Material.hpp"

#include "GraphicsRenderer.hpp"

#define COMPUTEPIPELINE reinterpret_cast<ComputePipeline*>(m_Pipeline)

namespace Soon
{
	class ComputeMaterial : public Material
	{
	private:
	public:

		ComputeMaterial(void);
		~ComputeMaterial(void);

		void Process( );
		void UnProcess( void );
		void SetWorkGroup(uint32_t x, uint32_t y, uint32_t z);

		void RemoveFromPipeline(std::uint32_t meshId);
	};
} // namespace Soon
