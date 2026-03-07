
#include "framework.h"
#include "mesh.h"
#include "image.h"
#include "shader.h"
#include "texture.h"

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
    Shader* shad;
    Texture* text;
	float animation_time = 0.0f;  // total time since the start of the animation
    
    Entity();
    Entity(Mesh *mesh, Matrix44 *model_matrix, Image* texture, eRenderMode mode, int id = 0);
    Entity(Mesh *mesh, Matrix44 *model_matrix, Shader *shad, Texture *text);

    bool isInside(Vector3 v);
    void Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer);
    void Update(float dt);
    
    void Render(Camera* camera);
};
