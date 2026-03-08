#include "material.h"
#include "shader.h"
#include "texture.h"
#include "framework.h"

void Material::Enable() {
	shader->Enable();
}

void Material::Enable(const sUniformData& uniformData) {
    //shader->SetVector2("u_resolution", Vector2((float)window_width, (float)window_height));
    shader->SetFloat("u_aspect", (float)window_width / (float)window_height);
    shader->SetFloat("u_pi", PI);
    shader->SetTexture("u_texture", texture);
    shader->SetFloat("u_time", time);
	shader->SetFloat("u_amb_light_intensity", uniformData.amb_light_intensity);
	shader->SetMatrix44("u_model", uniformData.model_matrix);
	shader->SetMatrx44("u_viewprojection", uniformData.vp_matrix);
	// material properties
}

void Material::Disable() {
	shader->Disable();
}