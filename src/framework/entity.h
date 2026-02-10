
#include "framework.h"
#include "mesh.h"
#include "image.h"

enum class eRenderMode {
    POINTCLOUD,
    WIREFRAME,
    TRIANGLES,
    TRIANGLES_INTERPOLATED
};

class Entity
{
public:
    Mesh *mesh;
    Matrix44 *model_matrix;
    int animationId;
    Image* texture;
    eRenderMode mode;
    float animation_time = 0.0f;
    
    Entity();
    Entity(Mesh *mesh, Matrix44 *model_matrix, Image* texture, eRenderMode mode, int id = 0);

    bool isInside(Vector3 v);
    void Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer);
    void Update(float dt);
};
