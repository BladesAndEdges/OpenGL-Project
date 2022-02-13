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
#include<gtx/quaternion.hpp>

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
void updateShadowView(const Camera& mainView, Camera& shadowView, float zenith, float azimuth, float shadowDrawDistance, float& dimension)
{
	const glm::vec3 eulerAngles = glm::vec3(glm::radians(-zenith), glm::radians(azimuth), 0.0f);
	const glm::quat lightSpaceToWorldQuaternion = glm::quat(eulerAngles);
	const glm::mat3 orientation = glm::toMat3(lightSpaceToWorldQuaternion);

	glm::vec3 worldSpaceMainViewBounds[8];
	mainView.computeFrustumPlaneCornersInWorldSpace(mainView.getNearPlane(), worldSpaceMainViewBounds);
	mainView.computeFrustumPlaneCornersInWorldSpace(shadowDrawDistance, worldSpaceMainViewBounds + 4);

	glm::vec3 min = orientation * worldSpaceMainViewBounds[0];
	glm::vec3 max = min;

	for (uint32_t corner = 1; corner < 8; corner++)
	{
		const glm::vec3 point = orientation * worldSpaceMainViewBounds[corner];

		min = glm::min(min, point);
		max = glm::max(max, point);
	}

	const glm::vec2 cascadeDimension = glm::vec2(max - min);
 	const glm::vec3 cascadeCenterWorldSpace = glm::transpose(orientation) * ((min + max) / 2.0f);

	dimension = (cascadeDimension.x > cascadeDimension.y) ? cascadeDimension.x : cascadeDimension.y;

	shadowView.setCameraWidth(dimension);
	shadowView.setCameraHeight(dimension);
	shadowView.setCameraWorldPosition(cascadeCenterWorldSpace);
	shadowView.setCameraWorldOrientation(orientation);
}

