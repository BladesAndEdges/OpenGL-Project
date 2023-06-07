#define STB_IMAGE_IMPLEMENTATION

#include<Windows.h>
#include<winuser.h>

#define ArraySize(x) sizeof(x)/sizeof(x[0]);
#include<iostream>
#include <experimental/filesystem>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GraphicsConfigurations.h"

#include "ProfileMarker.h" // Included above glad due to windows.h
#include "Model.h"
#include "MaterialReader.h"
#include "Mesh.h"
#include "Vertex.h"

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#pragma warning(push)
#pragma warning(disable:4201)   
#include<gtc/matrix_transform.hpp>
#include<gtx/quaternion.hpp>

#pragma warning(pop)

#include "PerViewUniformData.h"
#include "Texture.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Cascade.h"
#include "GBuffer.h"
#include "UniformBuffer.h"

#include <cstdint>
#include <string>

float g_deltaTime = 0.0f;
float g_previousFrameTime = 0.0f;

float g_previousCursorX = 0.0f;
float g_previousCursorY = 0.0f;

// --------------------------------------------------------------------------------
void framebufferCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
}

// --------------------------------------------------------------------------------
void GLAPIENTRY MessageCallback(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);

		if (type != 33360)
		{
			__debugbreak();
		}

		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		if (type != 33360)
		{
			__debugbreak();
		}
		break;
	case GL_DEBUG_SEVERITY_LOW:
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		if (type != 33360) // Ignore performance warnings for now
		{
			__debugbreak();
		}
		break;
	default:
		break;
	}
}

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
glm::vec3 calculateWorldSpaceToLightVector(float zenith, float azimuth)
{
	const glm::vec3 eulerAngles = glm::vec3(glm::radians(-zenith), glm::radians(azimuth), 0.0f);
	const glm::quat lightSpaceToWorldQuaternion = glm::quat(eulerAngles);

	const glm::vec3 fromLightVectorLightSpace = glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 toLightVectorLightSpace = -fromLightVectorLightSpace;

	const glm::vec3 toLightVectorWorldSpace = lightSpaceToWorldQuaternion * toLightVectorLightSpace;
	const glm::vec3 normalizedToLightVectorWorldSpace = glm::normalize(toLightVectorWorldSpace);

	return normalizedToLightVectorWorldSpace;
}

// --------------------------------------------------------------------------------
void updateShadowView(const Camera& mainView, Camera& shadowView, float cascadeWidth, float zenith, float azimuth, float cascadeStartDistance, float cascadeEndDistance)
{

	// Compte both the World Space -> Light Space and Light Space -> World Space quaternion
	const glm::vec3 eulerAngles = glm::vec3(glm::radians(-zenith), glm::radians(azimuth), glm::radians(0.0f));
	const glm::quat lightSpaceToWorldSpaceQuaternion = glm::quat(eulerAngles);
	const glm::quat worldSpaceToLightSpaceQuaternion = glm::inverse(lightSpaceToWorldSpaceQuaternion);

	// Compute the cascades 8 world space corners
	glm::vec3 worldSpaceFrustumCorners[8u];
	mainView.computeFrustumPlaneCornersInWorldSpace(cascadeStartDistance, worldSpaceFrustumCorners);
	mainView.computeFrustumPlaneCornersInWorldSpace(cascadeEndDistance, &worldSpaceFrustumCorners[4u]);

	// Convert all corners to Light Space
	glm::vec3 lightSpaceFrustumCorners[8u];
	for (uint32_t corner = 0; corner < 8u; corner++)
	{
		lightSpaceFrustumCorners[corner] = worldSpaceToLightSpaceQuaternion * worldSpaceFrustumCorners[corner];
	}

	// Largest distance is two points along the diagonals
	const glm::vec3 diagonalVector = lightSpaceFrustumCorners[6u] - lightSpaceFrustumCorners[0u];
	const float boundingShereDiamater = glm::length(diagonalVector);

	// Get light space center
	glm::vec3 lightSpaceFrustumCenter = glm::vec3(0.0f);
	for (uint32_t corner = 0u; corner < 8u; corner++)
	{
		lightSpaceFrustumCenter = lightSpaceFrustumCenter + lightSpaceFrustumCorners[corner];
	}
	lightSpaceFrustumCenter = lightSpaceFrustumCenter * (1.0f / 8.0f);

	// Texel Snapping???
	const float texelsPerWorldUnit = (float)cascadeWidth / boundingShereDiamater;
	lightSpaceFrustumCenter = texelsPerWorldUnit * lightSpaceFrustumCenter;
	lightSpaceFrustumCenter = glm::floor(lightSpaceFrustumCenter);
	lightSpaceFrustumCenter = lightSpaceFrustumCenter / texelsPerWorldUnit;

	shadowView.setCameraWidth(boundingShereDiamater);
	shadowView.setCameraHeight(boundingShereDiamater);
	shadowView.setCameraWorldPosition(lightSpaceToWorldSpaceQuaternion * lightSpaceFrustumCenter);
	shadowView.setCameraWorldOrientation(glm::toMat4(lightSpaceToWorldSpaceQuaternion));
}

