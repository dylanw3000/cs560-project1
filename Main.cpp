
#define _CRT_SECURE_NO_WARNINGS

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "shader.h"
#include "Camera.h"
// #include "model.h"
#include "animator.h"

// assimp
#include <assimp/Importer.hpp>

const unsigned int width = 1200;
const unsigned int height = 800;


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "Dylan Washburne - Digipen", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);


	/*** time ***/
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	/********* Load *********/
	stbi_set_flip_vertically_on_load(true);

	Model mainModel("resources/objects/yodan/steve.x");
	Animation mainAnimation("resources/objects/yodan/steve.x", 0, &mainModel);
	Animation animTwo("resources/objects/yodan/steve.x", 1, &mainModel);


	Animator animator(&mainAnimation);
	Animator animator2(&animTwo);
	// animator.UpdateAnimation(0.05);


	Model spiderModel("resources/objects/fbx/Spider.fbx");
	Animation spiderAnimation("resources/objects/fbx/Spider.fbx", 0, &spiderModel);
	Animator spiderAnimator(&spiderAnimation);


	Model yodanModel("resources/objects/yodan/yodan.x");
	Animation yodanWalk("resources/objects/yodan/yodan.x", 0, &yodanModel);
	Animation yodanRun("resources/objects/yodan/yodan.x", 1, &yodanModel);
	Animator yodanWalkAnimator(&yodanWalk);
	Animator yodanRunAnimator(&yodanRun);


	/****** Shaders ******/
	Shader animationShader("shaders/default.vert", "shaders/default.frag");
	// Shader lightCubeShader("shaders/light.vert", "shaders/light.frag");
	Shader lineShader("shaders/lines.vert", "shaders/lines.frag");
	Shader staticShader("shaders/static.vert", "shaders/static.frag");

	



	//
	float rotationX = 0.0f;
	float rotationY = 0.0f;
	float rotationZ = 0.0f;
	glm::vec3 modelPosition(0.f, -0.8f, 0.f);
	double prevTime = glfwGetTime();

	Camera camera(width, height, glm::vec3(0.0f, 5.0f, 20.0f));

	glEnable(GL_DEPTH_TEST);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Variables to be changed in the ImGUI window
	bool drawObject = true;
	bool drawOther = true;
	bool drawWireframe = false;
	bool drawSkeleton = true;
	float size = 1.0f;
	float color[4] = { 0.8f, 0.3f, 0.6f, 1.0f };

	bool animNo = false;



	glm::vec3 lightPos(1.2f, 1.0f, 10.0f);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Animation Updates
		animator.UpdateAnimation(deltaTime);
		animator2.UpdateAnimation(deltaTime);

		spiderAnimator.UpdateAnimation(deltaTime);

		yodanWalkAnimator.UpdateAnimation(deltaTime);
		yodanRunAnimator.UpdateAnimation(deltaTime);


		/********* *********/
		// be sure to activate shader when setting uniforms/drawing objects
		animationShader.use();
		animationShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		animationShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		animationShader.setVec3("lightPos", lightPos);

		// view/projection transformations
		/*
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		*/
		animationShader.setMat4("camMatrix", camera.cameraMatrix);

		{
			if (animNo) {
				auto transforms = animator2.GetFinalBoneMatrices();
				for (int i = 0; i < transforms.size(); i++)
					animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			else {
				auto transforms = animator.GetFinalBoneMatrices();
				for (int i = 0; i < transforms.size(); i++)
					animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			
		}

		// model
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, modelPosition);
		model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotationZ), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.1f));
		
		animationShader.setMat4("model", model);

		// render the object
		if (drawObject) {
			mainModel.Draw(animationShader);

			if (drawOther) {
				{
					if (animNo) { // boolean to pass either the walk or run animation
						auto transforms = yodanRunAnimator.GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); i++)
							animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
					}
					else {
						auto transforms = yodanWalkAnimator.GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); i++)
							animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
					}
					animationShader.setMat4( "model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5, -.8, -20)), glm::vec3(0.1f) ) );

					yodanModel.Draw(animationShader);
				}
			}
		}

		
		
		lineShader.use();
		lineShader.setMat4("camMatrix", camera.cameraMatrix);
		lineShader.setMat4("model", model);

		{
			if (animNo) {	// boolean to pass either the walk or run animation
				auto transforms = animator2.GetFinalBoneMatrices();
				for (int i = 0; i < transforms.size(); i++)
					lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			else {
				auto transforms = animator.GetFinalBoneMatrices();
				for (int i = 0; i < transforms.size(); i++)
					lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
		}

		if (drawWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mainModel.Draw(lineShader);

			if (drawOther) {
				{
					if (animNo) {
						auto transforms = yodanRunAnimator.GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); i++)
							lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
					}
					else {
						auto transforms = yodanWalkAnimator.GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); i++)
							lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
					}
					lineShader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5, -.8, -20)), glm::vec3(0.1f)));

					yodanModel.Draw(lineShader);
				}
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


		if (drawSkeleton) {
			glClear(GL_DEPTH_BUFFER_BIT);
			//mainModel.
			// mainAnimation.DrawSkelly();
			lineShader.setMat4("model", model);
			if (animNo) {
				animator2.DrawSkelly();
			}
			else {
				animator.DrawSkelly();
			}

			if (drawOther) {
				lineShader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5, -.8, -20)), glm::vec3(0.1f)));
				if (animNo) {
					yodanRunAnimator.DrawSkelly();
				}
				else {
					yodanWalkAnimator.DrawSkelly();
				}
			}
			
		}
		
		

		/*
		staticShader.use();
		staticShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		staticShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		staticShader.setVec3("lightPos", lightPos);

		staticShader.setMat4("camMatrix", camera.cameraMatrix);
		staticShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.005f)));

		{
			animationShader.use();
			auto transforms = spiderAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); i++) {
				animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			animationShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.005f)));
		}

		spiderModel.Draw(animationShader);

		lineShader.use();
		{
			auto transforms = spiderAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); i++) {
				lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			lineShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.005f)));
		}

		if (drawWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			spiderModel.Draw(lineShader);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


		if (drawSkeleton) {
			glClear(GL_DEPTH_BUFFER_BIT);
			spiderAnimator.DrawSkelly();
		}
		*/
		


		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			prevTime = crntTime;
		}


		{
			float rotationSpeed = 5.0f;
			if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
			{
				rotationY -= rotationSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL) == GLFW_PRESS)
			{
				rotationY += rotationSpeed;
			}

			float translationSpeed = 0.2f;
			if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
			{
				modelPosition.x -= translationSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			{
				modelPosition.x += translationSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
			{
				modelPosition.z += translationSpeed;
			}
			if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			{
				modelPosition.z -= translationSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
			{
				modelPosition = glm::vec3(0.f, -0.8f, 0.f);
				rotationX = rotationY = rotationZ = 0.f;
			}
		}

		// ImGUI window creation
		ImGui::Begin("ImGUI Window");

		ImGui::Text("Debug Views");
		ImGui::Checkbox("Draw Object", &drawObject);
		ImGui::Checkbox("Draw Wireframe", &drawWireframe);
		ImGui::Checkbox("Draw Skeleton", &drawSkeleton);

		ImGui::Text("Rotation");
		// ImGui::SliderFloat("Rotation X", &rotationX, 0.0f, 360.0f);
		ImGui::SliderFloat("Rotation Y", &rotationY, 0.0f, 360.0f);
		// ImGui::SliderFloat("Rotation Z", &rotationZ, 0.0f, 360.0f);

		ImGui::Text("Lighting");
		ImGui::SliderFloat("Light X", &lightPos.x, -5.f, 5.f);
		ImGui::SliderFloat("Light Y", &lightPos.y, -5.f, 5.f);
		ImGui::SliderFloat("Light Z", &lightPos.z, -10.f, 10.f);

		ImGui::Checkbox("Draw Other", &drawOther);
		ImGui::Checkbox("Animation Swap", &animNo);
		// color editor
		// ImGui::ColorEdit4("Color", color);
		// Ends the window
		ImGui::End();


		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
