
#pragma once

#include "shader.h"
#include "texture.h"
#include "framework.h"
#include "camera.h"

class Material;

struct sLight {
	Vector3 position;
	Vector3 intensity;
};

struct sUniformData {
    // entity
    Matrix44 *model_matrix;
    
    // camera
	Matrix44 vp_matrix;
    Vector3 cam_eye;
    
    // lights
	Vector3 Ia;
    int nLights;
    sLight lights[6];
    int index;
    
    // interactivity
    bool ct;
    bool st;
    bool nt;
    bool accumulateT_selectF;   // EXTRA
};

class Material
{
public:
    Shader *shader;
    Texture *texture;
    Texture *normal_texture;
    Vector3 *ka;
    Vector3 *kd;
    Vector3 *ks;
	float shininess;  // the s in the power of the specular component

    Material();
    Material(Shader *shader, Texture *texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess);
    Material(Shader *shader, Texture *texture, Texture *normal_texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess);
    
    void Enable();
	void Enable(const sUniformData& uniformData);
	void Disable();
};
