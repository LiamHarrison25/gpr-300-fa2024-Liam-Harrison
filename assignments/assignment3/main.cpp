#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/camera.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <iostream>
#include <ew/procGen.h>
#include <cstdlib>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ew/procGen.h>

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

struct G_Buffer
{
	GLuint fbo;
	GLuint world_position;
	GLuint world_normal;
	GLuint albeto;
	GLuint depth;
};

struct PointLight
{
	glm::vec3 position;
	float radius;
	glm::vec4 color;
};

const int MAX_POINT_LIGHTS = 64;
PointLight pointLights[MAX_POINT_LIGHTS];


float planeVertices[] =
{
	// positions            // normals         // texcoords
		-29.0f, -6.5f, -29.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		-29.0f, -6.5f,  29.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		 29.0f, -6.5f,  29.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,

		 29.0f, -6.5f, -29.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
		-29.0f, -6.5f, -29.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 29.0f, -6.5f,  29.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f
};

unsigned int planeVAO, planeVBO;

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

unsigned int fbo;
unsigned int colorBuffer;

struct Material
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

const int NUM_MONKIES = 64;


G_Buffer g_buffer;



void create_pass()
{
	//G_Buffer g_buffer;

	//setting up the quad
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

	//send in quad data
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	//Create frame buffers

	//second fbo:

	glCreateFramebuffers(0, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//fbo:

	glCreateFramebuffers(1, &g_buffer.fbo); //create the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.fbo);

	


	//world_position:


	glGenTextures(1, &g_buffer.world_position); //create the texture
	glBindTexture(GL_TEXTURE_2D, g_buffer.world_position); //binding the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_buffer.world_position, 0); //attaching the color buffer to the frame buffer


	//world_normal:

	
	glGenTextures(1, &g_buffer.world_normal); //create the texture
	glBindTexture(GL_TEXTURE_2D, g_buffer.world_normal); //binding the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_buffer.world_normal, 0); //attaching the color buffer to the frame buffer


	//albeto:


	glGenTextures(1, &g_buffer.albeto); //create the texture
	glBindTexture(GL_TEXTURE_2D, g_buffer.albeto); //binding the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_buffer.albeto, 0); //attaching the color buffer to the frame buffer


	//depth:


	glGenTextures(1, &g_buffer.depth); //create the texture
	glBindTexture(GL_TEXTURE_2D, g_buffer.depth); //binding the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_buffer.depth, 0); //attaching the color buffer to the frame buffer

	GLenum buffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, buffers);	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//error with the framebuffer
		std::cout << "framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::vec4 RandomColor()
{
	float red = (rand() % 100 + 1) / 100.0f;
	float blue = (rand() % 100 + 1) / 100.0f;
	float green = (rand() % 100 + 1) / 100.0f;
	float alpha = 1;

	return glm::vec4(red, green, blue, alpha);
}

void createQuad()
{
	

	//-------------------------

	//setting up the quad
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

	//send in quad data
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	//-------------------------
}



int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	GLuint chipTexture = ew::loadTexture("assets/chip.jpg");

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader geometryPassShader = ew::Shader("assets/geometryPass.vert", "assets/geometryPass.frag");
	ew::Shader displayShader = ew::Shader("assets/lightingPass.vert", "assets/lightingPass.frag");
	ew::Shader lightOrbShader = ew::Shader("assets/lightOrb.vert", "assets/lightOrb.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Transform monkeyTransform;

	ew::Mesh plane = ew::createPlane(1024, 1024, 1028);
	ew::Transform planeTransform;
	planeTransform.position.y = -1;

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	create_pass();
	createQuad();

	ew::Transform monkeyArray[64];

	ew::Mesh sphereMesh = ew::Mesh(ew::createSphere(1.0f, 8));

	int width = 8;
	int spacing = 5;

	int i, j;
	
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < width; j++)
		{
			int index = j * width + i;
			monkeyTransform.position.z = i * spacing;
			monkeyTransform.position.x = j * spacing;
			monkeyTransform.position.y = 1;
			monkeyArray[index] = monkeyTransform;
			pointLights[index].position.z = i * spacing;
			pointLights[index].position.x = j * spacing;
			pointLights[index].position.y = 3;
			pointLights[index].color = RandomColor();
			pointLights[index].radius = 5.0f;
		}
	}


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// math
		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;


		for (i = 0; i < NUM_MONKIES - 1; i++)
		{
			monkeyArray[i].rotation = glm::rotate(monkeyArray[i].rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		}
		
		cameraController.move(window, &camera, deltaTime);

		//Geometry pass-----------------------------------------------------------------
		#pragma region GeometryPass

		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.fbo);

		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glBindTextureUnit(0, chipTexture);

		geometryPassShader.use();

		geometryPassShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		geometryPassShader.setInt("_MainTex", 0);


		for (i = 0; i < NUM_MONKIES - 1; i++)
		{
			geometryPassShader.setMat4("_Model", monkeyArray[i].modelMatrix());
			monkeyModel.draw();
		}

		glBindTextureUnit(0, brickTexture);

		geometryPassShader.setMat4("_Model", planeTransform.modelMatrix());
		plane.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		#pragma endregion

		//Lighting pass-----------------------------------------------------------------
		#pragma region LightingPass

		glEnable(GL_DEPTH_TEST);



		glBindVertexArray(screenVAO);

		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTextureUnit(0, g_buffer.world_position);
		glBindTextureUnit(1, g_buffer.world_normal);
		glBindTextureUnit(2, g_buffer.albeto);

		displayShader.use();




		//NOTE: OLD
		/*displayShader.setInt("gPosition", 0);
		displayShader.setInt("gNormal", 1);
		displayShader.setInt("gAlbedo", 2);

		displayShader.setFloat("_Material.Ka", material.Ka);
		displayShader.setFloat("_Material.Kd", material.Kd);
		displayShader.setFloat("_Material.Ks", material.Ks);
		displayShader.setFloat("_Material.Shininess", material.Shininess);

		displayShader.setVec3("_EyePos", camera.position);*/


		//NEW: ----------------------

		displayShader.setInt("_MainTex", 0);

		displayShader.setInt("gPosition", 0);
		displayShader.setInt("gNormal", 1);
		displayShader.setInt("gAlbedo", 2);

		displayShader.setMat4("_Model", monkeyTransform.modelMatrix());
		displayShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		displayShader.setVec3("_EyePos", camera.position);

		displayShader.setFloat("_Material.Ka", material.Ka);
		displayShader.setFloat("_Material.Kd", material.Kd);
		displayShader.setFloat("_Material.Ks", material.Ks);
		displayShader.setFloat("_Material.Shininess", material.Shininess);

		

		//--------------------------------

		//set shader light uniforms
		for (i = 0; i < MAX_POINT_LIGHTS; i++)
		{
				std::string prefix = "_PointLights[" + std::to_string(i) + "].";
				displayShader.setVec3(prefix + "position", pointLights[i].position);
				displayShader.setFloat(prefix + "radius", pointLights[i].radius);
				displayShader.setVec4(prefix + "color", pointLights[i].color);
		}

		

		#pragma endregion

		//draws the post processing quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//Render light spheres: --------------------------------------------------------
		#pragma region LightSpheres

		glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); //write to current fbo

		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		//Draw the light orbs

		lightOrbShader.use();
		
		lightOrbShader.setMat4("_ViewProjection", camera.projectionMatrix()* camera.viewMatrix());

		float lightScaleRadius = 0.2f;

		for (i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			glm::mat4 m = glm::mat4(1.0f);
			m = glm::translate(m, pointLights[i].position);
			m = glm::scale(m, glm::vec3(lightScaleRadius));

			lightOrbShader.setMat4("_Model", m);
			lightOrbShader.setVec3("_Color", pointLights[i].color);
			sphereMesh.draw();
		}

		#pragma endregion
		//-----------------
		

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

	ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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

	ImVec2 windowSize;
	ImVec2 texSize = ImVec2(300, 300);

	ImGui::Begin("Gbuffer");

	ImGui::Image((ImTextureID)g_buffer.world_position, texSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::Image((ImTextureID)g_buffer.albeto, texSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::Image((ImTextureID)g_buffer.world_normal, texSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::Image((ImTextureID)g_buffer.depth, texSize, ImVec2(0, 1), ImVec2(1, 0));


	ImGui::End();

	

	//ImGui::Begin("FBO");
	////Stretch image to be window size
	//windowSize = ImGui::GetWindowSize();
	////Invert 0-1 V to flip vertically for ImGui display
	////shadowMap is the texture2D handle
	//ImGui::Image((ImTextureID)g_buffer.fbo, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();


	//ImGui::Begin("albeto");
	////Stretch image to be window size
	//windowSize = ImGui::GetWindowSize();
	////Invert 0-1 V to flip vertically for ImGui display
	////shadowMap is the texture2D handle
	//ImGui::Image((ImTextureID)g_buffer.albeto, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();

	//ImGui::Begin("depth");
	////Stretch image to be window size
	//windowSize = ImGui::GetWindowSize();
	////Invert 0-1 V to flip vertically for ImGui display
	////shadowMap is the texture2D handle
	//ImGui::Image((ImTextureID)g_buffer.depth, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();

	//ImGui::Begin("world_normal");
	////Stretch image to be window size
	//windowSize = ImGui::GetWindowSize();
	////Invert 0-1 V to flip vertically for ImGui display
	////shadowMap is the texture2D handle
	//ImGui::Image((ImTextureID)g_buffer.world_normal, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();

	//ImGui::Begin("world_position");
	////Stretch image to be window size
	//windowSize = ImGui::GetWindowSize();
	////Invert 0-1 V to flip vertically for ImGui display
	////shadowMap is the texture2D handle
	//ImGui::Image((ImTextureID)g_buffer.world_position, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();

	

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

