
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

    void Render(Image* framebuffer, Camera* camera, const Color& c);
};
