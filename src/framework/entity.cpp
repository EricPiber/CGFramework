
#include "entity.h"

Entity::Entity() {
    mesh = NULL;
    model_matrix = NULL;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
}
