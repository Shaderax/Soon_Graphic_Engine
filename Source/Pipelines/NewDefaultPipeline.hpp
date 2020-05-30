#pragma once

#include "Pipelines/ShaderPipeline.hpp"

namespace Soon
{
class NewDefaultPipeline : public ShaderPipeline
{
    public:
        NewDefaultPipeline( void )
        {
            _pathVert = "Ressources/Shaders/TestShaders/TestShader.vert.spv";
            _pathFrag = "Ressources/Shaders/TestShaders/TestShader.frag.spv";

			DefaultUniformStruct defaultStruct;
			defaultStruct.isUnique = true;
			defaultStruct.name = "UniformModel";
            SetDefaultUniform(defaultStruct);
        }

        ~NewDefaultPipeline( void )
        {

        }
};
}
