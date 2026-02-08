
#include "framework.h"
#include "mesh.h"
#include "image.h"

class Entity
{
public:
    Mesh *mesh;
    Matrix44 *model_matrix;
    int animationId;
    float animation_time = 0.0f;
    
    Entity();
    Entity(Mesh *mesh, Matrix44 *model_matrix, int id = 0);

    bool isInside(Vector3 v);
    Vector2 GetScreenCoordinates(Vector3 v, int width, int height);
    void Render(Image* framebuffer, Camera* camera, const Color& c);
    void Update(float dt);
};
