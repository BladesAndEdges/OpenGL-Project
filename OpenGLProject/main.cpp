#define STB_IMAGE_IMPLEMENTATION

#define ArraySize(x) sizeof(x)/sizeof(x[0]);

#include<iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#pragma warning(push)
#pragma warning(disable:4201)   
#include<gtc/matrix_transform.hpp>
#pragma warning(pop)

#include "UniformBuffer.h"
#include "Texture.h"
#include "MeshReader.h"
#include "Camera.h"
#include "Framebuffer.h"

#include <cstdint>
#include <string>

float g_deltaTime = 0.0f;
float g_previousFrameTime = 0.0f;

float g_previousCursorX = 0.0f;
float g_previousCursorY = 0.0f;

void framebufferCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
}

void GLAPIENTRY MessageCallback(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
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
	float currentFrameTime = (float)glfwGetTime();
	g_deltaTime = currentFrameTime - g_previousFrameTime;
	g_previousFrameTime = currentFrameTime;

	const float unitsPerFrame = 5.0f * g_deltaTime;

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

void renderScene()
{


}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 450 core";
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	Shader basicShader("Shaders\\basicShader.vert", "Shaders\\basicShader.frag");

	//UBO
	UniformBuffer uniformBuffer;

	unsigned int uniformMatrixBlockIndex;
	uniformMatrixBlockIndex = glGetUniformBlockIndex(basicShader.getProgramID(), "sceneMatrices");
	assert(uniformMatrixBlockIndex != GL_INVALID_INDEX);


	/*
	Returns a list of specified buffer object names.
	These "names" are integers. They are not guarranteed
	to be a set of continuous integers.
	*/

	GLuint sceneUBO;
	glGenBuffers(1, &sceneUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, sceneUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glObjectLabel(GL_BUFFER, sceneUBO, -1, "SceneUniformBuffer");

	glBindBufferRange(GL_UNIFORM_BUFFER, uniformMatrixBlockIndex, sceneUBO, 0, sizeof(UniformBuffer)); // Understand what this does, since something seems fishy

	//USE glBufferSubData() to update the values of the UBO members later on whent the values change due to the addition of a camera.

	//---------------------------------------------------------------------------------------------------------------------------------------
	// Testing to see if the MeshReader Data can be rendered
	MeshReader mainModel(R"(Meshes\sponza\sponza.obj)", R"(Meshes\sponza\sponza.mtl)");

	// Vertex Buffers and VAO
	unsigned int modelVBO;
	unsigned int modelVAO;

	glGenVertexArrays(1, &modelVAO);
	glGenBuffers(1, &modelVBO);

	glBindVertexArray(modelVAO);

	// Index Buffer
	unsigned int modelIBO;
	glGenBuffers(1, &modelIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mainModel.getIndexBuffer().size() * sizeof(unsigned int), mainModel.getIndexBuffer().data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
	glBufferData(GL_ARRAY_BUFFER, mainModel.getIndexedVertexBuffer().size() * sizeof(Vertex), mainModel.getIndexedVertexBuffer().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textureCoordinate));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent));
	glEnableVertexAttribArray(3);


	Shader meshTestShader(R"(Shaders\meshTestShader.vert)", R"(Shaders\meshTestShader.frag)");
	//Upload sampler data
	meshTestShader.useProgram();
	meshTestShader.bindTextureToSampler(0, "ambientTextureSampler");
	meshTestShader.bindTextureToSampler(1, "diffuseTextureSampler");
	meshTestShader.bindTextureToSampler(2, "specularTextureSampler");
	meshTestShader.bindTextureToSampler(3, "normalMapTextureSampler");
	meshTestShader.bindTextureToSampler(4, "maskTextureSampler");
	glUseProgram(0);

	//-----------------------------------------------------------------------------------------------------------------------------------------

	Camera camera(mainModel.getSceneCenter());

	float frameTimeArray[128];
	unsigned int frameNumber = 0;

	Texture dummyNormalMap(R"(Meshes\sponza\textures\dummy_ddn.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat, 
																		TextureFilterMode::Point);
	Texture dummyMask(R"(Meshes\sponza\textures\dummy_mask.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat,TextureFilterMode::Point);

	// ImGui stuff
	bool normalMapBool = true;
	bool ambientBool = true;
	bool diffuseBool = true;
	bool specularBool = true;

	float azimuthAngle = 0.0f;
	float zenithAngle = 0.0f;
	float bus[3] = { 0.0f, 0.0f, 0.0f };

	int widthDepthMap, heightDepthMap;
	glfwGetFramebufferSize(window, &widthDepthMap, &heightDepthMap);

	Texture shadowMap("ShadowMap", widthDepthMap, heightDepthMap, TextureTarget::Texture2D, TextureWrapMode::ClampEdge, 
										TextureFilterMode::Point, TextureFormat::DEPTH32);

	Framebuffer fb;
	fb.attachTexture(shadowMap, AttachmentType::DepthAttachment);

	GLenum status = glCheckNamedFramebufferStatus(fb.getName(), GL_FRAMEBUFFER);

	assert(status == GL_FRAMEBUFFER_COMPLETE);

	while (!glfwWindowShouldClose(window))
	{
		const float timerStartingPoint = (float)glfwGetTime();

		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ---------------------------------------------------------------------------------------------------------------------------
		// Trying to rotate the Vector for the Sun
		// this will be in it's own separate function eventually
		glm::mat4 lightSourceOrientation = glm::mat4(1.0f);
		glm::mat4 azimuthMatrix = glm::mat4(1.0f);
		glm::mat4 zenithMatrix = glm::mat4(1.0f);

		azimuthMatrix = glm::rotate(azimuthMatrix, glm::radians(azimuthAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		zenithMatrix = glm::rotate(zenithMatrix, glm::radians(zenithAngle), glm::vec3(1.0f, 0.0f, 0.0f));

		lightSourceOrientation = azimuthMatrix * zenithMatrix;

		glm::vec4 lightSourceDirection = glm::vec4(0.0f, 1001.0f, 0.0f, 0.0f);
		lightSourceDirection = lightSourceOrientation * lightSourceDirection;

		copyVec4ToFloatArray(lightSourceDirection, uniformBuffer.lightSourceDirection);
		// ------------------------------------------------------------------------------------------------------------------------------

		processCameraInput(camera, window);

		glEnable(GL_DEPTH_TEST);

		const glm::vec4 cameraWorldSpacePosition = glm::vec4(camera.getWorldPosition(), 0.0f);
		copyVec4ToFloatArray(cameraWorldSpacePosition, uniformBuffer.worldSpaceCameraPosition);

		glm::mat4 model = glm::mat4(1.0f);
		copyMat4ToFloatArray(model, uniformBuffer.model);

		glm::mat4 view = camera.createViewMatrix();
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		glm::mat4 persProj = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

		glm::mat4 viewProjection = persProj * view;
		copyMat4ToFloatArray(viewProjection, uniformBuffer.viewProjection);

		meshTestShader.useProgram(); // Make sure the shader is being used before setting these textures
		glBindVertexArray(modelVAO);

		glBindBuffer(GL_UNIFORM_BUFFER, sceneUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		int camWidth, camHeight;
		glfwGetFramebufferSize(window, &camWidth, &camHeight);

		glViewport(0, 0, camWidth, camHeight);

		const GLfloat d = 1.0f;
		glClearNamedFramebufferfv(fb.getName(), GL_DEPTH, 0, &d);

		glBindFramebuffer(GL_FRAMEBUFFER, fb.getName());

		for (const Mesh& mesh : mainModel.getMeshes())
		{
			//Upload the correct data for the Material???
			//You should check if the return of the uniform location is -1 btw
			glUniform1f(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ns"), mesh.material->m_shininess);
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ka"), 1, value_ptr(mesh.material->m_ambientColour));
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Kd"), 1, value_ptr(mesh.material->m_diffuseColour));
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ks"), 1, value_ptr(mesh.material->m_specularColour));

			assert(mesh.material->m_ambientTexture != nullptr);
			assert(mesh.material->m_diffuseTexture != nullptr);
			assert(mesh.material->m_specularTexture != nullptr);
			assert(mesh.material->m_normalMapTexture != nullptr);
			assert(mesh.material->m_maskTexture != nullptr);
			
			mesh.material->m_ambientTexture->useTexture(0);
			mesh.material->m_diffuseTexture->useTexture(1);
			mesh.material->m_specularTexture->useTexture(2);
			mesh.material->m_normalMapTexture->useTexture(3);
			mesh.material->m_maskTexture->useTexture(4);

			glDrawElements(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, (void*)(mesh.firstIndex * sizeof(unsigned int)));
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//ImGui::Begin("Shadow Map");
		ImGui::Image((void*)(intptr_t)shadowMap.getName(), ImVec2(512.0f, 512.0f));
		//ImGui::End();

		//--------------------------------------------------------------------------------------------------------------------------------------

		int winWidth, winHeight;
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, windowWidth , winHeight);

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Perspective
		glm::mat4 perspectiveProjection = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

		viewProjection = perspectiveProjection * view;
		copyMat4ToFloatArray(viewProjection, uniformBuffer.viewProjection);

		glBindBuffer(GL_UNIFORM_BUFFER, sceneUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		for (const Mesh& mesh : mainModel.getMeshes())
		{
			//Upload the correct data for the Material???
			//You should check if the return of the uniform location is -1 btw
			glUniform1f(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ns"), mesh.material->m_shininess);
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ka"), 1, value_ptr(mesh.material->m_ambientColour));
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Kd"), 1, value_ptr(mesh.material->m_diffuseColour));
			glUniform3fv(glGetUniformLocation(meshTestShader.getProgramID(), "material.Ks"), 1, value_ptr(mesh.material->m_specularColour));

			assert(mesh.material->m_ambientTexture != nullptr);
			assert(mesh.material->m_diffuseTexture != nullptr);
			assert(mesh.material->m_specularTexture != nullptr);
			assert(mesh.material->m_normalMapTexture != nullptr);
			assert(mesh.material->m_maskTexture != nullptr);

			mesh.material->m_ambientTexture->useTexture(0);
			mesh.material->m_diffuseTexture->useTexture(1);
			mesh.material->m_specularTexture->useTexture(2);
			mesh.material->m_normalMapTexture->useTexture(3);
			mesh.material->m_maskTexture->useTexture(4);

			glDrawElements(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, (void*)(mesh.firstIndex * sizeof(unsigned int)));
		}

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		// I probably do not need the copy, but can change the value in the uniform directly

		ImGui::Begin("Debug Toggles");
		ImGui::Checkbox("Normal Mapping", &normalMapBool);
		if (normalMapBool)
		{
			uniformBuffer.normalMapToggle = 1;
		}
		else
		{
			uniformBuffer.normalMapToggle = 0;
		}

		ImGui::Checkbox("Ambient", &ambientBool);
		if (ambientBool)
		{
			uniformBuffer.ambientToggle = 1;
		}
		else
		{
			uniformBuffer.ambientToggle = 0;
		}

		ImGui::Checkbox("Diffuse", &diffuseBool);
		if (diffuseBool)
		{
			uniformBuffer.diffuseToggle = 1;
		}
		else
		{
			uniformBuffer.diffuseToggle = 0;
		}

		ImGui::Checkbox("Specular", &specularBool);
		if (specularBool)
		{
			uniformBuffer.specularToggle = 1;
		}
		else
		{
			uniformBuffer.specularToggle = 0;
		}

		ImGui::SliderFloat("Azimuth", &azimuthAngle, 0.0f, 360.0f);
		ImGui::SliderFloat("Zenith", &zenithAngle, 0.0f, 90.0f);

		ImGui::ColorEdit3("Light Colour", bus);

		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		const float timerEndPoint = (float)glfwGetTime();

		const float milisecondsElapsed = (timerEndPoint - timerStartingPoint)  * 1000.0f;

		/*std::cout << "Frame Timer in miliseconds: " << milisecondsElapsed << std::endl;*/

		float averageFrameTime = measureAverageFrameTime(milisecondsElapsed, frameNumber, frameTimeArray);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		const std::string projectName = "Crytek Sponza OpenGL Project ";
		const std::string averageTimeTitle = std::to_string(averageFrameTime);
		const std::string windowTitle = projectName + averageTimeTitle;

		glfwSetWindowTitle(window, windowTitle.c_str());

		frameNumber++;
	}

	delete &fb;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}