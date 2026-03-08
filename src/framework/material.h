
#pragma once

#include "shader.h"
#include "texture.h"
#include "framework.h"
#include "camera.h"

class Material;

struct sLight {
	Vector3 position;
	float intensity;
};

struct sUniformData {
    Matrix44 *model_matrix;
	Matrix44 vp_matrix;
	float amb_light_intensity;
	// scene light, etc
};

class Material
{
public:
    Shader *shader;
    Texture *texture;
    Vector3 *ka;
    Vector3 *kd;
    Vector3 *ks;
	float shininess;  // the s in the power of the specular component

    Material();
    Material(Shader *shader, Texture *texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess);
    
    void Enable();
	void Enable(const sUniformData& uniformData);
	void Disable();
};
