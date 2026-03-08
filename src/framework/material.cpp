#include "material.h"

Material::Material() {
    shader = NULL;
    texture = NULL;
    ka = NULL;
    kd = NULL;
    ks = NULL;
    shininess = 0;
}

Material::Material(Shader *shader, Texture *texture, Vector3 *ka, Vector3 *kd, Vector3 *ks, float shininess) {
    this->shader = shader;
    this->texture = texture;
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
    shader->SetMatrix44("u_model", *uniformData.model_matrix);
    shader->SetMatrix44("u_viewprojection", uniformData.vp_matrix);
    shader->SetTexture("u_texture", texture);
}

void Material::Disable() {
	shader->Disable();
}
