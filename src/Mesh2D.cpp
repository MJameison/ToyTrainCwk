#include "Mesh2D.h"

#include <iostream>

Mesh2D::Mesh2D(std::vector<Vertex2D> vertices, std::vector<unsigned int> lines) {
    this->vertices = vertices;
    this->lines = lines;
}

unsigned int Mesh2D::AddVertex(Vertex2D v) {
    vertices.push_back(v);
    return vertices.size() - 1;
}

void Mesh2D::AddLine(unsigned int v1, unsigned int v2) {
    lines.push_back(v1);
    lines.push_back(v2);
}

Mesh *Mesh2D::Extrude(std::vector<OrientedPoint> path, float distances[]) {
    if (vertices.size() < 1) {
        return nullptr;
    }

    Mesh *mesh = new Mesh();

    // Calculate u-span assuming its equal to total length of lines
    // The assumption is the horizontal texture is perfectly sized i.e. TexCoord in [0..1]
    float uSpan = 0.0f;
    for (unsigned int i = 0; i < lines.size(); i += 2) {
        glm::vec2 a = vertices[lines[i  ]].Position;
        glm::vec2 b = vertices[lines[i+1]].Position;
        uSpan += glm::distance(a, b);
    }

    for (unsigned int i = 0; i < path.size(); ++i) {
        OrientedPoint point = path[i];

        // Calculate v texcoord
        float vCoord = distances[i] / uSpan;
        
        // Add vertices
        for (unsigned int j = 0; j < vertices.size(); ++j) {
            Vertex2D extrudeVertex = vertices[j];

            Vertex vertex;
            vertex.Position = point.position + point.orientation * glm::vec3(extrudeVertex.Position, 0.0f);
            vertex.Normal = glm::normalize(point.orientation * glm::vec3(extrudeVertex.Normal, 0.0f));

            vertex.TexCoords = glm::vec2(extrudeVertex.TexPos, vCoord);

            mesh->AddVertex(vertex);
        }

        // Add triangles
        if (i == 0)
            continue;
        
        unsigned int prevRowBase = (i - 1) * vertices.size();
        unsigned int currRowBase = i * vertices.size();

        for (unsigned int l = 0; l < lines.size(); l += 2) {
            unsigned int a = currRowBase + lines[l];
            unsigned int b = prevRowBase + lines[l];
            unsigned int c = prevRowBase + lines[l+1];
            unsigned int d = currRowBase + lines[l+1];

            mesh->AddTriangle(a, b, d);
            mesh->AddTriangle(b, c, d);
        }
    }

    mesh->UpdateMesh();

    return mesh;
}
