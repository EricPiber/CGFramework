#include "entity.h"

Entity::Entity() {
    mesh = NULL;
    model_matrix = NULL;
    texture = NULL;
    animationId = 0;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix, Image* texture, eRenderMode mode, int id) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
    this->texture = texture;
    this->mode = mode;
    this->animationId = id;
    shad = NULL;
    text = NULL;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix, Shader *shad, Texture *text) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
    this->shad = shad;
    this->text = text;
}

bool Entity::isInside(Vector3 v) {  // triangle inside the clip space if all its vertices are inside the cube defined by x,y,z in [-1, 1]
    return v.x >= -1.0f && v.x <= 1.0f &&
        v.y >= -1.0f && v.y <= 1.0f &&
        v.z >= -1.0f && v.z <= 1.0f;
}

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer) {
    if(framebuffer->wireT_trianF) {  // change to wireframe
        if(mode == eRenderMode::TRIANGLES || mode == eRenderMode::TRIANGLES_INTERPOLATED) {
            mode = eRenderMode::WIREFRAME;
        }
    } else {
        if(mode == eRenderMode::WIREFRAME) {
            mode = eRenderMode::TRIANGLES;  // change to triangles
        }
    }
	if (framebuffer->interpolUVsT_colorF) {  // change to interpolated triangles using textures
        if(mode == eRenderMode::TRIANGLES) {
            mode = eRenderMode::TRIANGLES_INTERPOLATED;
        }
    } else {
        if(mode == eRenderMode::TRIANGLES_INTERPOLATED) {  // change to plain colour triangles
            mode = eRenderMode::TRIANGLES;  // change to plain colour triangles
        }
    }
    
	std::vector<Vector3> mesh_vert = mesh->GetVertices();
    std::vector<Vector2> mesh_uv = mesh->GetUVs();
	unsigned long num_vert = mesh_vert.size();
	if (num_vert < 3) {  // not enough vertices to render a triangle
        return;
    }
	for (int i = 0; i + 2 < num_vert; i += 3) {  // for each vertex of a triangle
        // local to world
        mesh_vert[i] = *model_matrix * mesh_vert[i];
        mesh_vert[i+1] = *model_matrix * mesh_vert[i + 1];
        mesh_vert[i+2] = *model_matrix * mesh_vert[i + 2];

        // world to projection
		mesh_vert[i] = camera->ProjectVector(mesh_vert[i]);
        mesh_vert[i+1] = camera->ProjectVector(mesh_vert[i+1]);
        mesh_vert[i+2] = camera->ProjectVector(mesh_vert[i+2]);

		// clipping: if any of the vertices is outside the clip space, skip the triangle
        if (!isInside(mesh_vert[i]) || !isInside(mesh_vert[i+1]) || !isInside(mesh_vert[i+2]))
            continue;
        
        // filling struct
        sTriangleInfo triangle{mesh_vert[i], mesh_vert[i+1], mesh_vert[i+2], Color::RED, Color::GREEN, Color::BLUE, mesh_uv[i], mesh_uv[i+1], mesh_uv[i+2], texture};
        
        // rendering depending on entity's mode
        if(mode == eRenderMode::POINTCLOUD) {
            framebuffer->DrawPointcloud(triangle, zbuffer);
        } else if(mode == eRenderMode::WIREFRAME) {
            framebuffer->DrawWireframe(triangle, zbuffer);
        } else if(mode == eRenderMode::TRIANGLES) {
            framebuffer->DrawTriangles(triangle, zbuffer);
        } else if(mode == eRenderMode::TRIANGLES_INTERPOLATED) {
            framebuffer->DrawTriangleInterpolated(triangle, zbuffer);
        }
    }
}

void Entity::Update(float dt) {
	if (animationId == 1) {  // rotation animation
        Matrix44 mRot = Matrix44();
        mRot.MakeRotationMatrix(0.5f*DEG2RAD, Vector3(0, 1, 0));
        *model_matrix = *model_matrix * mRot;
	}
	else if (animationId == 2) {  // scaling animation
        animation_time += dt;
        Matrix44 mScale = Matrix44();
        float mod = fmod(animation_time, 10.0f);
        float scale;
		if (mod < 5.0f) {  // scale up for 5 seconds, then scale down for 5 seconds
            scale = 1.001f;
        } else {
            scale = 0.999001f;
        }
        
        mScale.MakeScaleMatrix(scale, scale, scale);
        
        *model_matrix = *model_matrix * mScale;
	}
	else if (animationId == 3) {  // translation animation
        animation_time += dt;
        float mod = fmod(animation_time, 5.0f);
		if ((1.25f < mod) && (mod < 3.75f)) {  // translate right and left for 2.5 seconds each around the center
            model_matrix->M[3][0] = model_matrix->M[3][0] + 0.002;
        } else {
            model_matrix->M[3][0] = model_matrix->M[3][0] - 0.002;
        }
	}
	else if (animationId == 4) {  // combined animation
        animation_time += dt;
        float mod1 = fmod(animation_time, 10.0f);
        float mod2 = fmod(animation_time, 5.0f);
        Matrix44 mScale = Matrix44();
        Matrix44 mRot = Matrix44();
        mRot.MakeRotationMatrix(0.5f*DEG2RAD, Vector3(1, 0, 0));
        float scale;
        if(mod1 < 5.0f) {
            scale = 1.001f;
        } else {
            scale = 0.999001f;
        }
        if((1.25f < mod2) && (mod2 < 3.75f)) {
            model_matrix->M[3][0] = model_matrix->M[3][0] + 0.002;
        } else {
            model_matrix->M[3][0] = model_matrix->M[3][0] - 0.002;
        }
        
        mScale.MakeScaleMatrix(scale, scale, scale);
        
        *model_matrix = *model_matrix * mRot * mScale;
    }
}

void Entity::Render(Camera* camera) {
    shad->Enable();
    shad->SetMatrix44("u_model", *model_matrix);
    shad->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);
    shad->SetTexture("u_texture", text);
    mesh->Render();
    shad->Disable();
}

void Entity::Render(sUniformData& uniformData) {
    uniformData.material->Enable();
    shad->SetMatrix44("u_model", uniformData.entity->model_matrix);
    shad->SetMatrix44("u_viewprojection", uniformData.camera->viewprojection_matrix);
    shad->SetTexture("u_texture", text);
    mesh->Render();
    uniformData.Material->Disable();
}