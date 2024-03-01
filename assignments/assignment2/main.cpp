#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/transform.h>
#include <ew/texture.h>
#include <ew/procGen.h>



//#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

ew::Camera camera;
ew::CameraController cameraController;

struct Material {
	float Ka = 1.0; 
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

//glm::mat4 lightSpaceMatrix;
//glm::mat4 model;

unsigned int depthMap;
	unsigned int depthMapFBO;
int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Shader shadow = ew::Shader("assets/shadow.vert", "assets/shadow.frag");

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");

	
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees
	

	ew::Transform monkeyTransform;

	monkeyTransform.position = glm::vec3(0, 0, 0);

	ew::Mesh planeMesh = ew::Mesh(ew::createPlane(15, 15, 5));

	ew::Transform planeTransform;

	planeTransform.position = glm::vec3(0, -2, 0);
	

	const unsigned int Shadow_W = 2048, Shadow_H = 2048;

	glCreateFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glGenTextures(1, &depthMap);

	

	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, Shadow_W, Shadow_H);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);



	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap,0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	

	


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing
	//glm::vec3 plane[6] = {glm::vec3(5,0,5), glm::vec3(-5,0,5), glm::vec3(5,0,-5), glm::vec3(-5,0,-5), glm::vec3(-5,0,5), glm::vec3(5,0,-5) };

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;



		

		//RENDER

	


		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, Shadow_W, Shadow_H);
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		glClear( GL_DEPTH_BUFFER_BIT);


		float nearPlane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.f, 10.0f, -10.0f, 10.0f, nearPlane, far_plane);
		//glm::mat4 lightProjection = glm::ortho(-1.f, 1.0f, -1.0f, 1.0f, nearPlane, far_plane);
		
		glm::vec3 lightPos = glm::vec3(0, 5.0f, 1.0f);

		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;






		//rendering Shadow 

		shadow.use();

		shadow.setMat4("_ViewProjection", lightSpaceMatrix);
		shadow.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw();


		shadow.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1080, 720);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);


		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, screenWidth, screenHeight);
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		//glClearDepth(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", depthMap);
		}

		shader.use();




		//render scene
		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);




		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);

		//Rotate model around Y axis

		cameraController.move(window, &camera, deltaTime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);


		shader.setMat4("_Model", monkeyTransform.modelMatrix());
		shader.setMat4("_ViewProjection", camera.projectionMatrix()* camera.viewMatrix());
		shader.setVec3("_EyePos", camera.position);
		shader.setMat4("_LightViewProj", lightSpaceMatrix);
		monkeyModel.draw();
		

		//floor
		shader.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();
		
		
		//Make "_MainTex" sampler2D sample from the 2D texture bound to unit 0
		shader.setInt("_MainTex", 0);
		shader.setInt("_ShadowMap", 1);
		//shadow mapping
		





		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void render(const ew::Shader& shader) {

}


void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}


void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Shadow Map");

	ImGui::BeginChild("ShadowMap");

	ImVec2 WindowSize = ImGui::GetWindowSize();

	ImGui::Image((ImTextureID)depthMap, WindowSize, ImVec2(0, 1), ImVec2(1, 0));


	ImGui::EndChild();
	ImGui::End();

	

	ImGui::Begin("Settings");

	ImGui::Text("Add Controls Here!");
	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseD", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
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

