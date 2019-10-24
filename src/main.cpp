#include "main.h"

void resize(GLFWwindow *window, int width, int height){
    windowWidth = width;
    windowHeight = height;
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
}

void updateFrameTime() {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

bool initWindow(){
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

    // The window couldn't be created
    if (!window){
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);

    // Window resize callback
	glfwSetFramebufferSizeCallback(window, resize);
	// Window keyboard callback
	glfwSetKeyCallback(window, onKeyPress);
	// Window mouse motion callback
	glfwSetCursorPosCallback(window, onMouseMotion);
    return true;
}

bool initGlad(){

    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status){
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

void initGL(){
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

bool init() {

	// Initialize the window, glad components and framebuffer
	if (!initWindow() || !initGlad())
		return false;
	// Initialize the opengl context
	initGL();
	// Loads the shader
	rayTracingShader = new Shader("assets/shaders/rayTracing.vert", "assets/shaders/rayTracing.frag");
	// Loads all the geometry into the GPU
	buildGeometry();

	return true;
}

void buildGeometry(){
	// Quad for debug purposes:
	float quadVertices[] = {
		// positions        // Color   		   // texture Coords
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};
	// Setup plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

unsigned int loadTexture(const char *path){
    unsigned int id;
    // Creates the texture on GPU
    glGenTextures(1, &id);
    // Loads the texture
    int textureWidth, textureHeight, numberOfChannels;
    // Flips the texture when loads it because in opengl the texture coordinates are flipped
    stbi_set_flip_vertically_on_load(true);
    // Loads the texture file data
    unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
    if (data){
        // Gets the texture channel format
        GLenum format;
        switch (numberOfChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        // Creates the texture mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR:: Unable to load texture " << path << std::endl;
        glDeleteTextures(1, &id);
    }
    // We dont need the data texture anymore because is loaded on the GPU
    stbi_image_free(data);

    return id;
}

void processKeyboardInput(GLFWwindow *window){

    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Checks if the r key is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        // Reloads the shader
        delete rayTracingShader;
		rayTracingShader = new Shader("assets/shaders/rayTracing.vert", "assets/shaders/rayTracing.frag");
    }

	if (cameraMode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.moveForward(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.moveLeft(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.moveRight(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.moveBackward(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.moveUp(deltaTime);

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.moveDown(deltaTime);
	}
}

void onKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_C:
			if (!cameraMode) {
				cameraMode = true;
				glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				cameraMode = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			break;
		case GLFW_KEY_V:
			rebounds++;
			break;
		case GLFW_KEY_B:
			if (rebounds > 1)
				rebounds--;
		}
	}
}

void onMouseMotion(GLFWwindow* window, double xpos, double ypos){
	if (cameraMode) {
		glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
		double xoffset = ((windowWidth / 2.0) - xpos) * mouseSpeed * deltaTime;
		double yoffset = ((windowHeight / 2.0) - ypos) * mouseSpeed * deltaTime;

		//std::cout << xoffset << "," << yoffset << std::endl;
		camera.mouseUpdate(glm::vec2(xoffset, yoffset));
	}
}

void drawQuad(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	rayTracingShader->use();


	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, .5f, 1000.0f);
	glm::mat4 view = camera.getWorldToViewMatrix();
	glm::mat4 vp = projection * view;
	glm::mat4 invVP = glm::inverse(vp);
	rayTracingShader->setMat4("invVP", invVP);
	rayTracingShader->setVec3("eye", camera.position);
	rayTracingShader->setInt("rebounds", rebounds);
	//Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void render(){

    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw QUAD
	drawQuad();

	// Set default viewport size
	glViewport(0, 0, windowWidth, windowHeight);

	// Swap the buffer
    glfwSwapBuffers(window);
}

void update(){

    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window)){
		// Update frame time
		updateFrameTime();
        // Checks for keyboard inputs
        processKeyboardInput(window);
        // Renders everything
        render();
        // Check and call events
        glfwPollEvents();
    }
}

int main(int argc, char const *argv[]){

    // Initialize all the app components
    if (!init()){
        // Something went wrong
        std::cin.ignore();
        return -1;
    }

    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;
    // Starts the app main loop
    update();

    // Deletes the texture from the gpu
    glDeleteTextures(1, &textureID);
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
    delete rayTracingShader;

    // Stops the glfw program
    glfwTerminate();

    return 0;
}
