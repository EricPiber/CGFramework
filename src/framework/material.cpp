#include "material.h"
#include "shader.h"
#include "texture.h"
#include "framework.h"

Material::Enable() {
	shader->Enable();
}

Material::Disable() {
	shader->Disable();
}