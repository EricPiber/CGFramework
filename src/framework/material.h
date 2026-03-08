#include "shader.h"
#include "texture.h"
#include "framework.h"

struct sLight {
	Vector3 position;
	float intensity;
};

class Material
{
public:
    Shader *shader;
    Texture *texture;
    Color *components;
	float shininess;  // the s in the power of the specular component

    // methods
    Enable();
	Disable();
}
