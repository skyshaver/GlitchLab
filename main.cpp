#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <tuple>

#include "Shader.h"
#include "VideoReader.h"

const int WWIDTH = 800;
const int WHEIGHT = 600;

// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// variables to hide mouse when program is first launched
double prevXpos = 0.0;
double prevYpos = 0.0;
bool firstMouseMove = true;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove)
	{
		prevXpos = xpos;
		prevYpos = ypos;
		firstMouseMove = false;
	}

	if (prevXpos != xpos || prevYpos != ypos)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

// store previous window size
int wXPos, wYPos, wW, wH;
static bool fullscreen = false;
// temp setup glfw keycallback to use esc for exit
void processInput(GLFWwindow* window)
{
	// esc to exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }

	// toggle full screen
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fullscreen)
	{
		// capture state of window
		glfwGetWindowPos(window, &wXPos, &wYPos);
		glfwGetWindowSize(window, &wW, &wH);
		// switch to exclusive fullscreen
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		fullscreen = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		// switch back to previous window state
		glfwSetWindowMonitor(window, nullptr, wXPos, wYPos, wW, wH, 0);
		fullscreen = false;
	}

}

std::tuple<bool, double> parseCommandLineArgs(int argc, const char** argv);

int main(int argc, const char** argv)
{
	if (!glfwInit())
	{
		std::cout << "Couldn't init GLFW!\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// don't show window till after initial render
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WWIDTH, WHEIGHT, "Glitch Lab", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// hide mouse pointer
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	// just the four corners
	//float vertices[] = {
	//	// positions          // colors           // texture coords
	//	 1.0f,  1.f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // top right
	//	 1.0f, -1.f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // bottom right
	//	-1.0f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,    // top left 
	//	-1.f,  -1.f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // bottom left
	//};

	// 2D rejig fill screen
	float vertices[] = {
		// pos      // tex
		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f
	};

	//// 2D rejig, top right corner
	//float vertices[] = {
	//	// pos      // tex
	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, 0.0f,

	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 1.0f, 1.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f, 0.0f
	//};


	// texture buffer setup
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex and texture coords in one array
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/
	// texture coord attribute
	/*glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);*/

	// texture setup for video play back
	GLuint tex_handle;
	glGenTextures(1, &tex_handle);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	
	// init videoreader and load video
	VideoReader videoReader("videos/butterfly.mp4");

	// compile texture shaders
	//Shader textureShader_01("shaders/texture_shader2D.vert", "shaders/texture_shader.frag");
	Shader textureShader_01("shaders/chaos_shader.vert", "shaders/chaos_shader.frag");

	// shader uniform setup
	int currentWw, currentWh;
	double mouseXpos, mouseYpos;

	// get clargs
	double quitTimer = 60.0;
	bool isFullScreen = false;
	std::tie(isFullScreen, quitTimer) = parseCommandLineArgs(argc, argv);

	if (isFullScreen)
	{
		// switch to exclusive fullscreen, the actual display of fullscreen/ any screen is put off till the first render is done
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, tex_handle);

		/*if (int(glfwGetTime()) % 2 == 0)
		{
			textureShader_01.use();
		}
		else
		{
			textureShader.use();
		}*/
		textureShader_01.use();

		// pass elapsed time uniform
		static double startTime = glfwGetTime();
		float u_time = static_cast<float>(glfwGetTime() - startTime);
		textureShader_01.setFloat("u_time", u_time);

		// use quit timer returned from clargs to close window
		if(u_time > quitTimer){ glfwSetWindowShouldClose(window, true); }

		// screen position uniform
		glfwGetWindowSize(window, &currentWw, &currentWh);
		glm::vec2 u_resolution = { static_cast<float>(currentWw), static_cast<float>(currentWh) };
		textureShader_01.setVec2("u_resolution", u_resolution);

		// pass mouse position uniform
		glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
		glm::vec2 u_mousePos = { mouseXpos, mouseYpos };
		textureShader_01.setVec2("u_mousePos", u_mousePos);

		// apply the texture from our video reader frame data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoReader.getWidth(), videoReader.getHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, videoReader.readFrame());
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// crude video sync for playback // TODO improve this sync function
		static bool firstFrame = true;
		if (firstFrame)
		{
			glfwSetTime(0.0);
			firstFrame = false;
		}
		while (videoReader.returnPtsInSecs() > glfwGetTime())
		{
			glfwWaitEventsTimeout(videoReader.returnPtsInSecs() - glfwGetTime());
		}
		// end sync

		// show window after first render to help avoid glitching
		glfwShowWindow(window);
		
		//---------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}