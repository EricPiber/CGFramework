#include "entity.h"

Entity::Entity() {
    mesh = NULL;
    model_matrix = NULL;
    animationId = 0;
}

Entity::Entity(Mesh *mesh, Matrix44 *model_matrix, int id) {
    this->mesh = mesh;
    this->model_matrix = model_matrix;
    this->animationId = id;
}

bool Entity::isInside(Vector3 v) {
    return v.x >= -1.0f && v.x <= 1.0f &&
        v.y >= -1.0f && v.y <= 1.0f &&
        v.z >= -1.0f && v.z <= 1.0f;
}

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer) {
	std::vector<Vector3> mesh_vert = mesh->GetVertices();
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
        
        // from projection to screen, using depth
        framebuffer->DrawTriangleInterpolated(mesh_vert[i], mesh_vert[i+1], mesh_vert[i+2], Color::RED, Color::GREEN, Color::BLUE, zbuffer);
        
        /*
		// viewport transformation: from clip space to screen space
		Vector2 p1 = framebuffer->GetScreenCoordinates(mesh_vert[i]);
        Vector2 p2 = framebuffer->GetScreenCoordinates(mesh_vert[i+1]);
        Vector2 p3 = framebuffer->GetScreenCoordinates(mesh_vert[i+2]);

        // drawing triangles
        framebuffer->DrawTriangle(p1, p2, p3, c, true, c);
         
        // not filled
        framebuffer->DrawLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, c);
        framebuffer->DrawLineDDA((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, c);
        framebuffer->DrawLineDDA((int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y, c);
        
        
        */
    }
}

void Entity::Update(float dt) {
    if (animationId == 1) {
        Matrix44 mRot = Matrix44();
        mRot.MakeRotationMatrix(0.5f*DEG2RAD, Vector3(0, 1, 0));
        *model_matrix = *model_matrix * mRot;
    } else if (animationId == 2) {
        animation_time += dt;
        Matrix44 mScale = Matrix44();
        float mod = fmod(animation_time, 10.0f);
        float scale;
        if(mod < 5.0f) {
            scale = 1.001f;
        } else {
            scale = 0.999001f;
        }
        
        mScale.MakeScaleMatrix(scale, scale, scale);
        
        *model_matrix = *model_matrix * mScale;
    } else if (animationId == 3) {
        animation_time += dt;
        float mod = fmod(animation_time, 5.0f);
        if((1.25f < mod) && (mod < 3.75f)) {
            model_matrix->M[3][0] = model_matrix->M[3][0] + 0.002;
        } else {
            model_matrix->M[3][0] = model_matrix->M[3][0] - 0.002;
        }
    } else if (animationId == 4) {
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
