#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>


#define STB_IMAGE_IMPLEMENTATION
#include "../include/libs/stb_image.h"

#include "../include/libs/SimplexNoise.h"

// Simple texture loading function 
    unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture wrapping and filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        stbi_image_free(data); // free the loaded image memory
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

//==================== File Loader =====================

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ================= Shader sources =================
    
std::string vertCode = loadShaderSource("src/shaders/shader.vert");
std::string fragCode = loadShaderSource("src/shaders/shader.frag");

const char* vertexShaderSource   = vertCode.c_str();
const char* fragmentShaderSource = fragCode.c_str();


// ================= Camera =================
glm::vec3 cameraPos   = glm::vec3(0.0f, 20.0f, 30.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float yaw   = -90.0f;
float pitch = -20.0f;
bool firstMouse = true;

float deltaTime = 5.0f;
float lastFrame = 0.0f;

// ================== Input ==================
void processInput(GLFWwindow *window)
{
    float cameraSpeed = 10.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

//================== Framebuffer resize ==================
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//================= Cube generator with texture coordinates =================
void addCube(std::vector<float>& vertices,
             std::vector<unsigned int>& indices,
             glm::vec3 pos, float heightValue,
             unsigned int& indexOffset)
{
    // Cube vertices with texture coordinates
float cubeVerts[] = {
    // ===== S0: Front
    -0.5f,-0.5f,  0.5f,   0.0f,  0.25f,
     0.5f,-0.5f,  0.5f,   0.25f, 0.25f,
     0.5f, 0.5f,  0.5f,   0.25f, 0.0f,
    -0.5f, 0.5f,  0.5f,   0.0f,  0.0f,

    // ===== S1: Back
    -0.5f,-0.5f, -0.5f,   0.25f, 0.25f,
     0.5f,-0.5f, -0.5f,   0.5f,  0.25f,
     0.5f, 0.5f, -0.5f,   0.5f,  0.0f,
    -0.5f, 0.5f, -0.5f,   0.25f, 0.0f,

    // ===== S2: Left
    -0.5f,-0.5f, -0.5f,   0.5f,  0.25f,
    -0.5f,-0.5f,  0.5f,   0.75f, 0.25f,
    -0.5f, 0.5f,  0.5f,   0.75f, 0.0f,
    -0.5f, 0.5f, -0.5f,   0.5f,  0.0f,

    // ===== S3: Right
     0.5f,-0.5f, -0.5f,   0.75f, 0.25f,
     0.5f,-0.5f,  0.5f,   1.0f,  0.25f,
     0.5f, 0.5f,  0.5f,   1.0f,  0.0f,
     0.5f, 0.5f, -0.5f,   0.75f, 0.0f,

    // ===== T0: Top (unchanged)
    -0.5f, 0.5f, -0.5f,   0.0f,  0.25f,
     0.5f, 0.5f, -0.5f,   0.250f, 0.300f,
     0.5f, 0.5f,  0.5f,   0.250f, 0.500f,
    -0.5f, 0.5f,  0.5f,   0.0f,  0.5f,

    // ===== B0: Bottom (unchanged)
    -0.5f,-0.5f, -0.5f,   0.25f, 0.25f,
     0.5f,-0.5f, -0.5f,   0.5f,  0.25f,
     0.5f,-0.5f,  0.5f,   0.5f,  0.5f,
    -0.5f,-0.5f,  0.5f,   0.25f, 0.5f
};





    unsigned int cubeInd[] = {
        0,  1,  2,   2,  3,  0,   // front
        4,  5,  6,   6,  7,  4,   // back
        8,  9,  10,  10, 11, 8,   // left
        12, 13, 14,  14, 15, 12,  // right
        16, 17, 18,  18, 19, 16,  // top
        20, 21, 22,  22, 23, 20   // bottom
    };

    // Add vertices (position + height + texture coordinates)
    for (int i = 0; i < 24; i++) { // 24 vertices (4 per face * 6 faces)
        vertices.push_back(cubeVerts[i*5] + pos.x);     // x position
        vertices.push_back(cubeVerts[i*5+1] + pos.y);   // y position
        vertices.push_back(cubeVerts[i*5+2] + pos.z);   // z position
        vertices.push_back(heightValue);                 // height attribute
        vertices.push_back(cubeVerts[i*5+3]);           // u texture coordinate
        vertices.push_back(cubeVerts[i*5+4]);           // v texture coordinate
    }

    // Add indices
    for (int i = 0; i < 36; i++) {
        indices.push_back(cubeInd[i] + indexOffset);
    }

    indexOffset += 24; // 24 vertices per cube
}

// ================== Main ==================
int main()
{
    srand(time(0));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(1980, 1080, "Textured Terrain", monitor, NULL);
    
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Load textures (
    unsigned int grassTexture = loadTexture("textures/grass.png");
    unsigned int rockTexture = loadTexture("textures/stone.png");
    unsigned int snowTexture = loadTexture("textures/snow.png");
    unsigned int soilTexture = loadTexture("textures/dirt.png");

    // Build shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ======= Terrain storage =======
    std::vector<float> vertices;        // stores vertex positions + height + texture coords
    std::vector<unsigned int> indices;  // stores triangle indices
    unsigned int indexOffset = 0;       // tracks vertex count

    // ======= The Noise!!!!=======
    SimplexNoise noiseGenerator;  // default settings
    float scale1 = 0.01f;          // base terrain
    float scale2 = 0.02f;          // detail layer
    float maxHeight = 20.0f;       // max cube height

    // First pass: calculate heights for all positions
    const int terrainSize = 1000;
    std::vector<std::vector<int>> heightMap(terrainSize, std::vector<int>(terrainSize));
    
    for (int x = 0; x < terrainSize; x++) {
        for (int z = 0; z < terrainSize; z++) {
            int worldX = x - 500;
            int worldZ = z - 500;
            
            float n1 = noiseGenerator.noise(worldX * scale1, worldZ * scale1);
            float n2 = noiseGenerator.noise(worldX * scale2, worldZ * scale2) * 0.3f;
            float avgNoise = n1 + n2;
            avgNoise = glm::clamp(avgNoise, -1.0f, 1.0f);
            
            float h = ((avgNoise + 1.0f) / 2.0f) * maxHeight;
            int heightVal = static_cast<int>(std::round(h));
            if (heightVal < 1) heightVal = 1;
            
            heightMap[x][z] = heightVal;
        }
    }

    // Second pass: only add visible surface blocks
    for (int x = 0; x < terrainSize; x++) {
        for (int z = 0; z < terrainSize; z++) {
            int worldX = x - 500;
            int worldZ = z - 500;
            int height = heightMap[x][z];
            
            // Always add the top surface block
            addCube(vertices, indices, glm::vec3(worldX, height - 1, z - 500), (float)(height - 1), indexOffset);
            
            // Add blocks that have exposed sides
            for (int y = 0; y < height - 1; y++) {
                bool hasExposedFace = false;
                
                // Check if this block has any exposed faces by comparing with neighbors
                // Check left neighbor
                if (x == 0 || (x > 0 && heightMap[x-1][z] <= y)) hasExposedFace = true;
                // Check right neighbor  
                if (x == terrainSize-1 || (x < terrainSize-1 && heightMap[x+1][z] <= y)) hasExposedFace = true;
                // Check front neighbor
                if (z == 0 || (z > 0 && heightMap[x][z-1] <= y)) hasExposedFace = true;
                // Check back neighbor
                if (z == terrainSize-1 || (z < terrainSize-1 && heightMap[x][z+1] <= y)) hasExposedFace = true;
                
                if (hasExposedFace) {
                    addCube(vertices, indices, glm::vec3(worldX, y, z - 500), (float)y, indexOffset);
                }
            }
        }
    }

    // Setup VAO/VBO/EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex height (location 1)
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinates (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLint isOutlineLoc = glGetUniformLocation(shaderProgram, "isOutline");

    // Set texture uniforms
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "useTint"), true); //tint toggle (tint totally removed prevously)
    glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "rockTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "snowTexture"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "soilTexture"), 3);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, snowTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, soilTexture);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view  = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(75.0f),
                                                width / (float)height,
                                                0.1f, 500.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // Draw filled cubes
        glUniform1i(isOutlineLoc, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Reset polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}