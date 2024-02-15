#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/camera.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void resetCamera(ew::Camera* camera, ew::CameraController* controller);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

ew::CameraController cameraController;

ew::Camera camera;

struct Material
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	GLuint chipTexture = ew::loadTexture("assets/chip.jpg");
	GLuint greenAO = ew::loadTexture("assets/greenshell/greenshell_AO.png");
	GLuint greenCOL = ew::loadTexture("assets/greenshell/greenshell_col.png");
	GLuint greenMTL = ew::loadTexture("assets/greenshell/greenshell_mtl.png");
	GLuint greenRGH = ew::loadTexture("assets/greenshell/greenshell_rgh.png");
	GLuint greenSPC = ew::loadTexture("assets/greenshell/greenshell_spc.png");

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader pbrShader = ew::Shader("assets/pbr.vert", "assets/pbr.frag");

	ew::Model greenShellModel = ew::Model("assets/greenshell/greenshell.obj");
	ew::Transform greenShellTransform;

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		greenShellTransform.rotation = glm::rotate(greenShellTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		cameraController.move(window, &camera, deltaTime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, greenAO);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, greenCOL);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, greenMTL);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, greenRGH);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, greenSPC);

		pbrShader.use();

		pbrShader.setInt("_MainTex", 0);

		pbrShader.setMat4("_Model", greenShellTransform.modelMatrix());
		pbrShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		pbrShader.setVec3("cameraPos", camera.position);
		//pbrShader.setVec3("lightPos", )
		//pbrShader.setVec3("ambientLight", )
		//pbrShader.setVec3("worldNormal", )
		//pbrShader.setVec3("lightColor", )

		pbrShader.setFloat("pbrMaterial.color", greenCOL);
		pbrShader.setFloat("pbrMaterial.albeto", greenAO);
		pbrShader.setFloat("pbrMaterial.metalic", greenMTL);
		pbrShader.setFloat("pbrMaterial.occlusion", greenRGH);
		pbrShader.setFloat("pbrMaterial.specular", greenSPC);

		greenShellModel.draw();

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");

	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}

	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &cameraController);
	}

	ImGui::Text("Add Controls Here!");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

