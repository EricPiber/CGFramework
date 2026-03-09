#include "material.h"

Material::Material() {
    shader = NULL;
    texture = NULL;
    normal_texture = NULL;
    ka = NULL;
    kd = NULL;
    ks = NULL;
    shininess = 0;
}

Material::Material(Shader *shader, Texture *texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess) {
    this->shader = shader;
    this->texture = texture;
    normal_texture = NULL;
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;
    this->shininess = shininess;
}

Material::Material(Shader *shader, Texture *texture, Texture *normal_texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess) {
    this->shader = shader;
    this->texture = texture;
    this->normal_texture = normal_texture;
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;
    this->shininess = shininess;
}

void Material::Enable() {
	shader->Enable();
}

void Material::Enable(const sUniformData& uniformData) {
    shader->Enable();
    // entity
    shader->SetMatrix44("u_model", *uniformData.model_matrix);
    
    // camera
    shader->SetMatrix44("u_viewprojection", uniformData.vp_matrix);
    shader->SetVector3("u_cam_eye", uniformData.cam_eye);
    
    // material
    shader->SetTexture("u_texture", texture);
    if(normal_texture != NULL) {shader->SetTexture("u_normal_texture", normal_texture);}
    shader->SetVector3("u_ka", *ka);
    shader->SetVector3("u_kd", *kd);
    shader->SetVector3("u_ks", *ks);
    shader->SetFloat("u_shininess", shininess);
    
    // light
    if(uniformData.index == 0) {shader->SetVector3("u_Ia", uniformData.Ia);}
    else {shader->SetVector3("u_Ia", Vector3(0.0f));}
    shader->SetVector3("u_m", uniformData.lights[uniformData.index].position);
    shader->SetVector3("u_I", uniformData.lights[uniformData.index].intensity);
    
    // interactivity
    shader->SetInt("u_ct", (int)uniformData.ct);
    shader->SetInt("u_st", (int)uniformData.st);
    shader->SetInt("u_nt", (int)uniformData.nt);
}

void Material::Disable() {
	shader->Disable();
}