// --------------------------------------------------------------------------------
void renderAttributeToGBuffer(const Model& model, const Framebuffer& framebuffer)
{	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getName());

	const GLenum outputBuffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

	glNamedFramebufferDrawBuffers(framebuffer.getName(), 5u, outputBuffers);

	// Clear values
	const float c_positionAndNormalClearValue[3] = { 0.0f, 0.0f, 0.0f };
	const uint32_t c_diffuseAndSpecularClearValue[3] = { 0u, 0u, 0u };
	const float c_smoothnessClearValue = 0.0f;
	const GLfloat c_depthClearValue = 1.0f;

	// Clear the textures
	glClearNamedFramebufferfv(framebuffer.getName(), GL_COLOR, 0, c_positionAndNormalClearValue);
	glClearNamedFramebufferfv(framebuffer.getName(), GL_COLOR, 1, c_positionAndNormalClearValue);
	glClearNamedFramebufferuiv(framebuffer.getName(), GL_COLOR, 2, c_diffuseAndSpecularClearValue);
	glClearNamedFramebufferuiv(framebuffer.getName(), GL_COLOR, 3, c_diffuseAndSpecularClearValue);
	glClearNamedFramebufferfv(framebuffer.getName(), GL_COLOR, 4, &c_smoothnessClearValue);
	glClearNamedFramebufferfv(framebuffer.getName(), GL_DEPTH, 0, &c_depthClearValue);

	for (const Mesh& mesh : model.getMeshes())
	{
		assert(mesh.material->m_ambientTexture != nullptr);
		assert(mesh.material->m_diffuseTexture != nullptr);
		assert(mesh.material->m_specularTexture != nullptr);
		assert(mesh.material->m_normalMapTexture != nullptr);
		assert(mesh.material->m_maskTexture != nullptr);
		assert(mesh.material->m_uniformBuffer != nullptr);

		mesh.material->m_ambientTexture->useTexture(0, TextureComparisonMode::None);
		mesh.material->m_diffuseTexture->useTexture(1, TextureComparisonMode::None);
		mesh.material->m_specularTexture->useTexture(2, TextureComparisonMode::None);
		mesh.material->m_normalMapTexture->useTexture(3, TextureComparisonMode::None);
		mesh.material->m_maskTexture->useTexture(4, TextureComparisonMode::None);
		mesh.material->m_uniformBuffer->useBuffer();

		glDrawElements(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, (void*)(mesh.firstIndex * sizeof(unsigned int)));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//THIS DOESNT SEEM TO BE RENDERING FROM A VIEW ANYMORE. IT ONLY RENDERS THE OBECTS? Should it include the ubo + camera ?
// --------------------------------------------------------------------------------
void renderSceneFromView(const Camera&,  const PerViewUniformData&, const Model& model, const Framebuffer& framebuffer, 
								const Texture* shadowMap)
{
	assert(shadowMap != nullptr);
	shadowMap->useTexture(5, TextureComparisonMode::LessEqual);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getName());

	for (const Mesh& mesh : model.getMeshes())
	{
		assert(mesh.material->m_ambientTexture != nullptr);
		assert(mesh.material->m_diffuseTexture != nullptr);
		assert(mesh.material->m_specularTexture != nullptr);
		assert(mesh.material->m_normalMapTexture != nullptr);
		assert(mesh.material->m_maskTexture != nullptr);
		assert(mesh.material->m_uniformBuffer != nullptr);

		mesh.material->m_ambientTexture->useTexture(0, TextureComparisonMode::None);
		mesh.material->m_diffuseTexture->useTexture(1, TextureComparisonMode::None);
		mesh.material->m_specularTexture->useTexture(2, TextureComparisonMode::None);
		mesh.material->m_normalMapTexture->useTexture(3, TextureComparisonMode::None);
		mesh.material->m_maskTexture->useTexture(4, TextureComparisonMode::None);
		mesh.material->m_uniformBuffer->useBuffer();
		glDrawElements(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, (void*)(mesh.firstIndex * sizeof(unsigned int)));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// --------------------------------------------------------------------------------
void imguiStyleSetting()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style->FramePadding = ImVec2(4, 2);
	style->ItemSpacing = ImVec2(10, 2);
	style->IndentSpacing = 12;
	style->ScrollbarSize = 10;

	style->WindowRounding = 4;
	style->FrameRounding = 4;
	style->PopupRounding = 4;
	style->ScrollbarRounding = 6;
	style->GrabRounding = 4;
	style->TabRounding = 4;

	style->WindowTitleAlign = ImVec2(1.0f, 0.5f);
	style->WindowMenuButtonPosition = ImGuiDir_Right;

	style->DisplaySafeAreaPadding = ImVec2(4, 4);
}

// --------------------------------------------------------------------------------
int main()
{
	// GLFW-related variables
	GLFWwindow* window = nullptr;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
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
	imguiStyleSetting();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 450 core";
	ImGui_ImplOpenGL3_Init(glsl_version);

	
	glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	Shader meshTestShader(R"(Shaders\meshTestShader.vert)", R"(Shaders\meshTestShader.frag)");
	Shader depthOnlyPassShader(R"(Shaders\depthOnlyPass.vert)", R"(Shaders\depthOnlyPass.frag)");
	Shader shadowMapDebugShader(R"(Shaders\shadowMapDebug.vert)", R"(Shaders\shadowMapDebug.frag)");

	//UBO
	PerViewUniformData perViewUniforms;
	UniformBuffer perViewUniformBuffer(6u, sizeof(PerViewUniformData), nullptr, "PerViewUniformBuffer");

	MaterialReader materialReader;
	materialReader.parseMaterialFile(R"(Meshes\sponza\sponza.mtl)");
	Model sponzaModel(R"(SponzaModel.compiled)", R"(Meshes\sponza\sponza.obj)", materialReader);

	//-----------------------------------------------------------------------------------------------------------------------------------------

	int mainViewWidth, mainViewHeight;
	glfwGetFramebufferSize(window, &mainViewWidth, &mainViewHeight);

	Camera mainView = Camera::perspective(sponzaModel.getSceneCenter(), (float)mainViewWidth,
		(float)mainViewHeight, 0.1f, 100.0f, 90.0f);

	float frameTimeArray[128];
	unsigned int frameNumber = 0;

	Texture dummyNormalMap(R"(Meshes\sponza\textures\dummy_ddn.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat,
		TextureFilterMode::Point);
	Texture dummyMask(R"(Meshes\sponza\textures\dummy_mask.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Point);

	// Shadow Map texture
	Texture* shadowMap = nullptr;
	const uint32_t shadowMapSizes[6] = { 128, 256, 512, 1024, 2048, 4096 }; // Possibly move this into the GraphicsConfig class as options

	std::vector<Cascade> cascades;

	Framebuffer mainFramebuffer = Framebuffer::defaultFramebuffer();

	// -----------------------------------------------------------GBuffer
	GBuffer* gBuffer = nullptr; // Assume no resizes for now
	Shader gBufferPassShader(R"(Shaders\gBufferPassShader.vert)", R"(Shaders\gBufferPassShader.frag)");
	Shader lightingPassShader(R"(Shaders\lightingPassShader.vert)", R"(Shaders\lightingPassShader.frag)");

	GraphicsConfigurations graphicsConfigurations;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Update all configuration settings accessible via the UI
		graphicsConfigurations.update();

		// Update Camera
		int currentFramebufferSizeX, currentFramebufferSizeY;
		glfwGetFramebufferSize(window, &currentFramebufferSizeX, &currentFramebufferSizeY);

		mainView.setCameraWidth((float)currentFramebufferSizeX);
		mainView.setCameraHeight((float)currentFramebufferSizeY);

		processCameraInput(mainView, window);

		// Shadow Map == nullptr only for frame 0; And for a moment whilst deleted (?)
		if ((shadowMap == nullptr) || (graphicsConfigurations.getNumberOfActiveCascades() != (uint32_t)cascades.size()) || ((uint32_t)shadowMapSizes[graphicsConfigurations.getShadowMapDimensionsId()] != shadowMap->getWidth()))
		{
			// Recreate the Shadow Map
			delete shadowMap;
			shadowMap = new Texture("ShadowMap", shadowMapSizes[graphicsConfigurations.getShadowMapDimensionsId()], shadowMapSizes[graphicsConfigurations.getShadowMapDimensionsId()], graphicsConfigurations.getNumberOfActiveCascades(), TextureTarget::ArrayTexture2D,
				TextureWrapMode::ClampEdge, TextureFilterMode::Bilinear, TextureFormat::DEPTH32);

			// Update Cascades
			cascades.clear();

			while ((uint32_t)cascades.size() < graphicsConfigurations.getNumberOfActiveCascades())
			{
				cascades.emplace_back(shadowMap, (uint32_t)cascades.size());
			}
		}

		for (const Cascade& cascade : cascades)
		{
			assert(cascade.getShadowMap()->getHeight() == shadowMapSizes[graphicsConfigurations.getShadowMapDimensionsId()]);
			assert(cascade.getShadowMap()->getWidth() == shadowMapSizes[graphicsConfigurations.getShadowMapDimensionsId()]);
			assert(cascades.size() == graphicsConfigurations.getNumberOfActiveCascades());
		}

		// Check both dimensions as they can be dragged separately
		if ((gBuffer == nullptr) || (gBuffer->getWidth() != mainView.getViewWidth()) || (gBuffer->getHeight() != mainView.getViewHeight()))
		{
			// Recreate the GBuffer
			delete gBuffer;
			gBuffer = new GBuffer((uint32_t)mainView.getViewWidth(), (uint32_t)mainView.getViewHeight());
		}

		const float timerStartingPoint = (float)glfwGetTime();

		glEnable(GL_DEPTH_TEST);

		//--------------------------------------------------------------------------------------------------------------------------------------
		// Shadow Camera rendering

		depthOnlyPassShader.useProgram(); // Make sure the shader is being usegd before setting these textures

		// Max is 200
		// 7%, 20%, 50% 100%
		const float cascadeSplitStartDistances[4] =
		{
			0.1f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			0.07f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			0.2f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			0.5f * graphicsConfigurations.getMaximumShadowDrawDistance()
		};

		const float cascadeSplitEndDistances[4] =
		{
			0.07f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			0.2f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			0.5f * graphicsConfigurations.getMaximumShadowDrawDistance(),
			1.0f * graphicsConfigurations.getMaximumShadowDrawDistance()
		};

		float offsetScale = 0.0f;
		uint32_t cascadeIndex = 0u;

		// Depth Pass(es);
		glBindVertexArray(sponzaModel.getVAO());
		for (Cascade& cascade : cascades)
		{

			const std::string debugMarkerName = "Cascade " + std::to_string(cascadeIndex);

			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, debugMarkerName.c_str());

			int framebufferWidth, framebufferHeight;
			glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

			const uint32_t width = cascade.getShadowMap()->getWidth();
			const uint32_t height = cascade.getShadowMap()->getHeight();
			glViewport(0, 0, cascade.getShadowMap()->getWidth(), cascade.getShadowMap()->getHeight());
			
			const GLfloat clearValue = 1.0f;
			glClearNamedFramebufferfv(cascade.getFramebuffer().getName(), GL_DEPTH, 0, &clearValue);

			const float texelSize = 1.0f / (float)(cascade.getShadowMap()->getWidth());
			offsetScale = graphicsConfigurations.getSamplingRadiusInTexelUnits() * texelSize;

			const float cascadeStartDistance = cascadeSplitStartDistances[cascadeIndex];
			const float cascadeEndDistance = cascadeSplitEndDistances[cascadeIndex];
			updateShadowView(mainView, cascade.getCascadeView(), (float)cascade.getShadowMap()->getWidth(), graphicsConfigurations.getGlobalLightSourceZenith(), graphicsConfigurations.getGlobalLightSourceAzimuth(), 
				cascadeStartDistance, cascadeEndDistance);

			glm::vec3 worldSpaceToLightVector = calculateWorldSpaceToLightVector(graphicsConfigurations.getGlobalLightSourceZenith(), graphicsConfigurations.getGlobalLightSourceAzimuth());

			updateUniformBuffer(perViewUniforms, cascade.getCascadeView(), cascade.getCascadeView(), cascadeSplitEndDistances[cascadeIndex], cascadeIndex, worldSpaceToLightVector, offsetScale, graphicsConfigurations.getMaximumShadowDrawDistance(), graphicsConfigurations.getFadedShadowsStartDistance(),
				graphicsConfigurations.getNormalMappingEnabled(), graphicsConfigurations.getDiffuseLightingEnabled(), graphicsConfigurations.getSpecularLightingEnabled(), graphicsConfigurations.getCascadesOverlayModeEnabled());

			perViewUniformBuffer.update(&perViewUniforms);
			perViewUniformBuffer.useBuffer();
			renderSceneFromView(cascade.getCascadeView(), perViewUniforms, sponzaModel, cascade.getFramebuffer(), cascade.getShadowMap());

			cascadeIndex++;

			glPopDebugGroup();
		}

		//----------------------------------------------------------------------------------
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		// What to do about this. Isn't this meaning cascade 0 values get partially updated ahead of others ?
		const glm::vec3 worldSpaceToLightVector = calculateWorldSpaceToLightVector(graphicsConfigurations.getGlobalLightSourceZenith(), graphicsConfigurations.getGlobalLightSourceAzimuth()); // Issue ?
		updateUniformBuffer(perViewUniforms, mainView, cascades[0].getCascadeView(), cascadeSplitStartDistances[1], 0u, worldSpaceToLightVector, offsetScale, graphicsConfigurations.getMaximumShadowDrawDistance(),
			graphicsConfigurations.getFadedShadowsStartDistance(), graphicsConfigurations.getNormalMappingEnabled(), graphicsConfigurations.getDiffuseLightingEnabled(), graphicsConfigurations.getSpecularLightingEnabled(), graphicsConfigurations.getCascadesOverlayModeEnabled());

		perViewUniformBuffer.update(&perViewUniforms);
		perViewUniformBuffer.useBuffer();

		if (graphicsConfigurations.getRendererType() == RendererType::Forward)
		{
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Forward Renderer Drawing");
			meshTestShader.useProgram();
			glBindVertexArray(sponzaModel.getVAO());
			glViewport(0, 0,(GLsizei)mainView.getViewWidth(), (GLsizei)mainView.getViewHeight());
			renderSceneFromView(mainView, perViewUniforms, sponzaModel, mainFramebuffer, shadowMap);
			glPopDebugGroup();
		}

		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Shadow Map Debug Quad");
		shadowMapDebugShader.useProgram();

		glViewport(0, 0, (GLsizei)mainView.getViewWidth(), (GLsizei)mainView.getViewHeight());
		shadowMap->useTexture(0, TextureComparisonMode::None);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, graphicsConfigurations.getNumberOfActiveCascades());

		glPopDebugGroup();

		if (graphicsConfigurations.getRendererType() == RendererType::Deferred)
		{
			// GBuffer test rendering
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Render to GBuffer Textures");
			gBufferPassShader.useProgram();
			glViewport(0, 0, (GLsizei)mainView.getViewWidth(), (GLsizei)mainView.getViewHeight());
			renderAttributeToGBuffer(sponzaModel, gBuffer->getFramebuffer());
			glPopDebugGroup();

			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Deferred Renderer Drawing");

			lightingPassShader.useProgram();

			gBuffer->getWorldPositionTexture()->useTexture(0, TextureComparisonMode::None);
			gBuffer->getWorldNormalTexture()->useTexture(1, TextureComparisonMode::None);
			gBuffer->getDiffuseColourTexture()->useTexture(2, TextureComparisonMode::None);
			gBuffer->getSpecularColourTexture()->useTexture(3, TextureComparisonMode::None);
			gBuffer->getSmoothnessTexture()->useTexture(4, TextureComparisonMode::None);
			shadowMap->useTexture(5, TextureComparisonMode::LessEqual);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glPopDebugGroup();
		}

		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGui Debug Window Rendering");
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glPopDebugGroup();

		glfwSwapBuffers(window);

		const float timerEndPoint = (float)glfwGetTime();

		const float milisecondsElapsed = (timerEndPoint - timerStartingPoint)  * 1000.0f;

		float averageFrameTime = measureAverageFrameTime(milisecondsElapsed, frameNumber, frameTimeArray);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		const std::string projectName = "Crytek Sponza OpenGL Project ";
		const std::string averageTimeTitle = std::to_string(averageFrameTime);
		const std::string windowTitle = projectName + averageTimeTitle;

		glfwSetWindowTitle(window, windowTitle.c_str());

		frameNumber++;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
