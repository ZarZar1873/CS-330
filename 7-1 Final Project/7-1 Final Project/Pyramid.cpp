/*
* Code base copied from plane files in this project
*/

// GLM
#include <glm/glm.hpp>

// Project
#include "Pyramid.h"

namespace static_meshes_3D {

    glm::vec3 Pyramid::vertices[12] =
    {
        // Side 1 (1, 2, 3)
        glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(-0.5f, -0.5f, 0.0f),
        // Side 2 (1, 4, 3)
        glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(-0.5f, -0.5f, 0.0f),
        // Side 3 (1, 2, 4)
        glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, -0.5f, -1.0f),
        // Bottom face (2, 3, 4)
        glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, -0.5f, -1.0f),
    };

    // complex texturing technique: texture tiling for more details on the ground
    // using 5.0f as max texture coordinate to repeat the image 5 times
    glm::vec2 Pyramid::textureCoordinates[6] =
    {
        glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)
    };

    Pyramid::Pyramid(glm::vec4 color)
        : StaticMesh3D(true, true, true),
        _color(color)
    {
        initializeData();
    }


    glm::vec4 Pyramid::getColor() const
    {
        return _color;
    }

    void Pyramid::render() const
    {
        if (!_isInitialized) {
            return;
        }

        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    void Pyramid::renderPoints() const
    {
        if (!_isInitialized) {
            return;
        }

        glBindVertexArray(_vao);
        glDrawArrays(GL_POINTS, 0, 12);
    }

    void Pyramid::initializeData()
    {
        if (_isInitialized) {
            return;
        }

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        const auto numVertices = 12;
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