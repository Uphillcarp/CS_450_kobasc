#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <GL/glew.h>					
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "MeshData.hpp"
#include "MeshGLData.hpp"
#include "GLSetup.hpp"
#include "Shader.hpp"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>
#include "glm/gtc/matrix_transform.hpp"  
#include "glm/gtx/transform.hpp" 
#include "glm/gtx/string_cast.hpp" 
#include "glm/gtc/type_ptr.hpp" 
#include "Utility.hpp"
#define GLM_ENABLE_EXPERIMENTAL
using namespace std;

float rotAngle = 0.0f;
glm::vec3 eye = glm::vec3(0,0,1);
glm::vec3 lookAt = glm::vec3(0,0,0);
glm::vec3 cameraDir = lookAt - eye;
glm::vec2 mousePos;

glm::mat4 makeLocalRotate(glm::vec3 offset, glm::vec3 axis, float angle){
	glm::mat4 R(1.0);

	R = glm::translate(-offset)*R;
	R = glm::rotate(glm::radians(angle), axis)*R;
	R = glm::translate(offset)*R;

	return R;
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos){
    glm::vec2 relMouse = mousePos - glm::vec2(xpos, ypos);
	cameraDir = lookAt - eye;

	int fw, fh;
	glfwGetFramebufferSize(window, &fw, &fh);
	if(fw > 0 && fh > 0){
		relMouse.x = relMouse.x/float(fw);
		relMouse.y = relMouse.y/float(fh);

		glm::mat4 x = makeLocalRotate(eye, glm::vec3(0,1,0), 30.0f * relMouse.x);
		glm::mat4 y = makeLocalRotate(eye, glm::cross(cameraDir, glm::vec3(0,1,0)), 
										30.0f * relMouse.y);

		glm::vec4 lookAtV = glm::vec4(lookAt, 1.0)*x*y;

		lookAt = glm::vec3(lookAtV);
	}

	mousePos = glm::vec2(xpos, ypos);
}

glm::mat4 makeRotateZ(glm::vec3 offset){
	glm::mat4 R(1.0);

	R = glm::translate(-offset)*R;
	R = glm::rotate(glm::radians(rotAngle), glm::vec3(0,0,1))*R;
	R = glm::translate(offset)*R;

	return R;
}

void renderScene(vector<MeshGL> &allMeshes, aiNode *node, glm::mat4 parentMat,  
					GLint modelMatLoc, int level){					
	aiMatrix4x4 transform = node->mTransformation;
	glm::mat4 nodeT;

	aiMatToGLM4(transform, nodeT);

	glm::mat4 modelMat = parentMat*nodeT;
	glm::vec3 pos = glm::vec3(modelMat[3].x, modelMat[3].y, modelMat[3].z);

	glm::mat4 R = makeRotateZ(pos);

	glm::mat4 tmpModel = R * modelMat;
	glUniformMatrix4fv(modelMatLoc, 1, false, glm::value_ptr(tmpModel));

	for(int i = 0; i < node->mNumMeshes; i++){
		int index = node->mMeshes[i];
		drawMesh(allMeshes.at(index));
	}
	
	for(int i = 0; i < node->mNumChildren; i++){
		renderScene(allMeshes, node->mChildren[i], modelMat, modelMatLoc, level+1);
	}
}

static void key_callback(GLFWwindow *window,
                        int key, int scancode,
                        int action, int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        if(key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
		else if(key == GLFW_KEY_W){
			cameraDir = lookAt - eye;
		}
		else if(key == GLFW_KEY_A){
			
		}
		else if(key == GLFW_KEY_S){
			cameraDir = lookAt - eye;
		}
		else if(key == GLFW_KEY_D){
			
		}
		else if(key == GLFW_KEY_J){
			rotAngle += 1.0;
		}
		else if(key == GLFW_KEY_K){
			rotAngle -= 1.0;
		}
    }
}

