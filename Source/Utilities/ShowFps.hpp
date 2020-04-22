#include <GLFW/glfw3.h>

double ShowFPS( double lastTime )
{
	static int nbFrames = 0;
	double currentTime = glfwGetTime();

	if ( currentTime - lastTime >= 1.0 )
	{
		std::cout << nbFrames << " Fps" << std::endl;
		nbFrames = 0;
		lastTime += 1.0;
	}
	nbFrames++;

     return lastTime;
}
