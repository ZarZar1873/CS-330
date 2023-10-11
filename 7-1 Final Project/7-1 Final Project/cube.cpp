/*
* Code base copied from plane files in this project, edits assisted by cube files found in torus example
*/

// GLM
#include <glm/glm.hpp>

// Project
#include "Cube.h"

namespace static_meshes_3D {

    glm::vec3 Cube::vertices[36] =
    {
        // Front face
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f),
        // Back face
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f),
        // Left face
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
        // Right face
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f),
        // Top face
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
        // Bottom face
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f),
    };

    // complex texturing technique: texture tiling for more details on the ground
    // using 5.0f as max texture coordinate to repeat the image 5 times
    glm::vec2 Cube::textureCoordinates[6] =
    {
        glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)
    };

    Cube::Cube(glm::vec4 color)
        : StaticMesh3D(true, true, true),
        _color(color)
    {
        initializeData();
    }


    glm::vec4 Cube::getColor() const
    {
        return _color;
    }

    void Cube::render() const
    {
        if (!_isInitialized) {
            return;
        }

        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void Cube::renderPoints() const
    {
        if (!_isInitialized) {
            return;
        }

        glBindVertexArray(_vao);
        glDrawArrays(GL_POINTS, 0, 36);
    }

    void Cube::initializeData()
    {
        if (_isInitialized) {
            return;
        }

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        const auto numVertices = 36;
        const auto vertexByteSize = getVertexByteSize();
        _vbo.createVBO(vertexByteSize * numVertices);
        _vbo.bindVBO();

        if (hasPositions())
        {
            _vbo.addRawData(vertices, sizeof(glm::vec3) * numVertices);
        }

        if (hasTextureCoordinates())
        {
            _vbo.addRawData(textureCoordinates, sizeof(glm::vec2) * numVertices);
        }


        // Add color for every vertex of cylinder bottom cover
        _vbo.addData(_color, numVertices);


        _vbo.uploadDataToGPU(GL_STATIC_DRAW);
        setVertexAttributesPointers(numVertices);
        _isInitialized = true;
    }

} // namespace static_meshes_3D