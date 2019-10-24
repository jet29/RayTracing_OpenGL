#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stb_image.h>
#include "Shader.h"
#include "Camera.h"

// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "GPU Ray Tracing Demo - UCV/2019";
// Window pointer
GLFWwindow *window;
// Camera class
Camera camera;
// Shader object
Shader *rayTracingShader;
// Index (GPU) of the geometry buffer
unsigned int VBO;
// Index (GPU) vertex array object
unsigned int VAO;
// Index (GPU) of the texture
unsigned int textureID;
// Time control variables
float deltaTime = 0.0f, lastFrame = 0.0f;
float mouseSpeed = 16.0f;
bool cameraMode = false;
int rebounds = 1;


// MVP Matrices
glm::mat4 View;
glm::mat4 Projection;
glm::mat4 ModelMatrix;

#define XDIM 256
#define YDIM 256
#define ZDIM 256


/**
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height);

/**
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow();

void updateFrameTime();

/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad();

/**
 * Initialize the opengl context
 * */
void initGL();

/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry();

/**
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */
unsigned int loadTexture(const char *path);

/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init();

/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */
void processKeyboardInput(GLFWwindow *window);

void onKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods);

void onMouseMotion(GLFWwindow* window, double xpos, double ypos);

void drawQuad();

/**
 * Render Function
 * */
void render();

/**
 * App main loop
 * */
void update();

/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[]);