void extractMeshData(aiMesh *mesh, Mesh &m){
	// Clear out vertices and elements
	m.vertices.clear();
	m.indices.clear();

	for(int i = 0; i < mesh->mNumVertices; i++){
		Vertex v;
		v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, 
								mesh->mVertices[i].z);
		v.color = glm::vec4(1.0, 0.0, 0.0, 1.0f);

		m.vertices.push_back(v);
	}

	for(int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];

		for(int j = 0; j < face.mNumIndices; j++){
			m.indices.push_back(face.mIndices[j]);
		}
	}
}

// Create very simple mesh: a quad (4 vertices, 6 indices, 2 triangles)
void createSimpleQuad(Mesh &m) {
	// Clear out vertices and elements
	m.vertices.clear();
	m.indices.clear();

	// Create four corners
	Vertex upperLeft, upperRight;
	Vertex lowerLeft, lowerRight;

	// Set positions of vertices
	// Note: glm::vec3(x, y, z)
	upperLeft.position = glm::vec3(-0.5, 0.5, 0.0);
	upperRight.position = glm::vec3(0.5, 0.5, 0.0);
	lowerLeft.position = glm::vec3(-0.5, -0.5, 0.0);
	lowerRight.position = glm::vec3(0.5, -0.5, 0.0);

	// Set vertex colors (red, green, blue, white)
	// Note: glm::vec4(red, green, blue, alpha)
	upperLeft.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	upperRight.color = glm::vec4(0.0, 1.0, 0.0, 1.0);
	lowerLeft.color = glm::vec4(0.0, 0.0, 1.0, 1.0);
	lowerRight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Add to mesh's list of vertices
	m.vertices.push_back(upperLeft);
	m.vertices.push_back(upperRight);	
	m.vertices.push_back(lowerLeft);
	m.vertices.push_back(lowerRight);
	
	// Add indices for two triangles
	m.indices.push_back(0);
	m.indices.push_back(3);
	m.indices.push_back(1);

	m.indices.push_back(0);
	m.indices.push_back(2);
	m.indices.push_back(3);
}

// Create very simple mesh: a pentagon (6 vertices, 9 indices, 3 triangles)
void createSimplePentagon(Mesh &m) {
	// Clear out vertices and elements
	m.vertices.clear();
	m.indices.clear();

	// Create four corners
	Vertex upperLeft, upperRight;
	Vertex lowerLeft, lowerRight;
	Vertex pointLeft, pointRight;

	// Set positions of vertices
	// Note: glm::vec3(x, y, z)
	upperLeft.position = glm::vec3(-0.5, 0.5, 0.0);
	upperRight.position = glm::vec3(0.5, 0.5, 0.0);
	lowerLeft.position = glm::vec3(-0.5, -0.5, 0.0);
	lowerRight.position = glm::vec3(0.5, -0.5, 0.0);
	pointLeft.position = glm::vec3(-0.9, 0.0, 0.0);
	pointRight.position = glm::vec3(0.9, 0.0, 0.0);


	// Set vertex colors (red, green, blue, white)
	// Note: glm::vec4(red, green, blue, alpha)
	upperLeft.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	upperRight.color = glm::vec4(0.0, 1.0, 0.0, 1.0);
	lowerLeft.color = glm::vec4(0.0, 0.0, 1.0, 1.0);
	lowerRight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	pointLeft.color = glm::vec4(1.0, 1.0, 0.0, 1.0);
	pointRight.color = glm::vec4(1.0, 1.0, 0.0, 1.0);

	// Add to mesh's list of vertices
	m.vertices.push_back(upperLeft);
	m.vertices.push_back(upperRight);	
	m.vertices.push_back(lowerLeft);
	m.vertices.push_back(lowerRight);
	m.vertices.push_back(pointLeft);
	m.vertices.push_back(pointRight);
	
	// Add indices for two triangles
	m.indices.push_back(0);
	m.indices.push_back(3);
	m.indices.push_back(1);
	
	m.indices.push_back(0);
	m.indices.push_back(2);
	m.indices.push_back(3);

	m.indices.push_back(1);
	m.indices.push_back(3);
	m.indices.push_back(5);
}

