#include "shader.h"
#include "texture.h"
#include "framework.h"
#include "entity.h"
#include "camera.h"
#include "application.h"

struct sLight {
	Vector3 position;
	float intensity;
};

struct sUniformData {
    Matrix44 model_matrix;
	Matrix44 vp_matrix;
	float amb_light_intensity;
	Material material;
	// scene light, etc
};

class Material
{
public:
    Shader *shader;
    Texture *texture;
    Color *components;
	float shininess;  // the s in the power of the specular component

    // methods
    void Enable();
	void Enable(const sUniformData& uniformData);
	void Disable();
}
