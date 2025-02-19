#include <iostream> // cout, cerr
#include <cstdlib> // EXIT_FAILURE
#include <GL/glew.h> // GLEW library
#include <GLFW/glfw3.h> // GLFW library

// GLM Math Header Inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

/*
* Shader Program Macro
*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace {
	const char* const WINDOW_TITLE = "3-3 Assignment: Building a 3D Pyramid"; //Window title

	// Window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh {
		GLuint vao;      // Handle for the vertex array object
		GLuint vbos[2];  // Handles for the vertex buffer objects
		GLuint nIndices; // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	//Triangle mesh data
	GLMesh gMesh;
	// Shader program
	GLuint gProgramId;
}

/*
* User-defined Funtion Initialization
*/
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh &mesh);
void UDestroyMesh(GLMesh &mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId);
void UDestroyShaderProgram(GLuint programId);

/*
* Vertex Shader Source Code
*/
const GLchar* vertexShaderSource = GLSL(440,
	layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout (location = 1) in vec4 color; // Color data from Vertex Attrib Pointer 1

	out vec4 vertexColor; // Variable to transfer color data to the fragment shader

	//Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
		vertexColor = color; // References incoming color data
	}
);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

    out vec4 fragmentColor;

    void main()
    {
        fragmentColor = vec4(vertexColor);
    }
);

int main(int argc, char* argv[]) {
	if (!UInitialize(argc, argv, &gWindow)) {
		return EXIT_FAILURE;
	}

	// Create the mesh
	UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

	// Create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId)) {
		return EXIT_FAILURE;
	}
	
	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// render loop
	while (!glfwWindowShouldClose(gWindow)) {
		// input
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	UDestroyMesh(gMesh);

	// Release shader program
	UDestroyShaderProgram(gProgramId);

	exit(EXIT_SUCCESS); // Exits the program successfully
}

/*
* Initialize GLFW, GLEW, and create a window
*/
bool UInitialize(int argc, char* argv[], GLFWwindow** window) {
	// GLFW: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

	if (*window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	// GLEW: initialize if using version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

/*
* Process all input: query GLFW whether relavant keys are pressed or released this frame and react accordingly
*/
void UProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

/*
* glfw: whenever the window size is changed this callback function executes
*/
void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

/*
* Function call to render a frame
*/
void URender() {
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Scales the object by 2
	glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
	// Rotates shape by 0 degrees
	glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f));
	// Places object at the origin
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 model = translation * rotation * scale;

	// Transforms the camera
	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 1, -3), // Camera is at (x, y, z) in the world space (camera position)
		glm::vec3(0, 0, 0),  // Camera looks at the origin (camera target)
		glm::vec3(0, 1, 0)   // Camera is right side up (normal view) (set to 0, -1, 0 to look upside down)
	);

	// Creates a perspective projection (field of view, aspect ratio, near plane, and far plane are the four parameters)
	glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

	// Sets the shader program to be used
	glUseProgram(gProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Activates the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vao);

	// Draws triangle
	glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);

	// Deactivates Vertex Array Onject (vao)
	glBindVertexArray(0);

	// glfw: swap buffers and poll IO events (keys pressed, keys released, mouse moved, etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

/*
* Impliments the UCreateMesh function
*/
void UCreateMesh(GLMesh &mesh) {
	// Specifies normalized device coordinates (x, y, z) amd color (RGBA) for the pyramid vertices
	GLfloat verts[] = {
		// Vertex Positions  // Colors (r,g,b,a)
		0.0f,  1.0f,   0.0f,  1.0f, 1.0f, 1.0f, 1.0f, // Vert: 0  Color: White   Loc: Top
		1.0f, -1.0f,  -1.0f,  0.5f, 1.0f, 1.0f, 1.0f, // Vert: 1  Color: Cyan    Loc: Left
		0.5f, -1.25f, -2.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // Vert: 2  Color: Purple  Loc: Front
		0.5f, -1.0f,   1.0f,  0.0f, 0.0f, 1.0f, 1.0f, // Vert: 3  Color: Blue    Loc: Inside
	   -1.0f, -1.0f,  -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // Vert: 4  Color: Yellow  Loc: Right

	};

	// Index data to share position data
	GLushort indices[] = {
		0, 2, 4, // Triangle 1 Front
		0, 1, 2, // Triangle 2 Left
		0, 3, 4, // Triangle 3 Right
		0, 1, 3, // Triangle 4 Back
		1, 2, 3, // Triangle 5 Bottom
		2, 3, 4, // Triangle 6 Bottom
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;

	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}

/*
* Impliments the UDestroyMesh function
*/
void UDestroyMesh(GLMesh& mesh) {
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}

/*
* Impliments the UCreateShaders function
*/
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId) {
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}

/*
* Impliments the UCreateShaders function
*/
void UDestroyShaderProgram(GLuint programId) {
	glDeleteProgram(programId);
}