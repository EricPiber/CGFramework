#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
    
    if (lab == 1) {
        // Loading images before running the app, save the result of load in an array to be sure everything was loaded
        bool correctLoad[16];
        correctLoad[0] = clear.LoadPNG("images/clear.png");
        correctLoad[1] = load.LoadPNG("images/load.png");
        correctLoad[2] = save.LoadPNG("images/save.png");
        correctLoad[3] = pencil.LoadPNG("images/pencil.png");
        correctLoad[4] = eraser.LoadPNG("images/eraser.png");
        correctLoad[5] = line.LoadPNG("images/line.png");
        correctLoad[6] = rectangle.LoadPNG("images/rectangle.png");
        correctLoad[7] = triangle.LoadPNG("images/triangle.png");
        correctLoad[8] = black.LoadPNG("images/black.png");
        correctLoad[9] = white.LoadPNG("images/white.png");
        correctLoad[10] = red.LoadPNG("images/red.png");
        correctLoad[11] = green.LoadPNG("images/green.png");
        correctLoad[12] = blue.LoadPNG("images/blue.png");
        correctLoad[13] = yellow.LoadPNG("images/yellow.png");
        correctLoad[14] = cyan.LoadPNG("images/cyan.png");
        correctLoad[15] = pink.LoadPNG("images/pink.png");
        for(int i=0; i<16; i++) {
            if(correctLoad[i] == false) {
                std::cout << "Some Image was not found!" << std::endl;
            }
        }
        // creation of buttons of menu bar
        butClear = Button(clear, 10, 10, CLEAR);
        butLoad = Button(load, 50, 10, LOAD);
        butSave = Button(save, 90, 10, SAVE);
        butPencil = Button(pencil, 130, 10, PENCIL);
        butEraser = Button(eraser, 170, 10, ERASER);
        butLine = Button(line, 210, 10, LINE);
        butRectangle = Button(rectangle, 250, 10, RECTANGLE);
        butTriangle = Button(triangle, 290, 10, TRIANGLE);
        butBlack = Button(black, 330, 10, BLACK);
        butWhite = Button(white, 370, 10, WHITE);
        butRed = Button(red, 410, 10, RED);
        butGreen = Button(green, 450, 10, GREEN);
        butBlue = Button(blue, 490, 10, BLUE);
        butYellow = Button(yellow, 530, 10, YELLOW);
        butCyan = Button(cyan, 570, 10, CYAN);
        butPink = Button(pink, 610, 10, PINK);
    } else if (lab == 2) {
        
        Mesh *mesh1 = new Mesh();
        mesh1->LoadOBJ("meshes/lee.obj");
        Image *texture1 = new Image();
        texture1->LoadTGA("textures/lee_color_specular.tga", true);
        Mesh* mesh2 = new Mesh();
        mesh2->LoadOBJ("meshes/anna.obj");
        Image *texture2 = new Image();
        texture2->LoadTGA("textures/anna_color_specular.tga", true);
        Mesh* mesh3 = new Mesh();
        mesh3->LoadOBJ("meshes/cleo.obj");
        Image *texture3 = new Image();
        texture3->LoadTGA("textures/cleo_color_specular.tga", true);

        model_matrix0 = new Matrix44();
        model_matrix1 = new Matrix44();
        model_matrix2 = new Matrix44();
        model_matrix3 = new Matrix44();
        model_matrix4 = new Matrix44();

        Matrix44 mTrans0 = Matrix44();
        Matrix44 mTrans1 = Matrix44();
        Matrix44 mTrans2 = Matrix44();
        Matrix44 mTrans3 = Matrix44();
        Matrix44 mTrans4 = Matrix44();
        
		mTrans0.MakeTranslationMatrix(0, -0.2, 0);
        mTrans1.MakeTranslationMatrix(0.7, 0.3, -1);
        mTrans2.MakeTranslationMatrix(-0.7, 0.3, -1);
        mTrans3.MakeTranslationMatrix(0, -1, -0.5);
        mTrans4.MakeTranslationMatrix(0, 0.2, 0);

        
        Matrix44 mScale = Matrix44();
        Matrix44 mScale0 = Matrix44();
        mScale.MakeScaleMatrix(1.0f, 1.0f, 1.0f);
        mScale0.MakeScaleMatrix(2.0f, 2.0f, 2.0f);
        
		*model_matrix0 = mTrans0 * mScale0;
        *model_matrix1 = mTrans1 * mScale;
        *model_matrix2 = mTrans2 * mScale;
        *model_matrix3 = mTrans3 * mScale;
        *model_matrix4 = mTrans4 * mScale;
        
		entity0 = new Entity(mesh2, model_matrix0, texture2, eRenderMode::TRIANGLES_INTERPOLATED, 0);
        entity1 = new Entity(mesh3, model_matrix1, texture3, eRenderMode::POINTCLOUD, 1);
        entity2 = new Entity(mesh2, model_matrix2, texture2, eRenderMode::WIREFRAME, 2);
        entity3 = new Entity(mesh3, model_matrix3, texture3, eRenderMode::TRIANGLES, 3);
        entity4 = new Entity(mesh1, model_matrix4, texture1, eRenderMode::TRIANGLES_INTERPOLATED, 4);
        
        camera = new Camera();
        if(camPerspective) {
            // for perspective projection:
            camera->SetPerspective(60.0f, (float)window_width / (float)window_height, 0.5f, 3.0f);
            camera->LookAt(Vector3(0.0f, 0.5f, 1.5f), Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        }
        else {
            // for orthographic projection:
            camera->SetOrthographic(-1.0f, 1.0f, 1.0f, -1.0f, -10.0f, 10.0f);
            camera->LookAt(Vector3(0.0f, 0.5f, 1.5f), Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        }
        
        zbuffer = new FloatImage(framebuffer.width, framebuffer.height);
    }
    framebuffer.Fill(Color::BLACK);
    
}

// Render one frame
void Application::Render(void)
{

    if (lab == 1) {
        // if animation is called, render starfield
        if (starfield_initialized) {
            starfield.Render(&framebuffer);
        }
        
        // draw menu bar and buttons
        framebuffer.DrawRect(0, 0, framebuffer.width, 50, Color::GRAY, 1, true, Color::GRAY);
        butClear.DrawButton(framebuffer);
        butLoad.DrawButton(framebuffer);
        butSave.DrawButton(framebuffer);
        butPencil.DrawButton(framebuffer);
        butEraser.DrawButton(framebuffer);
        butLine.DrawButton(framebuffer);
        butRectangle.DrawButton(framebuffer);
        butTriangle.DrawButton(framebuffer);
        butBlack.DrawButton(framebuffer);
        butWhite.DrawButton(framebuffer);
        butRed.DrawButton(framebuffer);
        butGreen.DrawButton(framebuffer);
        butBlue.DrawButton(framebuffer);
        butYellow.DrawButton(framebuffer);
        butCyan.DrawButton(framebuffer);
        butPink.DrawButton(framebuffer);
    } else if (lab == 2) {
        zbuffer->Fill(10000.0f);
        if (entities_initialized) {
            makeAction(CLEAR);
            entity1->Render(&framebuffer, camera, zbuffer);
            entity2->Render(&framebuffer, camera, zbuffer);
            entity3->Render(&framebuffer, camera, zbuffer);
            entity4->Render(&framebuffer, camera, zbuffer);
        } else {
            makeAction(CLEAR);
            entity0->Render(&framebuffer, camera, zbuffer);
        }
    }
    framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    if (lab == 1) {
        // if animation is called, update starfield
        if (starfield_initialized)
            starfield.Update(seconds_elapsed);
    } else if (lab == 2) {
        if (entities_initialized) {
            entity1->Update(seconds_elapsed);
            entity2->Update(seconds_elapsed);
            entity3->Update(seconds_elapsed);
            entity4->Update(seconds_elapsed);
        }
    }
}

void Application::makeAction(Action action) {
    switch (action) {
    case CLEAR:
        framebuffer.Fill(Color::BLACK);
        framebuffer.SaveTGA("clear.tga");
        loadTGA = true;
        framebuffer.LoadTGA("clear.tga");
        break;
    case LOAD:
        loadTGA = true;
        framebuffer.LoadTGA("output.tga", true);
        break;
    case SAVE:
        framebuffer.SaveTGA("output.tga");
        break;
    case PENCIL:
        currTool = PENCIL;
        break;
    case ERASER:
        currTool = ERASER;
        break;
    case LINE:
        currTool = LINE;
        break;
    case RECTANGLE:
        currTool = RECTANGLE;
        break;
    case TRIANGLE:
        currTool = TRIANGLE;
        break;
    case BLACK:
        framebuffer.defColor = Color::BLACK;
        break;
    case WHITE:
        framebuffer.defColor = Color::WHITE;
        break;
    case RED:
        framebuffer.defColor = Color::RED;
        break;
    case GREEN:
        framebuffer.defColor = Color::GREEN;
        break;
    case BLUE:
        framebuffer.defColor = Color::BLUE;
        break;
    case YELLOW:
        framebuffer.defColor = Color::YELLOW;
        break;
    case CYAN:
        framebuffer.defColor = Color::CYAN;
        break;
    case PINK:
        framebuffer.defColor = Color::PURPLE;
        break;
    }
}

void Application::makeAnimation() {
	// save the last paint framebuffer
    if(!starfield_initialized) {
        framebuffer.SaveTGA("paint.tga");
    }
	// initialize starfield animation
    starfield.Init(framebuffer.width, framebuffer.height);
	makeAction(CLEAR);
    starfield_initialized = true;
}

void Application::paint() {
	// load last painted framebuffer
    makeAction(CLEAR);
	starfield_initialized = false;
	loadTGA = true;
    framebuffer.LoadTGA("paint.tga", true);
}

void Application::changeCameraProp(float d) {
    if (camProp == CAM_NEAR) {
        camera->near_plane = fmin(camera->far_plane - 0.1f, camera->near_plane + d);
    } else if (camProp == CAM_FAR) {
        camera->far_plane = fmax(camera->near_plane + 0.1f, camera->far_plane + d);
    } else if (camProp == CAM_FOV) {
        camera->fov = clamp(camera->fov + (d*50), 5.0f, 170.0f);  // limit FOV to interval [5, 170] to avoid incorrect/weird projections
    }
    
    camera->UpdateProjectionMatrix();
	camera->UpdateViewProjectionMatrix();
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
    if (lab == 1) {
        switch(event.keysym.sym) {
            case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
            case SDLK_PLUS: framebuffer.defBorderWidth++; break; // increase border width
            case SDLK_MINUS: if(framebuffer.defBorderWidth > 0) {framebuffer.defBorderWidth--;} break; // decrease border width
            case SDLK_1: paint(); break; // go to paint mode
            case SDLK_2: makeAnimation(); break; // go to animation mode
            case SDLK_f: framebuffer.isFilled = !framebuffer.isFilled; break; // toggle fill mode
            default: break;
        }
    } else if (lab == 2) {
        switch(event.keysym.sym) {
            case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
            case SDLK_PLUS: changeCameraProp(0.1); break;
            case SDLK_MINUS: changeCameraProp(-0.1); break;
            case SDLK_1: makeAction(CLEAR); entities_initialized = false; break;
            case SDLK_2: makeAction(CLEAR); entities_initialized = !entities_initialized; break;
            case SDLK_f: camProp = CAM_FAR; break;
			case SDLK_n: camProp = CAM_NEAR; break;
			case SDLK_v: camProp = CAM_FOV; break;
            case SDLK_t: framebuffer.meshT_colorF = !framebuffer.meshT_colorF; break;
            case SDLK_z: framebuffer.occlusions = !framebuffer.occlusions; break;
            case SDLK_c: framebuffer.interpolUVsT_colorF = !framebuffer.interpolUVsT_colorF; break;
            case SDLK_w: framebuffer.wireT_trianF = !framebuffer.wireT_trianF; break;
            default: break;
        }
    }
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
    if (lab == 1) {
        if (event.button == SDL_BUTTON_LEFT) {
            if (butClear.IsMouseInside(mouse_position)) {
                makeAction(CLEAR);
            }
            else if (butLoad.IsMouseInside(mouse_position)) {
                makeAction(LOAD);
            }
            else if (butSave.IsMouseInside(mouse_position)) {
                makeAction(SAVE);
            }
            else if (butPencil.IsMouseInside(mouse_position)) {
                makeAction(PENCIL);
            }
            else if (butEraser.IsMouseInside(mouse_position)) {
                makeAction(ERASER);
            }
            else if (butLine.IsMouseInside(mouse_position)) {
                makeAction(LINE);
            }
            else if (butRectangle.IsMouseInside(mouse_position)) {
                makeAction(RECTANGLE);
            }
            else if (butTriangle.IsMouseInside(mouse_position)) {
                makeAction(TRIANGLE);
            }
            else if (butBlack.IsMouseInside(mouse_position)) {
                makeAction(BLACK);
            }
            else if (butWhite.IsMouseInside(mouse_position)) {
                makeAction(WHITE);
            }
            else if (butRed.IsMouseInside(mouse_position)) {
                makeAction(RED);
            }
            else if (butGreen.IsMouseInside(mouse_position)) {
                makeAction(GREEN);
            }
            else if (butBlue.IsMouseInside(mouse_position)) {
                makeAction(BLUE);
            }
            else if (butYellow.IsMouseInside(mouse_position)) {
                makeAction(YELLOW);
            }
            else if (butCyan.IsMouseInside(mouse_position)) {
                makeAction(CYAN);
            }
            else if (butPink.IsMouseInside(mouse_position)) {
                makeAction(PINK);
            }
            if (currTool == LINE) {
                *orig_mouse = mouse_position; // save first point
                framebuffer.SaveTGA("temp.tga");
            }
            else if (currTool == RECTANGLE) {
                *orig_mouse = mouse_position;
                framebuffer.SaveTGA("temp.tga");
            }
            else if (currTool == TRIANGLE) {
                if (mouse_position.y > 50) {
                    switch (triCounter) {
                    case 0:
                        *p0 = mouse_position;
                        framebuffer.SaveTGA("temp.tga");
                        triCounter = 1; // number of points clicked
                        break;
                    case 1:
                        *p1 = mouse_position;
                        triCounter = 2;
                        break;
                    case 2:
                        loadTGA = true;
                        framebuffer.LoadTGA("temp.tga", true);
                        *p2 = mouse_position;
                        framebuffer.DrawTriangle(*p0, *p1, *p2, framebuffer.defColor, framebuffer.isFilled, framebuffer.defColor);
                        triCounter = 0;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
    if (lab == 1) {
        if (event.button == SDL_BUTTON_LEFT) {
            if (currTool == LINE) {
                loadTGA = true;
                framebuffer.LoadTGA("temp.tga", true);
                framebuffer.DrawLineDDA(orig_mouse->x, orig_mouse->y, mouse_position.x, mouse_position.y, framebuffer.defColor);
            }
            else if (currTool == RECTANGLE) {
                loadTGA = true;
                framebuffer.LoadTGA("temp.tga", true);
                int* data = framebuffer.CompRect(*orig_mouse, mouse_position);
                framebuffer.DrawRect(data[0], data[1], data[2], data[3], framebuffer.defColor, framebuffer.defBorderWidth, framebuffer.isFilled, framebuffer.defColor);
                delete data;
            }
        }
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    if(lab == 1) {
        if(currTool == PENCIL) {
            if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                if(mouse_position.y > 50) {
                    if(prev_mouse->x > 0) {
                        if((time-prev_time)<0.05) { // paint = lots of small lines
                            framebuffer.DrawLineDDA(prev_mouse->x, prev_mouse->y, mouse_position.x, mouse_position.y, framebuffer.defColor);
                        }
                    }
                    *prev_mouse = mouse_position;
                    prev_time = time;
                }
            }
        }
        else if(currTool == ERASER) {
            if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                if(mouse_position.y > 50) {
                    if(prev_mouse->x > 0) {
                        if((time-prev_time)<0.05) {
                            framebuffer.DrawLineDDA(prev_mouse->x, prev_mouse->y, mouse_position.x, mouse_position.y, Color::BLACK);
                        }
                    }
                    *prev_mouse = mouse_position;
                    prev_time = time;
                }
            }
        } else if(currTool == LINE) {
            if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                if(mouse_position.y > 50) {
                    if(prev_mouse->x > 0) {
                        framebuffer.DrawLineDDA(orig_mouse->x, orig_mouse->y, prev_mouse->x, prev_mouse->y, Color::BLACK);
                        framebuffer.DrawLineDDA(orig_mouse->x, orig_mouse->y, mouse_position.x, mouse_position.y, framebuffer.defColor);
                    }
                    *prev_mouse = mouse_position;
                }
            }
        } else if(currTool == RECTANGLE) {
            if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                if(mouse_position.y > 50) {
                    int *data = framebuffer.CompRect(*orig_mouse, mouse_position);
                    if(prev_data != NULL) {
                        framebuffer.DrawRect(prev_data[0], prev_data[1], prev_data[2], prev_data[3], Color::BLACK, framebuffer.defBorderWidth, framebuffer.isFilled, Color::BLACK); // erase previous rectangle
                        framebuffer.DrawRect(data[0], data[1], data[2], data[3], framebuffer.defColor, framebuffer.defBorderWidth, framebuffer.isFilled, framebuffer.defColor);
                        delete prev_data;
                    }
                    prev_data = data;
                }
            }
        } else if(currTool == TRIANGLE) {
            if(mouse_position.y > 50) {
                switch(triCounter) {
                    case 0:
                        break;
                    case 1:
                        if(prev_mouse->x > 0) {
                            framebuffer.DrawLineDDA(p0->x, p0->y, prev_mouse->x, prev_mouse->y, Color::BLACK); // erase previous line
                            framebuffer.DrawLineDDA(p0->x, p0->y, mouse_position.x, mouse_position.y, framebuffer.defColor);
                        }
                        *prev_mouse = mouse_position;
                        break;
                    case 2:
                        if(prev_mouse->x > 0) {
                            framebuffer.DrawTriangle(*p0, *p1, *prev_mouse, Color::BLACK, framebuffer.isFilled, Color::BLACK); // erase previous triangle
                            framebuffer.DrawTriangle(*p0, *p1, mouse_position, framebuffer.defColor, framebuffer.isFilled, framebuffer.defColor);
                        }
                        *prev_mouse = mouse_position;
                        break;
                    default:
                        break;
                }
            }
        }
    } else if (lab == 2) {
        if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            Vector2 d = mouse_delta;
            float speed = 0.005f;

            Vector3 v = camera->eye - camera->center;  // Vector from object to camera
			float r = v.Length();  // radius of the sphere on which the camera moves around the center

			// transform from Cartesian to spherical camera's coordinates to get the angles of rotation around the center
			float yaw = atan2f(v.x, v.z);  // 2 * arctan(v.x / v.z) gives the angle of rotation around the y-axis, giving correct quadrants and avoids division by zero
			float pitch = asinf(v.y / r);  // arcsin(v.y / r) gives the angle of rotation around the x-axis, giving correct quadrants and avoids division by zero

			// modify angles based on mouse movement and speed, then transform back to Cartesian coordinates to get the new camera position
            yaw += d.x * speed;
            pitch -= d.y * speed;
			pitch = clamp(pitch, -1.55f, 1.55f); // avoid flip of camera when pitch goes beyond vertical (90 degrees up or down)

			// convert back to Cartesian coordinates to get new camera position, keeping the same radius from the center
            camera->eye = camera->center + Vector3(
                r * cosf(pitch) * sinf(yaw),
                r * sinf(pitch),
                r * cosf(pitch) * cosf(yaw)
            );

            camera->up = Vector3(0, 1, 0);
            camera->LookAt(camera->eye, camera->center, camera->up); // update center and up vectors based on new eye position
            camera->UpdateViewProjectionMatrix();
        } else if(mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            Vector2 delta = mouse_delta;
			camera->Move(Vector3(-delta.x, delta.y, 0) * 0.01f); // move camera in x and y direction based on mouse movement
			camera->UpdateViewProjectionMatrix();
        }
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    if (camPerspective) {
        float dy = event.preciseY;

        if (dy > 0) {  // up -> zoom in
            Vector3 temp = camera->center - camera->eye;
            temp = temp * 0.1f; // move 10% of the distance between eye and center
            currProperty temp2 = camProp;
            camProp = CAM_FAR;
            changeCameraProp(-temp.Length());
            camProp = temp2;
            camera->eye = camera->eye + temp;
            camera->UpdateViewMatrix();
            camera->UpdateViewProjectionMatrix();
        }
        else if (dy < 0) {  // down -> zoom out
            Vector3 temp = camera->eye - camera->center;
            temp = temp * 0.1f; // move 10% of the distance between eye and center
            currProperty temp2 = camProp;
            camProp = CAM_FAR;
            changeCameraProp(temp.Length());
            camProp = temp2;
            camera->eye = camera->eye + temp;
            camera->UpdateViewMatrix();
            camera->UpdateViewProjectionMatrix();
        }
    } else {
        float dy = event.preciseY;

        if (dy > 0) {  // up -> zoom in
			camera->SetOrthographic(camera->left * 0.9f, camera->right * 0.9f, camera->top * 0.9f, camera->bottom * 0.9f, camera->near_plane, camera->far_plane);
            camera->UpdateViewMatrix();
            camera->UpdateViewProjectionMatrix();
        }
        else if (dy < 0) {  // down -> zoom out
			camera->SetOrthographic(camera->left * 1.1f, camera->right * 1.1f, camera->top * 1.1f, camera->bottom * 1.1f, camera->near_plane, camera->far_plane);
            camera->UpdateViewMatrix();
            camera->UpdateViewProjectionMatrix();
        }
    }
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

Button::Button() {
    image = NULL;
    pos = NULL;
}

Button::Button(Image &image, int x, int y, Action type) {
    this->image = &image;
    pos = new Vector2(x, y);
    this->type = type;
}

bool Button::IsMouseInside(Vector2 mousePosition) {
    if((pos->x<=mousePosition.x) && (mousePosition.x<=pos->x+image->width) && (pos->y<=mousePosition.y) && (mousePosition.y<=pos->y+image->height)) {
        return true;
    }
    return false;
}

void Button::DrawButton(Image& framebuffer) {
    Vector2* in = framebuffer.makeInside(*pos);
    const Vector2* use;
    if(in == NULL) {
        use = pos;
    } else {
        use = in;
    }
    
    if(image) {
        framebuffer.DrawImage(*image, use->x, use->y);
    }
}

void ParticleSystem::Init(int w, int h) {
    width = w; height = h;
    for (int i = 0; i < MAX_PARTICLES; ++i)
        Respawn(particles[i]);
}

void ParticleSystem::Render(Image* framebuffer) {
    // Draw as points
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        Particle& p = particles[i];
        if (p.inactive) continue;

        int x = (int)p.position.x;
        int y = (int)p.position.y;

        // bounds check
        if (x < 0 || x >= (int)framebuffer->width || y < 0 || y >= (int)framebuffer->height)
            continue;

        if (p.colorful == 1 || p.colorful == 7) {
            framebuffer->SetPixel((unsigned)x, (unsigned)y, p.color);
        } else {
            for (int i = -4; i <= 4; i++) {
                for (int j = -4; j <= 4; j++) {
                    framebuffer->SetPixel(x + i, y + j, p.color);
                }
            }
        }

    }
}

void ParticleSystem::Update(float dt) {
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        Particle& p = particles[i];
        if (p.inactive) {
            p.colorful = rand() % 10;
            Respawn(p);
            continue;
        }

        p.ttl -= dt;
        if (p.ttl <= 0.0f) {
            p.inactive = true;
            continue;
        }

        // accelerate along direction
        // (we treat velocity as "current velocity" and add scaled direction each frame)
        Vector2 dir = p.velocity;
        float len = dir.length();
        if (len > 0.0001f) dir = dir / len;

        p.velocity += dir * (p.acceleration * dt);
        p.position += p.velocity * dt;

        // kill if outside (with some margin)
        if (p.position.x < -10 || p.position.x > width + 10 ||
            p.position.y < -10 || p.position.y > height + 10) {
            p.inactive = true;
            continue;
        }
    }
}

float ParticleSystem::frand01() {
    return (float)rand() / (float)RAND_MAX;
}

void ParticleSystem::Respawn(Particle& p)
{
    float values[20] = {
        100.0f, 110.0f, 120.0f, 130.0f, 140.0f,
        150.0f, 160.0f, 170.0f, 180.0f, 190.0f,
        200.0f, 210.0f, 220.0f, 230.0f, 240.0f,
        250.0f, 260.0f, 270.0f, 280.0f, 300.0f
    };

    float radius = values[rand() % 20];
    if (p.color.r == 0 && p.color.g == 0 && p.color.b == 0) {
        radius = 100.0f;
    }

    // random angle in [0, 2π)
    float ang = frand01() * (float)(PI * 2.0f);

    // center of circle
    Vector2 center(width * 0.5f, height * 0.5f);

    // random point on circumference
    Vector2 pos(
        center.x + cosf(ang) * radius,
        center.y + sinf(ang) * radius
    );

    p.position = pos;

    // direction = from center to particle position
    Vector2 dir = p.position - center;

    // safety: avoid zero-length vector
    if (dir.length() < 0.0001f)
    {
        float ang = frand01() * (float)(PI * 2.0);
        dir = Vector2(cosf(ang), sinf(ang));
    }
    else
    {
        dir.normalize();
    }

    // speed
    float baseSpeed = 250.0f + frand01() * 350.0f;
    p.velocity = dir * baseSpeed;

    if (p.colorful == 1 || p.colorful == 7) {
        // COLOR DEPENDS ON SPEED
        if (baseSpeed < 300.0f)
            p.color = Color::BLUE;                     // 250–300 (blue star)
        else if (baseSpeed < 350.0f)
            p.color = Color::CYAN;                     // 300–350 (cyan star)
        else if (baseSpeed < 400.0f)
            p.color = Color(170, 200, 255);             // light blue star
        else if (baseSpeed < 450.0f)
            p.color = Color::WHITE;                    // white star
        else if (baseSpeed < 500.0f)
            p.color = Color::YELLOW;                   // yellow star
        else if (baseSpeed < 550.0f)
            p.color = Color(255, 170, 100);             // orange star
        else
            p.color = Color::PURPLE;                   // pink/purple star
    }
    else {
        p.color = Color::BLACK;
    }

    // acceleration
    p.acceleration = 50.0f + frand01() * 220.0f;

    // ttl
    p.ttl = 0.6f + frand01() * 1.0f;

    p.inactive = false;
}
