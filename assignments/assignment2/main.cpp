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

#include <iostream>

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

//created framebuffer
unsigned int fbo;
unsigned int colorBuffer;
unsigned int depthBuffer;
unsigned int depthMapFbo;

float quad[] =
{
	//triangle 1
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,

	//triangle 2
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f
};

unsigned int screenVAO, screenVBO;

struct Material
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;


static void create_debug_pass()
{
	float debug_vertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f };
	glGenVertexArrays(1, &fbo);
	//glBindFramebuffer(1, );

}

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	GLuint chipTexture = ew::loadTexture("assets/chip.jpg");

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Transform monkeyTransform;
	ew::Shader shadowShader = ew::Shader("assets/shadow.vert", "assets/shadow.frag");
	ew::Shader debugShader = ew::Shader("assets/debug.vert", "assets/debug.frag");

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	////setting up the quad
	//glGenVertexArrays(1, &screenVAO);
	//glGenBuffers(1, &screenVBO);

	//glBindVertexArray(screenVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

	////send in quad data
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//glBindVertexArray(0);

	////Create frame buffers

	

   // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

	//Bind RBO to FBO
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//error with the framebuffer
		std::cout << "framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//Lighting variables
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		cameraController.move(window, &camera, deltaTime);

		// reset viewport because the debug quad needed a new viewport
		glViewport(0, 0, screenWidth, screenHeight);


		// RENDER FROM POV OF LIGHT TO DEPTH BUFFER
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		shadowShader.use();
		shadowShader.setMat4("_Model", monkeyTransform.modelMatrix());
		shadowShader.setMat4("_ViewProjection", lightSpaceMatrix);
		monkeyModel.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		///////////////////////////////////////////


     	// RENDER FROM POV OF OF CAMERA USING LIT
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, chipTexture);

		shader.setMat4("_Model", monkeyTransform.modelMatrix());
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setVec3("_EyePos", camera.position);
		monkeyModel.draw();
		///////////////////////////////////////////

  //   	// RENDER DEBUG QUAD USING DEPTH BUFFER
		//glViewport(screenWidth - 150, 0, 150, 150);

		//glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//debugShader.use();

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
		/////////////////////////////////////////////

		//glClear(GL_DEPTH_BUFFER_BIT);
		//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);

		//shadowShader.use();
		//shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix); //from learn openGl
		//
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, chipTexture);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glViewport(0, 0, screenWidth, screenHeight);

		////debug pipeline:

		////glClearColor()
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//debugShader.use();
		//debugShader.setVec3("vPos", screenWidth - 150, screenHeight - 150, 0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 0);

		/*glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, chipTexture);

		shadowShader.use();

		shadowShader.setInt("_MainTex", 0);

		shadowShader.setMat4("_Model", monkeyTransform.modelMatrix());
		shadowShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shadowShader.setVec3("_EyePos", camera.position);

		shadowShader.setFloat("_Material.Ka", material.Ka);
		shadowShader.setFloat("_Material.Kd", material.Kd);
		shadowShader.setFloat("_Material.Ks", material.Ks);
		shadowShader.setFloat("_Material.Shininess", material.Shininess);

		monkeyModel.draw();*/

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		//glClear(GL_DEPTH_BUFFER_BIT);

		////draw full screen quad
		//glDisable(GL_DEPTH_TEST);
		//glBindVertexArray(screenVAO);
		////glBindTexture(GL_TEXTURE_2D, colorBuffer);

		//glDrawArrays(GL_TRIANGLES, 0, 6);

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

