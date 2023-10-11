#pragma once

// GLM
#include <glm/glm.hpp>

// Project
#include "common/staticMesh3D.h"

namespace static_meshes_3D {

    /**
     * Pyramid static mesh of unit size.
     */
    class Pyramid : public StaticMesh3D
    {
    public:
        Pyramid(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        void render() const override;
        void renderPoints() const override;

        static glm::vec3 vertices[12]; // Array of mesh vertices
        static glm::vec2 textureCoordinates[6]; // Array of mesh texture coordinates

        /**
        * Gets Pyramid color.
        */
        glm::vec4 getColor() const;

    private:
        void initializeData() override;
        glm::vec4 _color;
    };

} // namespace static_meshes_3D