#include <iostream>

#define GLEW_STATIC
#include<gl/glew.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<iostream>

#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma region camera declare
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

#pragma endregion

#pragma region Input declare

float deltaTime = 0.0f; // 當前幀與上一幀的時間差
float lastFrame = 0.0f; // 上一幀的時間
int cnt = 0;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_X && action == GLFW_PRESS) 
        cnt = (cnt + 1) % 16;
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

    float sensitivity = 0.5;
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
    printf("123");

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
    glfwSetKeyCallback(window, keyCallback);
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
    Shader* testShader = new Shader("./GLSL/vertexSource.vert", "./GLSL/fragmentSource.frag");
    #pragma endregion

    #pragma region Init Material
    Material* myMaterial = new Material(testShader, 
        LoadImageToGPU("./resource/container2.png", GL_RGBA, GL_RGBA),
        glm::vec3(1.0f, 1.0f, 1.0f), 64.0f);

    stbi_set_flip_vertically_on_load(true);
    #pragma endregion

    #pragma region Init and load models
    // load models
    Model ourModel("./model/model.dae");
    Animation danceAnimation("./model/model.dae", &ourModel);
    Animator animator(&danceAnimation);

    animator.UpdateAnimation(deltaTime, &(animator.GetCurrentAnimation()->GetRootNode()));
    #pragma endregion

    #pragma region prepare MVP matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    #pragma endregion


    while (!glfwWindowShouldClose(window))
    {
        // Compute FrameTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // process input
        processInput(window);

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) 
            animator.UpdateAnimation(deltaTime, &(animator.GetCurrentAnimation()->GetRootNode()));

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) 
            animator.UpdateAnimation(deltaTime, danceAnimation.joint.at(cnt));
        
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //set Material->textures	
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, myMaterial->diffuse);

        // Set material -> shader program
        testShader->use();

        // Set View matrix
        view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set projection matrix
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            testShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
            
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, -5.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        ourModel.Draw(testShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}