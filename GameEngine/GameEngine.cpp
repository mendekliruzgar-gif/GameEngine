#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string LoadShader(const char* aPath);
void checkGlError(const std::string& context);

bool isPerspective = true;
bool spacePressedLastFrame = false;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    checkGlError("Init GLAD");

    glEnable(GL_DEPTH_TEST);

    std::string vertexCode = LoadShader("Assets/Shaders/VertexShader.glsl");
    std::string fragmentCode = LoadShader("Assets/Shaders/FragmentShader.glsl");

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    checkGlError("Shader Setup");

    float cubeVertices[] = {
		// positions          // colors
        -0.3f, -0.3f, -0.3f,  1.0f, 0.0f, 0.0f,
         0.3f, -0.3f, -0.3f,  0.0f, 1.0f, 0.0f,
         0.3f,  0.3f, -0.3f,  0.0f, 0.0f, 1.0f,
        -0.3f,  0.3f, -0.3f,  1.0f, 1.0f, 0.0f,
        -0.3f, -0.3f,  0.3f,  1.0f, 0.0f, 1.0f,
         0.3f, -0.3f,  0.3f,  0.0f, 1.0f, 1.0f,
         0.3f,  0.3f,  0.3f,  0.5f, 0.5f, 0.5f,
        -0.3f,  0.3f,  0.3f,  1.0f, 0.5f, 0.0f
    };
    unsigned int cubeIndices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 4, 7,  7, 3, 0,
        1, 5, 6,  6, 2, 1,
        3, 2, 6,  6, 7, 3,
        0, 1, 5,  5, 4, 0
    };

    unsigned int cubeVBO, cubeVAO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<float> sphereVertices;
	std::vector<unsigned int> sphereIndices;
    int sectorCount = 12;
	int stackCount = 12;
	float radius = 0.35f;

	for (int i = 0; i <= stackCount; ++i) {
		float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);

			sphereVertices.push_back(x);
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);

            sphereVertices.push_back((x / radius + 1.0f) * 0.5f);
            sphereVertices.push_back((y / radius + 1.0f) * 0.5f);
            sphereVertices.push_back(0.8f);
		}
	}

    for (int i = 0; i < stackCount; ++i) {
		int k1 = i * (sectorCount + 1);
		int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }
            if (i != stackCount - 1) {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
        }
    }

    unsigned int sphereVBO, sphereVAO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	float pyramidVertices[] = {
        // Positions          // Colors
         0.3f, 0.0f, -0.3f,   0.2f, 1.0f, 0.2f,
         0.3f, 0.0f,  0.3f,   0.2f, 0.2f, 1.0f,
        -0.3f, 0.0f,  0.3f,   1.0f, 1.0f, 0.2f,
         0.0f, 0.6f,  0.0f,   1.0f, 0.5f, 1.0f
	};

    unsigned int pyramidIndices[] = {
        0, 1, 2,
        0, 3, 2,
        2, 3, 1,
        1, 0, 3,
    };

    unsigned int pyramidVBO, pyramidVAO, pyramidEBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glGenBuffers(1, &pyramidEBO);

    glBindVertexArray(pyramidVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    checkGlError("Buffer Setups");

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    GLint objTypeLoc = glGetUniformLocation(shaderProgram, "uObjectType");
    GLint basePosLoc = glGetUniformLocation(shaderProgram, "uBasePosition");

    while (!glfwWindowShouldClose(window)) {
		processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

		float currentTime = static_cast<float>(glfwGetTime());
        glUniform1f(timeLoc, currentTime);

        glm::mat4 projection(1.0f);
        if (isPerspective) {
			projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -4.5f));
        }
        else {
			projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 0.1f, 100.0f);
			projection = glm::translate(projection, glm::vec3(0.0f, 0.0f, -4.5f));
        }

        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(cubeVAO);

        glUniform1i(objTypeLoc, 0);
        glUniform3f(basePosLoc, -1.2f, 0.6f, 0.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glUniform1i(objTypeLoc, 1);
        glUniform3f(basePosLoc, -1.2f, -0.6f, 0.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        glBindVertexArray(sphereVAO);

        glUniform1i(objTypeLoc, 2);
        glUniform3f(basePosLoc, 0.0f, 0.6f, 0.0f);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices.size()), GL_UNSIGNED_INT, 0);

        glUniform1i(objTypeLoc, 3);
        glUniform3f(basePosLoc, 0.0f, -0.6f, 0.0f);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices.size()), GL_UNSIGNED_INT, 0);


		glBindVertexArray(pyramidVAO);

        glUniform1i(objTypeLoc, 4);
        glUniform3f(basePosLoc, 1.2f, 0.3f, 0.0f);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glUniform1i(objTypeLoc, 5);
        glUniform3f(basePosLoc, 1.2f, -0.9f, 0.0f);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        checkGlError("Render Loop");

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);

    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteBuffers(1, &pyramidEBO);

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool spacePressedThisFrame = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spacePressedThisFrame && !spacePressedLastFrame) {
        isPerspective = !isPerspective;
        std::cout << (isPerspective ? "Perspective" : "Orthographic") << std::endl;
    }
    spacePressedLastFrame = spacePressedThisFrame;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void checkGlError(const std::string& context)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL Error (" << context << "): code" << err << std::endl;
    }
}

std::string LoadShader(const char* aPath)
{
    std::string shaderCode;
    std::ifstream shaderFile;

    shaderFile.open(aPath);

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();

    shaderFile.close();
    shaderCode = shaderStream.str();

    return shaderCode;
}