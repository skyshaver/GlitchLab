#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VideoReader.h"

const int WWIDTH = 1920;
const int WHEIGHT = 1080;

// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// temp setup glfw keycallback to use esc for exit
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Couldn't init GLFW!\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WWIDTH, WHEIGHT, "hello", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	// vertices for rectangle, texture mapping inverted but image is mirrored?
	float vertices[] = {
		// positions          // colors           // texture coords
		 1.f,  1.f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // top right
		 1.f, -1.f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // bottom right
		//-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,    // top left 

		 1.f, -1.f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // bottom right
		-1.f, -1.f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // bottom left
		-1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,    // top left 
		 //0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	};

	// texture buffer setup
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
	Shader textureShader("shaders/texture_shader.vert", "shaders/texture_shader.frag");
	Shader textureShader_01("shaders/texture_shader.vert", "shaders/texture_shader_01.frag");

	glm::vec2 u_resolution = { float(videoReader.getWidth()), float(videoReader.getHeight()) };

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
		// send time uniform
		static float startTime = glfwGetTime();
		float u_time = glfwGetTime() - startTime;
		textureShader_01.setFloat("u_time", u_time);

		//glm::vec2 u_resolution = { 1.f, 0.f };
		textureShader_01.setVec2("u_resolution", u_resolution);
		

		// apply the texture from our video reader frame data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoReader.getWidth(), videoReader.getHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, videoReader.readFrame());
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		

		// crude sync
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


		//---------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}