// --------------------------------------------------------------------------------
void renderSceneFromView(const Shader& shader, const Camera&,  const UniformBuffer&, const MeshReader& model, const Framebuffer& framebuffer, 
								const Texture* shadowMap)
{

	shadowMap->useTexture(5);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getName());

	for (const Mesh& mesh : model.getMeshes())
	{
		// Eventually will be separate in a UBO per material
		glUniform1f(glGetUniformLocation(shader.getProgramID(), "material.Ns"), mesh.material->m_shininess);
		glUniform3fv(glGetUniformLocation(shader.getProgramID(), "material.Ka"), 1, value_ptr(mesh.material->m_ambientColour));
		glUniform3fv(glGetUniformLocation(shader.getProgramID(), "material.Kd"), 1, value_ptr(mesh.material->m_diffuseColour));
		glUniform3fv(glGetUniformLocation(shader.getProgramID(), "material.Ks"), 1, value_ptr(mesh.material->m_specularColour));

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
	/*ImGui::StyleColorsDark();*/
	//ImGui::StyleColorsClassic();
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
	UniformBuffer uniformBuffer;

	unsigned int uniformMatrixBlockIndex;
	uniformMatrixBlockIndex = glGetUniformBlockIndex(meshTestShader.getProgramID(), "sceneMatrices");
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textureCoordinate));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent));
	glEnableVertexAttribArray(3);

	//-----------------------------------------------------------------------------------------------------------------------------------------

	int mainViewWidth, mainViewHeight;
	glfwGetWindowSize(window, &mainViewWidth, &mainViewHeight);

	Camera mainView = Camera::perspective(mainModel.getSceneCenter(), (float)mainViewWidth,
		(float)mainViewHeight, 0.1f, 100.0f, 90.0f);

	float cascadeDimension = 1.0f;
	Camera shadowView = Camera::orthographic(glm::vec3(0.0f, 0.0f, 0.0f), cascadeDimension, cascadeDimension, -100.0f, 100.0f);

	float frameTimeArray[128];
	unsigned int frameNumber = 0;

	Texture dummyNormalMap(R"(Meshes\sponza\textures\dummy_ddn.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat,
		TextureFilterMode::Point);
	Texture dummyMask(R"(Meshes\sponza\textures\dummy_mask.png)", TextureTarget::Texture2D, TextureWrapMode::Repeat, TextureFilterMode::Point);

	// ImGui stuff
	bool normalMapBool = true;
	bool ambientBool = true;
	bool diffuseBool = true;
	bool specularBool = true;

	float azimuthAngle = 0.0f;
	float zenithAngle = 0.0f;
	float bus[3] = { 0.0f, 0.0f, 0.0f };

	float radiusInTexels = 0.0f;
	float shadowDrawDistance = 20.0f;

	// Shadow Map texture
	Texture* shadowMap = nullptr;
	bool shadowMapHasChangedSize = false;
	static int shadowMapSizeID = 0; // Here we store our selection data as an index.
	const int shadowMapSizes[6] = { 128, 256, 512, 1024, 2048, 4096 };

	Texture* shadowMapDummy = new Texture("ShadowMapDummy", shadowMapSizes[shadowMapSizeID], shadowMapSizes[shadowMapSizeID], TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
		TextureFilterMode::Bilinear, TextureFormat::DEPTH32, TextureComparisonMode::LessEqual);

	// Framebuffers
	Framebuffer shadowMapFramebuffer = Framebuffer::customFramebuffer();

	Framebuffer mainFramebuffer = Framebuffer::defaultFramebuffer();

	// Non-Comparison Sampler
	GLuint nonComparisonShadowSampler;
	glGenSamplers(1, &nonComparisonShadowSampler);

	glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	while (!glfwWindowShouldClose(window))
	{

		if ((shadowMap == nullptr) || shadowMapHasChangedSize)
		{
			if (shadowMapHasChangedSize)
			{
				shadowMapHasChangedSize = !shadowMapHasChangedSize;
			}

			delete shadowMap;
			shadowMap = new Texture("ShadowMap", shadowMapSizes[shadowMapSizeID], shadowMapSizes[shadowMapSizeID], TextureTarget::Texture2D, TextureWrapMode::ClampEdge,
				TextureFilterMode::Bilinear, TextureFormat::DEPTH32, TextureComparisonMode::LessEqual);
		}

		shadowMapFramebuffer.attachTexture(*shadowMap, AttachmentType::DepthAttachment);

		GLenum status = glCheckNamedFramebufferStatus(shadowMapFramebuffer.getName(), GL_FRAMEBUFFER);

		assert(status == GL_FRAMEBUFFER_COMPLETE);

		const float timerStartingPoint = (float)glfwGetTime();

		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		processCameraInput(mainView, window);

		glEnable(GL_DEPTH_TEST);
		
		//--------------------------------------------------------------------------------------------------------------------------------------
		// Shadow Camera rendering

		depthOnlyPassShader.useProgram(); // Make sure the shader is being used before setting these textures
		glBindVertexArray(modelVAO);

		int framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());

		const GLfloat depthClearValue = 1.0f;
		glClearNamedFramebufferfv(shadowMapFramebuffer.getName(), GL_DEPTH, 0, &depthClearValue);

		const float texelSize = 1.0f / (float)(shadowMap->getWidth());
		const float offsetScale = radiusInTexels * texelSize;

		float boundingBoxDimensions;
		updateShadowView(mainView, shadowView, zenithAngle, azimuthAngle, shadowDrawDistance, boundingBoxDimensions);
		glm::vec3 worldSpaceToLightVector = calculateWorldSpaceToLightVector(zenithAngle, azimuthAngle);
		updateUniformBuffer(uniformBuffer, shadowView, shadowView, worldSpaceToLightVector,  offsetScale, boundingBoxDimensions,
										 normalMapBool, ambientBool, diffuseBool, specularBool);

		glBindBuffer(GL_UNIFORM_BUFFER, sceneUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		renderSceneFromView(depthOnlyPassShader, shadowView, uniformBuffer, mainModel, shadowMapFramebuffer, shadowMapDummy);

		//--------------------------------------------------------------------------------------------------------------------------------------
		// Main Camera rendering

		////----------------------------------------------------------------------------------
		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//{
		//	ImGui::ShowDemoWindow(&show_demo_window);
		//}

		ImGui::Begin("Debug Toggles");

		ImGui::Checkbox("Normal Mapping", &normalMapBool);
		ImGui::Checkbox("Ambient", &ambientBool);
		ImGui::Checkbox("Diffuse", &diffuseBool);
		ImGui::Checkbox("Specular", &specularBool);

		ImGui::SliderFloat("Azimuth", &azimuthAngle, 0.0f, 360.0f);
		ImGui::SliderFloat("Zenith", &zenithAngle, 0.0f, 90.0f);

		// ImGui 
		static ImGuiComboFlags flags = 0;
		const char* items[] = { "128x128", "256x256", "512x512", "1024x1024", "2048x2048", "4096x4096"};

		const char* combo_preview_value = items[shadowMapSizeID];  // Pass in the preview value visible before opening the combo (it could be anything)
		
		if (ImGui::BeginCombo("SM Resolution", combo_preview_value, flags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (shadowMapSizeID == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					shadowMapSizeID = n;
					shadowMapHasChangedSize = !shadowMapHasChangedSize;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SliderFloat("PCF Texel Radius", &radiusInTexels, 0.0f, 100.0f);
		ImGui::SliderFloat("Shadow Draw Distance", &shadowDrawDistance, 1.0f, 200.0f);

		ImGui::End();
		//----------------------------------------------------------------------------------

		meshTestShader.useProgram();
		glBindVertexArray(modelVAO);

		int winWidth, winHeight;
		glfwGetWindowSize(window, &winWidth, &winHeight);

		mainView.setCameraWidth((float)winWidth);
		mainView.setCameraHeight((float)winHeight);

		glViewport(0, 0, winWidth , winHeight);

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		meshTestShader.useProgram();

		worldSpaceToLightVector = calculateWorldSpaceToLightVector(zenithAngle, azimuthAngle);
		updateUniformBuffer(uniformBuffer, mainView, shadowView, worldSpaceToLightVector,  offsetScale, boundingBoxDimensions,
									   normalMapBool, ambientBool, diffuseBool, specularBool);

		glBindBuffer(GL_UNIFORM_BUFFER, sceneUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		renderSceneFromView(meshTestShader, mainView, uniformBuffer, mainModel, mainFramebuffer, shadowMap);

		shadowMapDebugShader.useProgram();

		GLuint nonComparisonShadowSampler;
		glGenSamplers(1, &nonComparisonShadowSampler);

		glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(nonComparisonShadowSampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glBindSampler(0, nonComparisonShadowSampler);

		glBindTextureUnit(0, shadowMap->getName());
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindSampler(0, 0);

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

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
