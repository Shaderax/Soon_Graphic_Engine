#include "Renderer/Pipelines/ShaderPipeline.hpp"

class NewDefaultPipeline : ShaderPipeline
{
    public:
        NewDefaultPipeline( void )
        {
            _pathVert = "../Ressources/Shaders/DefaultShader.vert.spv";
            _pathFrag = "../Ressources/Shaders/DefaultShader.frag.spv";

            SetDefaultUniform("unitruc");
        }

        ~NewDefaultPipeline( void )
        {

        }
};