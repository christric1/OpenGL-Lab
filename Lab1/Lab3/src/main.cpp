#include <iostream>

#define GLEW_STATIC
#include<gl/glew.h>
#include<GLFW/glfw3.h>
#include<vector>

#include "Shader.h"
#include "Material.h"
#include "LightDirectional.h"
#include "LightPoint.h"
#include "LightSpot.h"
#include "Mesh.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma region camera declare
Camera camera(glm::vec3(0, 0, 3.0f), 0.0f, -90.0f, glm::vec3(0, 1.0f, 0));
#pragma endregion

#pragma region Light declare
glm::vec3 LightPos[] = {
    glm::vec3(1.0f, 0, 0),
    glm::vec3(0, 1.0f, 0),
    glm::vec3(0, 0, 1.0f),
    glm::vec3(5.0, 0, 1.0f),
    glm::vec3(6.0f, 1.0f, -1.0f),
    glm::vec3(6.0f, 3.0f, 0),
};

LightDirectional lightD = LightDirectional(LightPos[0], glm::vec3(glm::radians(120.0f), 0, 0));

LightPoint lightP0 = LightPoint(LightPos[1], glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0), glm::vec3(1.0f, 0, 0));
LightPoint lightP1 = LightPoint(LightPos[2], glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0), glm::vec3(0, 1.0f, 0));
LightPoint lightP2 = LightPoint(LightPos[3], glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0), glm::vec3(0, 0, 1.0f));
LightPoint lightP3 = LightPoint(LightPos[4], glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));

LightSpot lightS = LightSpot(LightPos[5], glm::vec3(glm::radians(90.0f), glm::radians(0.0f), 0));
#pragma endregion

#pragma region Input declare
float lastX = 400, lastY = 300;
bool firstMouse = false;

float deltaTime = 0.0f; // 當前幀與上一幀的時間差
float lastFrame = 0.0f; // 上一幀的時間

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.CamaraPos += cameraSpeed * camera.CamaraForward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.CamaraPos -= cameraSpeed * camera.CamaraForward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.CamaraPos -= glm::normalize(glm::cross(camera.CamaraForward, camera.CamaraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.CamaraPos += glm::normalize(glm::cross(camera.CamaraForward, camera.CamaraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // std::cout << "xpos:" << xpos << "ypos:" << ypos << std::endl;

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float deltaX, deltaY;
    deltaX = xpos - lastX;
    deltaY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    deltaX *= sensitivity;
    deltaY *= sensitivity;

    camera.ProcessMouseMovement(deltaX, deltaY);
}
#pragma endregion

unsigned int LoadImageToGPU(const char* filename, GLint internalFormat, GLenum format) {
    unsigned int textBuffer;
    glGenTextures(1, &textBuffer);
    glBindTexture(GL_TEXTURE_2D, textBuffer);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        // 灌資料
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    return textBuffer;
}

int main(int argc, char* argv[]) {

    #pragma region open a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Init GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("Init GLEW failed");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glEnable(GL_DEPTH_TEST);
    #pragma endregion

    #pragma region Init shader program
    Shader* testShader = new Shader("GLSL/vertexSource.vert", "GLSL/fragmentSource.frag");
    Shader* lightShader = new Shader("GLSL/lightVertexSource.vert", "GLSL/lightFragmentSource.frag");
    #pragma endregion

    #pragma region Init Material
    Material* myMaterial = new Material(testShader,
        LoadImageToGPU("Resource/container2.png", GL_RGBA, GL_RGBA),
        LoadImageToGPU("Resource/container2_specular.png", GL_RGBA, GL_RGBA),
        glm::vec3(1.0f, 1.0f, 1.0f),
        64.0f);
    #pragma endregion

    #pragma region Init and load models to VAO, VBO
    Model model_(".\\Model\\nanosuit.obj");
    stbi_set_flip_vertically_on_load(true);
    #pragma endregion

    #pragma region prepare MVP matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    #pragma endregion

    while (!glfwWindowShouldClose(window))
    {
        // Compute FrameTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // process input
        processInput(window);
        
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set material -> shader program
        testShader->use();

        // Set View matrix
        view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set projection matrix
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // lightDirctional
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.pos"), lightD.position.x, lightD.position.y, lightD.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.color"), lightD.color.x, lightD.color.y, lightD.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.dirToLight"), lightD.dirction.x, lightD.dirction.y, lightD.dirction.z);

        // lightPoint
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.pos"), lightP0.position.x, lightP0.position.y, lightP0.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.color"), lightP0.color.x, lightP0.color.y, lightP0.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.dirToLight"), lightP0.dirction.x, lightP0.dirction.y, lightP0.dirction.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.constant"), lightP0.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.linear"), lightP0.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.quadratic"), lightP0.quadratic);

        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.pos"), lightP1.position.x, lightP1.position.y, lightP1.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.color"), lightP1.color.x, lightP1.color.y, lightP1.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.dirToLight"), lightP1.dirction.x, lightP1.dirction.y, lightP1.dirction.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.constant"), lightP1.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.linear"), lightP1.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.quadratic"), lightP1.quadratic);

        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.pos"), lightP2.position.x, lightP2.position.y, lightP2.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.color"), lightP2.color.x, lightP2.color.y, lightP2.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.dirToLight"), lightP2.dirction.x, lightP2.dirction.y, lightP2.dirction.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.constant"), lightP2.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.linear"), lightP2.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.quadratic"), lightP2.quadratic);

        glUniform3f(glGetUniformLocation(testShader->ID, "lightP3.pos"), lightP3.position.x, lightP3.position.y, lightP3.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP3.color"), lightP3.color.x, lightP3.color.y, lightP3.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP3.dirToLight"), lightP3.dirction.x, lightP3.dirction.y, lightP3.dirction.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP3.constant"), lightP3.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP3.linear"), lightP3.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP3.quadratic"), lightP3.quadratic);

        // lightSpot
        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.pos"), lightS.position.x, lightS.position.y, lightS.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.color"), lightS.color.x, lightS.color.y, lightS.color.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.dirToLight"), lightS.dirction.x, lightS.dirction.y, lightS.dirction.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.constant"), lightS.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.linear"), lightS.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.quadratic"), lightS.quadratic);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.cosPhyInner"), lightS.cosPhyInner);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.cosPhyOutter"), lightS.cosPhyOutter);

        glUniform3f(glGetUniformLocation(testShader->ID, "cameraPos"), camera.CamaraPos.x, camera.CamaraPos.y, camera.CamaraPos.z);

        myMaterial->shader->SetUniform3f("material.ambient", myMaterial->ambient);
        myMaterial->shader->SetUniform1f("material.shininess", myMaterial->shininess);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        model_.Draw(myMaterial->shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}