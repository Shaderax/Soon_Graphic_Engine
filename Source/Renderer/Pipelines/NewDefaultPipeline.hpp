#pragma once

#include "Renderer/Pipelines/ShaderPipeline.hpp"

namespace Soon
{
class NewDefaultPipeline : public ShaderPipeline
{
    public:
        NewDefaultPipeline( void )
        {
            _pathVert = "Ressources/Shaders/TestShaders/TestShader.vert.spv";
            _pathFrag = "Ressources/Shaders/TestShaders/TestShader.frag.spv";

            //SetDefaultUniform("unitruc");
        }

        ~NewDefaultPipeline( void )
        {

        }
};
}