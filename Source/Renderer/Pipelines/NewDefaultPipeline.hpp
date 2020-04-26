#pragma once

#include "Renderer/Pipelines/ShaderPipeline.hpp"

namespace Soon
{
class NewDefaultPipeline : public ShaderPipeline
{
    public:
        NewDefaultPipeline( void )
        {
            _pathVert = "Ressources/Shaders/DefaultShader.vert.spv";
            _pathFrag = "Ressources/Shaders/DefaultShader.frag.spv";

            //SetDefaultUniform("unitruc");
        }

        ~NewDefaultPipeline( void )
        {

        }
};
}