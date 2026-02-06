#include "entity.h"

Entity::Entity() {
    mesh = NULL;
    model_matrix = NULL;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
	std::vector<Vector3> mesh_vert = mesh->GetVertices();
	int num_vert = mesh_vert.size();
    Matrix44 final_matrix = camera->GetViewProjectionMatrix() * (*model_matrix);
    for (int i = 0; i < num_vert; i++) {
        mesh_vert[i] = (*model_matrix) * mesh_vert[i];
		mesh_vert[i] = camera->ProjectVector(mesh_vert[i]);
    }
}
