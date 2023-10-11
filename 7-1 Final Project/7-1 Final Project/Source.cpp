/*
* @author dominic.drury@snhu.edu
* CS-330
* 
* Code for most shapes and the base document is provided by the tutorials provided in module 3 labeled
* "TorusExample and OpenGLSample-2objects.
*
* Camer.h contains liscensing information and originated from LearnOpenGL.com
*/

#include <iostream>     // cout, cerr
#include <GLFW/glfw3.h> // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Bmp.h"
#include "camera.h"
#include "cylinder.h"
#include "plane.h"
#include "Pyramid.h"
#include "Cube.h"
#include "shader.h"
#include "Sphere.h"
#include "Torus.h"
#include "TorusWhole.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera gCamera(glm::vec3(-1.0f, 2.0f, 6.0f));
float gLastX = SCR_WIDTH / 2.0f;
float gLastY = SCR_HEIGHT / 2.0f;
bool gFirstMouse = true;
float cameraSpeed = 2.5f;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// projection matrix
bool orthoView = false;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "7-1 Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// Position             // Texture   
		-5.0f, -5.0f, -5.0f,    0.0f, 0.0f,
		 5.0f, -5.0f, -5.0f,    1.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,    1.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,    1.0f, 1.0f,
		-5.0f, -5.0f,  5.0f,    0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,    0.0f, 0.0f,
	};

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(-4.0f,  2.0f,  3.0f),
		glm::vec3(-4.0f,  2.0f,  -3.0f),
		glm::vec3(4.0f,  2.0f,  3.0f),
		glm::vec3(4.0f,  2.0f,  -3.0f)
	};

	// first, configure the planes VAO (and VBO)
	unsigned int VBO, planesVAO;
	glGenVertexArrays(1, &planesVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position
	glBindVertexArray(planesVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// load and create textures
	// -------------------------
	unsigned int diffuseMap = loadTexture("images/wood.jfif");
	unsigned int specularMap = loadTexture("images/wood.jfif");
	unsigned int frostedGlass, blackVoid, noteBook, diceTray, dice, paper;

	/*
	* Frosted Glass Texture
	* Image by rawpixel.com on Freepik
	* https://www.freepik.com/free-photo/glass-background-with-frosted-pattern_18092852.htm#query=frosted%20glass&position=0&from_view=keyword&track=ais
	*/
	glGenTextures(1, &frostedGlass);
	glBindTexture(GL_TEXTURE_2D, frostedGlass);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("images/frostedGlass.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load frosted glass texture" << std::endl;
	}
	stbi_image_free(data);

	/*
	* Black Void Texture
	* Image originated from TorusExample's images folder from the tutorials provided in the module 3 announcmenmt.
	* Original name "screen"
	*/
	glGenTextures(1, &blackVoid);
	glBindTexture(GL_TEXTURE_2D, blackVoid);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/blackVoid.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load black void texture" << std::endl;
	}
	stbi_image_free(data);

	/*
	* Notebook Texture
	* Image originated from TorusExample's images folder from the tutorials provided in the module 3 announcmenmt.
	* Original name "seaglass2"
	*/
	glGenTextures(1, &noteBook);
	glBindTexture(GL_TEXTURE_2D, noteBook);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned char* data2 = stbi_load("images/noteBook.jpg", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load Notebook texture" << std::endl;
	}
	stbi_image_free(data2);

	/*
	* Dice Tray Texture
	* Image originated from TorusExample's images folder from the tutorials provided in the module 3 announcmenmt.
	* Original name "seaglass2"
	*/
	glGenTextures(1, &diceTray);
	glBindTexture(GL_TEXTURE_2D, diceTray);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data2 = stbi_load("images/diceTray.jpg", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load dice tray texture" << std::endl;
	}
	stbi_image_free(data2);

	/*
	* Dice Texture
	* Image originated from TorusExample's images folder from the tutorials provided in the module 3 announcmenmt.
	* Original name "Pink"
	*/
	glGenTextures(1, &dice);
	glBindTexture(GL_TEXTURE_2D, dice);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned char* data3 = stbi_load("images/dice.jpg", &width, &height, &nrChannels, 0);
	if (data3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load dice texture" << std::endl;
	}
	stbi_image_free(data3);

	/*
	* Paper Texture
	* Image originated from TorusExample's images folder from the tutorials provided in the module 3 announcmenmt.
	* Original name "transparentGlass"
	*/
	glGenTextures(1, &paper);
	glBindTexture(GL_TEXTURE_2D, paper);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data3 = stbi_load("images/paper.jpg", &width, &height, &nrChannels, 0);
	if (data3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load paper texture" << std::endl;
	}
	stbi_image_free(data3);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("material.diffuse", 0);
	ourShader.setInt("material.specular", 1);
	ourShader.setInt("frostedGlass", 2);
	ourShader.setInt("blackVoid", 3);
	ourShader.setInt("noteBook", 4);
	ourShader.setInt("diceTray", 5);
	ourShader.setInt("dice", 6);
	ourShader.setInt("paper", 7);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// activate shader
		ourShader.use();
		ourShader.setVec3("viewPos", gCamera.Position);
		ourShader.setFloat("material.shininess", 32.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient", 0.25f, 0.25f, 0.25f);
		ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.09);
		ourShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		ourShader.setVec3("pointLights[1].ambient", 0.25f, 0.25f, 0.25f);
		ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[1].constant", 1.0f);
		ourShader.setFloat("pointLights[1].linear", 0.09);
		ourShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		ourShader.setVec3("pointLights[2].ambient", 0.25f, 0.25f, 0.25f);
		ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[2].constant", 1.0f);
		ourShader.setFloat("pointLights[2].linear", 0.09);
		ourShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		ourShader.setVec3("pointLights[3].ambient", 0.25f, 0.25f, 0.25f);
		ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[3].constant", 1.0f);
		ourShader.setFloat("pointLights[3].linear", 0.09);
		ourShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		ourShader.setVec3("spotLight.position", gCamera.Position);
		ourShader.setVec3("spotLight.direction", gCamera.Front);
		ourShader.setVec3("spotLight.ambient", 1.0f, 1.0f, 0.0f);
		ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 0.0f);
		ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 0.0f);
		ourShader.setFloat("spotLight.constant", 1.0f);
		ourShader.setFloat("spotLight.linear", 0.09);
		ourShader.setFloat("spotLight.quadratic", 0.032);
		ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(7.0f)));

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection;
		if (!orthoView) {
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		else {
			projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 100.0f);
		}

		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = gCamera.GetViewMatrix();
		ourShader.setMat4("view", view);

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;

		// render the table top as 4 planes
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane table1(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		table1.render();
	
		model = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane table2(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		table2.render();
	
		model = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane table3(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		table3.render();
		
		model = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane table4(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		table4.render();

		// render the bottle body as a cylinder without a top (Cylinder.cpp file edited)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frostedGlass);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(0.0f, 1.5001f, -4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Cylinder cup(1, 50, 3, true, true, true);
		cup.render();

		// render the bottle handle as a half torus (Torus.h file edited)
		glBindTexture(GL_TEXTURE_2D, frostedGlass);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(1.0f, 1.5f, 2.0f));
		model = glm::translate(model, glm::vec3(0.489f, 0.5f, -1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		Torus cupHandle(0.1f, 0.3f, 30, 30);
		cupHandle.Draw();

		// render the dice tray as a cylinder without a top (Cylinder.cpp file edited)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diceTray);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		model = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::translate(model, glm::vec3(1.0f, 0.51f, -1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Cylinder diceTray(2, 50, 1, true, true, true);
		diceTray.render();

		// render notebook as cube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, noteBook);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(2.0f, 0.2f, 3.0f));
		model = glm::translate(model, glm::vec3(-0.85f, 0.5001f, -0.25f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Cube notebook(glm::vec4(0.5f, 0.0f, 0.0f, 0.5f));
		notebook.render();

		// render white plane as paper of notebook
		glBindTexture(GL_TEXTURE_2D, paper);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(2.0f, 0.2f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.85f, 0.5f, 0.751f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane noteBookPaper1(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		noteBookPaper1.render();
		model = glm::scale(glm::vec3(2.0f, 0.2f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.85f, 0.5f, -2.251f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane noteBookPaper2(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		noteBookPaper2.render();
		model = glm::scale(glm::vec3(1.0f, 0.2f, 3.001f));
		model = glm::translate(model, glm::vec3(-2.7001f, 0.5f, -0.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane noteBookPaper3(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		noteBookPaper3.render();
		model = glm::scale(glm::vec3(1.0f, 0.2f, 3.001f));
		model = glm::translate(model, glm::vec3(-0.6991f, 0.5f, -0.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Plane noteBookPaper4(glm::vec4(2.0f, 0.0f, 0.0f, 2.0f));
		noteBookPaper4.render();

		// render the spirals of the notebook
		glBindTexture(GL_TEXTURE_2D, blackVoid);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-5.25f, 0.25f, -4.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		TorusWhole sprial(0.1f, 0.3f, 30, 30);
		sprial.Draw();
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-5.25f, 0.25f, -2.75f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		TorusWhole sprial2(0.1f, 0.3f, 30, 30);
		sprial2.Draw();
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-5.25f, 0.25f, -1.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		TorusWhole sprial3(0.1f, 0.3f, 30, 30);
		sprial3.Draw();
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-5.25f, 0.25f, -0.25f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		TorusWhole sprial4(0.1f, 0.3f, 30, 30);
		sprial4.Draw();
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-5.25f, 0.25f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.setMat4("model", model);
		TorusWhole sprial5(0.1f, 0.3f, 30, 30);
		sprial5.Draw();

		// render white plane as paper of notebook

		// render d4 dice as triangle base pyramid 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dice);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::translate(model, glm::vec3(-1.25f, 0.51f, 0.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Pyramid d4Dice(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		d4Dice.render();

		// Render lamp object
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4((0.0f, 0.0f, 0.0f, 0.0f));
		model = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-2.0f, 7.0f, 10.0f));
		lightCubeShader.setMat4("model", model);
		static_meshes_3D::Cylinder light(0.5, 100, 0.5, true, true, true);
		light.render();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planesVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // W is forward
		gCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // S is backward
		gCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // A is left
		gCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // D is right
		gCamera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Q is down
		gCamera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // E is up
		gCamera.ProcessKeyboard(UP, deltaTime);

	// Toggle perspective and orthographic projection matrix
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // P is toggle (default false)
		orthoView = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) // P is toggle (default false)
		orthoView = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}