
#include "framework.h"
#include "mesh.h"
#include "image.h"

class Entity
{
public:
    Mesh *mesh;
    Matrix44 *model_matrix;
    
    Entity();
    Entity(Mesh *mesh, Matrix44 *model_matrix);

    bool Entity::isInside(Vector3 v);
    Vector2 Entity::Entity::GetScreenCoordinates(Vector3 v, int width, int height);
    void Render(Image* framebuffer, Camera* camera, const Color& c);
};
