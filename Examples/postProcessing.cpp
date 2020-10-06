#include "Math/vec3.hpp"
#include "Math/vec2.hpp"

#include "GraphicsInstance.hpp"
#include "GraphicsRenderer.hpp"

#include "Mesh.hpp"
#include "Materials/GraphicMaterial.hpp"

#include "Utilities/ShowFps.hpp"

#include <chrono>
#include <ctime>

#include "ObjLoader.hpp"

#include "MeshVertex.hpp"
#include "Camera.hpp"

#include <thread>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include "Configure.h"

using namespace Soon;

int main()
{
	// Init
	GraphicsInstance::GetInstance()->Initialize();
	GraphicsRenderer::GetInstance()->Initialize();
	BasePipeline* damn = GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/InputAttch.json");
	exit(-1);

	std::vector<Mesh>* meshArray = ObjLoader(GRAPHIC_SOURCE_DIR"/Ressources/3DModel/Cobra/Shelby.obj", GRAPHIC_SOURCE_DIR"/Ressources/3DModel/Cobra/", GRAPHIC_SOURCE_DIR"/Examples/postProcessing.json");
	std::vector<Mesh>* cube = ObjLoader(GRAPHIC_SOURCE_DIR"/Examples/Cube.obj");
	Mesh* mesh = &meshArray->back();

	glm::mat4 model;
	model = glm::scale(glm::mat4(1.0f), glm::vec3(10.2f));
	model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	BasePipeline* defaultPipeline = GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/NewDefaultPipeline.json");
	BasePipeline* postProcessPipeline = GraphicsRenderer::GetInstance()->AddPipeline(GRAPHIC_SOURCE_DIR"/Examples/postProcessing.json");
	glm::vec3 lightPos(10.0f, 0.0f, 0.0f);
	postProcessPipeline->SetUnique("l.lightPos", &lightPos);

	(*cube)[0].Render();
	(*cube)[0].GetMaterial().Set("um.model", &model);

	float shini = 32;
	for (auto& m : *meshArray)
	{
		m.Render();
		m.GetMaterial().Set("um.model", &model);
		m.GetMaterial().Set("cr.shininess", &shini);
	}

	//meshArray->back().GetMaterial().SetTexture("latexture", texture);

	double lastTime = 0;
	Camera3D camera;
	glm::mat4 tmpMat;

	model = glm::mat4(1.0f);

	// Loop
	std::cout << "Begin Loop" << std::endl;
	float looper = 0.0f;
	while (!GraphicsInstance::GetInstance()->ShouldClose(GraphicsInstance::GetInstance()->GetWindow()))
	{
		looper += 0.00001f;
		lastTime = ShowFPS(lastTime);

		lightPos.z += looper;
		model = glm::mat4(
    		1.0f, 0.0f, 0.0f, 0.0f,
    		0.0f, 1.0f, 0.0f, 0.0f,
    		0.0f, 0.0f, 1.0f, 0.0f,
    		lightPos.x, lightPos.y, lightPos.z, 1.0 );
		(*cube)[0].GetMaterial().Set("um.model", &model);

		tmpMat = camera.GetViewMatrix();
		postProcessPipeline->SetUnique("uc.view", &(tmpMat));
		defaultPipeline->SetUnique("uc.view", &(tmpMat));

		glm::vec3 viewPos(camera.m_Pos.x, camera.m_Pos.y, camera.m_Pos.z);
		postProcessPipeline->SetUnique("l.viewPos", &viewPos);
		postProcessPipeline->SetUnique("l.lightPos", &lightPos);

		tmpMat = camera.GetProjectionMatrix();
		postProcessPipeline->SetUnique("uc.proj", &(tmpMat));
		defaultPipeline->SetUnique("uc.proj", &(tmpMat));

		/**/
			camera.Move();
		/**/

		GraphicsRenderer::GetInstance()->Update();
		GraphicsInstance::GetInstance()->PollEvent();
		GraphicsInstance::GetInstance()->DrawFrame();
	}

	//delete mesh;

	// Destroy
	GraphicsRenderer::ReleaseInstance();
	GraphicsInstance::ReleaseInstance();

	return 0;
}
