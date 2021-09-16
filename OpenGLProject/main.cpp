#define STB_IMAGE_IMPLEMENTATION

#include<iostream>

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "UniformBuffer.h"
#include "Texture.h"
#include "MeshReader.h"
#include "Camera.h"

float g_deltaTime = 0.0f;
float g_previousFrameTime = 0.0f;

float g_previousCursorX = 0.0f;
float g_previousCursorY = 0.0f;

void framebufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	__debugbreak();
}

void processCameraInput(Camera& camera, GLFWwindow* window)
{
	float currentFrameTime = glfwGetTime();
	g_deltaTime = currentFrameTime - g_previousFrameTime;
	g_previousFrameTime = currentFrameTime;

	const float unitsPerFrame = 500.0f * g_deltaTime;

	const glm::vec3 forward(0.0f, 0.0f, -1.0f);
	const glm::vec3 up(0.0f, 1.0f, 0.0f);
	const glm::vec3 right(1.0f, 0.0f, 0.0f);
	
	//Find the screen coordinates of the cursor
	double xPos;
	double yPos;

	glfwGetCursorPos(window, &xPos, &yPos);

	float xCursorPos = (float)xPos;
	float yCursorPos = (float)yPos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		// Yaw
		const glm::vec3 worldSpaceUp = camera.getWorldOrientation() * up;

		const float deltaX = g_previousCursorX - xCursorPos;
		const glm::mat3 yawRotation = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(deltaX), worldSpaceUp)); // How do you not have a rotate for mat3??
		camera.applyWorldSpaceRotation(yawRotation);

		//Pitch
		const glm::vec3 worldSpaceRight = camera.getWorldOrientation() * right;

		const float deltaY = g_previousCursorY - yCursorPos;
		const glm::mat3 pitchRotation = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(deltaY), worldSpaceRight));
		camera.applyWorldSpaceRotation(pitchRotation);
	}

	g_previousCursorX = xCursorPos;
	g_previousCursorY = yCursorPos;

	//Camera Translation
	glm::vec3 viewSpaceTranslation(0.0f, 0.0f, 0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation + (unitsPerFrame * forward);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation - (unitsPerFrame * forward);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation - (unitsPerFrame * right);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation + (unitsPerFrame * right);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation + (unitsPerFrame * up);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		viewSpaceTranslation = viewSpaceTranslation - (unitsPerFrame * up);
	}

	glm::vec3 worldSpaceTranslation = camera.getWorldOrientation() * viewSpaceTranslation;
	glm::vec3 newWorldSpacePosition = camera.getWorldPosition() + worldSpaceTranslation;
	camera.setCameraWorldPosition(newWorldSpacePosition);
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "My Window", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	Shader basicShader("Shaders\\basicShader.vert", "Shaders\\basicShader.frag");

	float vertices[] = {
		// positions         // texture coordinate
		 -0.5f, -0.5f,  0.5f,	0.0f, 0.0f,   // Front
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f,  
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,   

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 
		0.5f, 0.5f,  0.5f,		1.0f, 1.0f, 
		-0.5f, 0.5f,  0.5f,		0.0f, 1.0f,

		-0.5f, -0.5f,  -0.5f,	0.0f, 0.0f,   // Bottom
		0.5f, -0.5f,  -0.5f,	1.0f, 0.0f,
		 0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,

		-0.5f, -0.5f,  -0.5f,	0.0f, 0.0f,
		0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

		0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   // Back
		-0.5f, -0.5f,  -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f,  -0.5f,  1.0f, 0.0f,
		-0.5f, 0.5f,  -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  -0.5f,  0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f,  0.0f, 0.0f,   // Top
		0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, 0.5f,  -0.5f,  1.0f, 1.0f,

		-0.5f, 0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, 0.5f,  -0.5f,  1.0f, 1.0f,
		-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f,

		0.5f, -0.5f, 0.5f,  0.0f, 0.0f,   // Right
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f, 0.5f,  -0.5f,  1.0f, 1.0f,

		0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
		0.5f, 0.5f,  -0.5f,  1.0f, 1.0f,
		0.5f, 0.5f,  0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   // Left
		-0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
		 -0.5f, 0.5f, 0.5f,  1.0f, 1.0f,

		-0.5f, -0.5f,  -0.5f,  0.0f, 0.0f,
		-0.5f, 0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f
	};


	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//UBO
	UniformBuffer uniformBuffer;

	GLuint uboMatricesID;

	unsigned int uniformMatrixBlockIndex;
	uniformMatrixBlockIndex = glGetUniformBlockIndex(basicShader.getProgramID(), "sceneMatrices");
	assert(uniformMatrixBlockIndex != GL_INVALID_INDEX);


	/*
	Returns a list of specified buffer object names.
	These "names" are integers. They are not guarranteed
	to be a set of continuous integers.
	*/
	glGenBuffers(1, &uboMatricesID);

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, uniformMatrixBlockIndex, uboMatricesID, 0, sizeof(UniformBuffer)); // Understand what this does, since something seems fishy


	//USE glBufferSubData() to update the values of the UBO members later on whent the values change due to the addition of a camera.

	// Jpgs 
	Texture greenMarble("Textures//greenMarble.jpg");
	Texture minion("Textures//minion.jpg");
	Texture sandTexture("Textures//sandTexture.jpg");
	Texture sunflower("Textures//Sunflower.jpg");
	Texture woodenCrate("Textures//woodenCrate.jpg");

	////Pngs
	Texture arcticFox("Textures//arcticFox.png");
	Texture grayGradient("Textures//grayGradient.png");
	Texture openglLogoTransparent("Textures//openglLogoTransparent.png");
	Texture wood("Textures//wood.png");

	//ppms
	Texture sample640x426(R"(Textures\sample_640x426.ppm)");
	Texture sample1280x853(R"(Textures\sample_1280x853.ppm)");
	Texture sample1920x1280(R"(Textures\sample_1920x1280.ppm)");
	Texture sample5184x3456(R"(Textures\sample_5184x3456.ppm)");

	//---------------------------------------------------------------------------------------------------------------------------------------
	// Testing to see if the MeshReader Data can be rendered

	//MeshReader stanfordBunny(R"(Meshes\cubePositive.obj)");
	//MeshReader stanfordBunny(R"(Meshes\bunny\bunny.obj)");
	//MeshReader stanfordBunny(R"(Meshes\CornellBox\CornellBox-Original.obj)");
	//MeshReader stanfordBunny(R"(Meshes\CornellBox\CornellBox-Empty-CO.obj)");
	MeshReader stanfordBunny(R"(Meshes\sponza\sponza.obj)");

	unsigned int positiveCubeVBO;
	unsigned int positiveCubeVAO;

	glGenVertexArrays(1, &positiveCubeVAO);
	glGenBuffers(1, &positiveCubeVBO);

	glBindVertexArray(positiveCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, positiveCubeVBO);

	glBufferData(GL_ARRAY_BUFFER, stanfordBunny.getSizeOfFaceArray(), stanfordBunny.getFaces().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(1);

	Shader meshTestShader(R"(Shaders\meshTestShader.vert)", R"(Shaders\meshTestShader.frag)");
	//-----------------------------------------------------------------------------------------------------------------------------------------

	Camera camera;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		processCameraInput(camera, window);

		glEnable(GL_DEPTH_TEST);

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float delta = 80.0f * (float)glfwGetTime();
		glm::mat4 model = glm::mat4(1.0f);
		copyMat4ToFloatArray(model, uniformBuffer.model);

		glm::mat4 view = camera.createViewMatrix();
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)800 / (float)600, 0.1f, 1000.0f);

		glm::mat4 viewProjection = projection * view;
		copyMat4ToFloatArray(viewProjection, uniformBuffer.viewProjection);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//jpg
		//greenMarble.useTexture();
		//minion.useTexture();
		//sandTexture.useTexture();
		//sunflower.useTexture();
		//woodenCrate.useTexture();

		////Pngs
		//arcticFox.useTexture();
		//grayGradient.useTexture();
		//openglLogoTransparent.useTexture();
		//wood.useTexture();

		////ppms
		//sample640x426.useTexture();
		//sample1280x853.useTexture();
		//sample1920x1280.useTexture();
		//sample5184x3456.useTexture();

		//basicShader.useProgram();
		meshTestShader.useProgram();
		glBindVertexArray(positiveCubeVAO);
		
		glDrawArrays(GL_TRIANGLES, 0, stanfordBunny.getFaces().size() * 3);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}