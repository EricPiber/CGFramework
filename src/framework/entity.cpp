#include "entity.h"

Entity::Entity() {
    mesh = NULL;
    model_matrix = NULL;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
}

bool Entity::isInside(Vector3 v) {
    return v.x >= -1.0f && v.x <= 1.0f &&
        v.y >= -1.0f && v.y <= 1.0f &&
        v.z >= -1.0f && v.z <= 1.0f;
}

Vector2 Entity::Entity::GetScreenCoordinates(Vector3 v, int width, int height) {
    // Convert from clip space (-1 to 1) to screen space (0 to width/height)
    float x = (v.x * 0.5f + 0.5f) * width;
	float y = (1.0f - (v.y * 0.5f + 0.5f)) * height;
    return Vector2(x, y);
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
	std::vector<Vector3> mesh_vert = mesh->GetVertices();
	int num_vert = mesh_vert.size();
	if (num_vert < 3) {  // not enough vertices to render a triangle
        return;
    }
	for (int i = 0; i + 2 < num_vert; i += 3) {  // for each vertex of a triangle
        // local to world
        mesh_vert[i] = *model_matrix * mesh_vert[i];
        mesh_vert[i+1] = *model_matrix * mesh_vert[i + 1];
        mesh_vert[i+2] = *model_matrix * mesh_vert[i + 2];

        // world to projection
		mesh_vert[i] = camera->ProjectVector(mesh_vert[i]);
        mesh_vert[i+1] = camera->ProjectVector(mesh_vert[i+1]);
        mesh_vert[i+2] = camera->ProjectVector(mesh_vert[i+2]);

		// clipping: if any of the vertices is outside the clip space, skip the triangle
        if (!isInside(mesh_vert[i]) || !isInside(mesh_vert[i+1]) || !isInside(mesh_vert[i+2]))
            continue;

		// viewport transformation: from clip space to screen space
		int w = framebuffer->width, h = framebuffer->height;
		Vector2 p1 = GetScreenCoordinates(mesh_vert[i], w, h);
        Vector2 p2 = GetScreenCoordinates(mesh_vert[i+1], w, h);
        Vector2 p3 = GetScreenCoordinates(mesh_vert[i+2], w, h);

        // drawing lines of triangle
        framebuffer->DrawLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, c);
        framebuffer->DrawLineDDA((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, c);
        framebuffer->DrawLineDDA((int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y, c);
    }
}
