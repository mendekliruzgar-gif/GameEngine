#include <glad/glad.h>
#include <iostream>
#include <glfw3.h>
#include <fstream>
#include <sstream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, GLint anchorLoc, GLint xAxisLoc, GLint yAxisLoc);
void drawRectangle(float x, float y, float width, float height, GLint anchorLoc, GLint xAxisLoc, GLint yAxisLoc);
void checkGlError(const std::string& context);

bool roofColorToggle = false;

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

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Game Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    checkGlError("Init GLAD");

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

    float baseTriangle[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseTriangle), baseTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    checkGlError("Buffer Setup");

    GLint anchorLoc = glGetUniformLocation(shaderProgram, "anchor");
    GLint xAxisLoc = glGetUniformLocation(shaderProgram, "xAxis");
    GLint yAxisLoc = glGetUniformLocation(shaderProgram, "yAxis");
    GLint colorLocation = glGetUniformLocation(shaderProgram, "uColor");

    while (!glfwWindowShouldClose(window)) {
		processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        if (roofColorToggle)
            glUniform4f(colorLocation, 1.0f, 0.5f, 0.0f, 1.0f);
        else
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        drawTriangle(-0.9f, 0.2f, 0.9f, 0.2f, 0.0f, 0.9f, anchorLoc, xAxisLoc, yAxisLoc);

        glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        drawRectangle(-0.6f, -0.8f, 1.2f, 1.0f, anchorLoc, xAxisLoc, yAxisLoc);

        glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
        drawRectangle(-0.15f, -0.8f, 0.3f, 0.5f, anchorLoc, xAxisLoc, yAxisLoc);

        glUniform4f(colorLocation, 1.0f, 1.0f, 0.0f, 1.0f);
        drawRectangle(-0.5f, -0.2f, 0.3f, 0.3f, anchorLoc, xAxisLoc, yAxisLoc);

        glUniform4f(colorLocation, 1.0f, 1.0f, 0.0f, 1.0f);
        drawRectangle(0.2f, -0.2f, 0.3f, 0.3f, anchorLoc, xAxisLoc, yAxisLoc);

        glUniform4f(colorLocation, 0.1f, 0.1f, 0.1f, 1.0f);
        drawRectangle(0.2f, 0.4f, 0.3f, 0.6f, anchorLoc, xAxisLoc, yAxisLoc);

        glBindVertexArray(0);
        checkGlError("Render Loop");

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, GLint anchorLoc, GLint xAxisLoc, GLint yAxisLoc)
{
    glUniform2f(anchorLoc, x1, y1);
    glUniform2f(xAxisLoc, x2 - x1, y2 - y1);
    glUniform2f(yAxisLoc, x3 - x1, y3 - y1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(float x, float y, float width, float height, GLint anchorLoc, GLint xAxisLoc, GLint yAxisLoc)
{
    drawTriangle(x, y, x + width, y, x, y + height, anchorLoc, xAxisLoc, yAxisLoc);
    drawTriangle(x + width, y, x + width, y + height, x, y + height, anchorLoc, xAxisLoc, yAxisLoc);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool spacePressedLastFrame = false;
    bool spacePressedThisFrame = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spacePressedThisFrame && !spacePressedLastFrame)
        roofColorToggle = !roofColorToggle;
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
        std::cout << "OpenGL Error (" << context << "): " << err << std::endl;
    }
}