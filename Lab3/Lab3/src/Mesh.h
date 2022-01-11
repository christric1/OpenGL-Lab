#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

#define GLEW_STATIC
#include <gl/glew.h>
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};


struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};


class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(float vertices[]);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader* shader);

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

