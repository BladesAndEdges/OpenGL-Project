#define STB_IMAGE_IMPLEMENTATION

#define ArraySize(x) sizeof(x)/sizeof(x[0]);

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
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		__debugbreak();
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		__debugbreak();
		break;
	case GL_DEBUG_SEVERITY_LOW:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		__debugbreak();
		break;
	default:
		break;
	}

}

void processCameraInput(Camera& camera, GLFWwindow* window)
{
	float currentFrameTime = glfwGetTime();
	g_deltaTime = currentFrameTime - g_previousFrameTime;
	g_previousFrameTime = currentFrameTime;

	std::cout << g_deltaTime << std::endl;

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
		//Probably not fixed, but work in progress
		const glm::vec3 wRight = camera.getWorldOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
		const glm::vec3 wUp = camera.getWorldOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);

		const float deltaY = g_previousCursorY - yCursorPos;
		const glm::mat3 pitchRotation = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(deltaY), right));

		const float deltaX = g_previousCursorX - xCursorPos;
		const glm::mat3 yawRotation = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(deltaX), up)); 

		const glm::mat3 finalRotation =   yawRotation * camera.getWorldOrientation() * pitchRotation;

		camera.setCameraWorldOrientation(finalRotation);
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

float measureAverageFrameTime(const float frameTimeInMilisecods, unsigned int frameNumber, float frameTimeArray[128])
{
	const unsigned int arraySize = ArraySize(frameTimeArray);
	const int indexInArray = frameNumber % arraySize;
	frameTimeArray[indexInArray] = frameTimeInMilisecods;

	float sum = 0.0f;
	if (frameNumber >= 128)
	{
		for (unsigned int frame = 0; frame < arraySize; frame++)
		{
			sum += frameTimeArray[frame];
		}
	}

	const float averageTime = sum / arraySize;
	return averageTime;
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Crytek Sponza Project", nullptr, nullptr);

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

	//---------------------------------------------------------------------------------------------------------------------------------------
	// Testing to see if the MeshReader Data can be rendered
	MeshReader mainModel(R"(Meshes\sponza\sponza.obj)", R"(Meshes\sponza\sponza.mtl)");


	unsigned int positiveCubeVBO;
	unsigned int positiveCubeVAO;

	glGenVertexArrays(1, &positiveCubeVAO);
	glGenBuffers(1, &positiveCubeVBO);

	glBindVertexArray(positiveCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, positiveCubeVBO);

	glBufferData(GL_ARRAY_BUFFER, mainModel.getSizeOfFaceArray(), mainModel.getFaces().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textureCoordinate));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(2);

	Shader meshTestShader(R"(Shaders\meshTestShader.vert)", R"(Shaders\meshTestShader.frag)");
	//-----------------------------------------------------------------------------------------------------------------------------------------

	Camera camera;

	float frameTimeArray[128];
	unsigned int frameNumber = 0;

	while (!glfwWindowShouldClose(window))
	{
		const float timerStartingPoint = glfwGetTime();

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

		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)800 / (float)600, 0.1f, 10000.0f);

		glm::mat4 viewProjection = projection * view;
		copyMat4ToFloatArray(viewProjection, uniformBuffer.viewProjection);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		meshTestShader.useProgram();
		glBindVertexArray(positiveCubeVAO);

		Texture background(R"(C:\Users\danie\Desktop\test.jpg)");
		
		for (const Mesh& mesh : mainModel.getMeshes())
		{
			if (mesh.material->m_textures.size() != 0)
			{
				mesh.material->m_textures[0].useTexture();
			}
			else
			{
				background.useTexture();
			}

			glDrawArrays(GL_TRIANGLES, mesh.firstIndex, mesh.vertexCount);
		}

		glfwSwapBuffers(window);


		const float timerEndPoint = glfwGetTime();

		const float milisecondsElapsed = (timerEndPoint - timerStartingPoint)  * 1000.0f;

		std::cout << "Frame Timer in miliseconds: " << milisecondsElapsed << std::endl;

		float averageFrameTime = measureAverageFrameTime(milisecondsElapsed, frameNumber, frameTimeArray);

		std::cout << "Average Time for 128 frames is: " << averageFrameTime << std::endl;
		frameNumber++;
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}