// Main 
int main(int argc, char **argv) {
	//Checking CMD Args
	string model = "./sampleModels/sphere.obj";

	if(argc >= 2)
	{
		model = argv[1];
	}

	//Setting Scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(model, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

	//Error checking
	if(scene == NULL){
		cout << "ERROR: SCENE NULL" << endl;
		exit(1);
	}
	else if(scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE){
		cout << "ERROR: mFlags AI_SCENE_FLAGS_INCOMPLETE" << endl;
		exit(1);
	}
	else if(scene->mRootNode == NULL){
		cout << "ERROR : mRootNode NULL" << endl;
		exit(1);
	}

	// Are we in debugging mode?
	bool DEBUG_MODE = true;

	// GLFW setup
	// Switch to 4.1 if necessary for macOS
	GLFWwindow* window = setupGLFW("Assign05: kobasc", 4, 3, 800, 800, DEBUG_MODE);

	// GLEW setup
	setupGLEW(window);

	//Getting cursor position
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);

	mousePos = glm::vec2(mx, my);

	// Check OpenGL version
	checkOpenGLVersion();

	// Set key_callback
	glfwSetKeyCallback(window, key_callback);

	// Set mouse_position_callback
	glfwSetCursorPosCallback(window, mouse_position_callback);

	// Hides cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	// Set up debugging (if requested)
	if(DEBUG_MODE) checkAndSetupOpenGLDebugging();

	// Set the background color to a shade of blue
	glClearColor(0.5f, 1.3f, 0.7f, 1.0f);	

	// Create and load shaders
	GLuint programID = 0;
	try {		
		// Load vertex shader code and fragment shader code
		string vertexCode = readFileToString("./shaders/Assign05/Basic.vs");
		string fragCode = readFileToString("./shaders/Assign05/Basic.fs");

		// Print out shader code, just to check
		if(DEBUG_MODE) printShaderCode(vertexCode, fragCode);

		// Create shader program from code
		programID = initShaderProgramFromSource(vertexCode, fragCode);
	}
	catch (exception e) {		
		// Close program
		cleanupGLFW(window);
		exit(EXIT_FAILURE);
	}

	glGetUniformLocation(programID, "projMat");

	// Create simple quad or pentagon
	// Mesh m;
	// createSimpleQuad(m);
	// createSimplePentagon(m);

	// Create OpenGL mesh (VAO) from data
	// MeshGL mgl;
	// createMeshGL(m, mgl);

	vector<MeshGL> myVector;

	for(int i = 0; i < scene->mNumMeshes; i++){
		Mesh m;
		MeshGL mesh;

		extractMeshData(scene->mMeshes[i], m);
		createMeshGL(m, mesh);

		myVector.push_back(mesh);
	}

	GLint modelMatLoc = glGetUniformLocation(programID, "modelMat");
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Set viewport size
		int fwidth, fheight;
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(0, 0, fwidth, fheight);

		// Clear the framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use shader program
		glUseProgram(programID);

		// Draw object
		// drawMesh(mgl);
		/*
		for(int i = 0; i < myVector.size(); i++){
			drawMesh(myVector[i]);
		}
		*/
		renderScene(myVector, scene->mRootNode, glm::mat4(1.0), modelMatLoc, 0);

		// Swap buffers and poll for window events		
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Sleep for 15 ms
		this_thread::sleep_for(chrono::milliseconds(15));
	}

	// Clean up mesh
	// cleanupMesh(mgl);

	for(int i = 0; i < myVector.size(); i++){
		cleanupMesh(myVector[i]);
	}

	// Clean up shader programs
	glUseProgram(0);
	glDeleteProgram(programID);
		
	// Destroy window and stop GLFW
	cleanupGLFW(window);

	return 0;
}
