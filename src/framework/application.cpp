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
    
    
    framebuffer.Fill(Color::BLACK);
    
}

// Render one frame
void Application::Render(void)
{
	// ...
    
    
    //framebuffer.Resize(window_width, window_height);

    if (starfield_initialized) {
        starfield.Render(&framebuffer);
    }

    if (!loadTGA) {
    }
    
    framebuffer.DrawRect(0, 0, framebuffer.width, 50, Color::GRAY, 1, true, Color::GRAY); // Menu bar
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
    
    framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    if (starfield_initialized)
        starfield.Update(seconds_elapsed);
}

void Application::makeAction(Action action) {
    switch(action) {
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
    if(!starfield_initialized) {
        framebuffer.SaveTGA("paint.tga");
    }
    starfield.Init(framebuffer.width, framebuffer.height);
	makeAction(CLEAR);
    starfield_initialized = true;
}

void Application::paint() {
    makeAction(CLEAR);
	starfield_initialized = false;
	loadTGA = true;
    framebuffer.LoadTGA("paint.tga", true);
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
        case SDLK_PLUS: framebuffer.defBorderWidth++; break;
        case SDLK_MINUS: framebuffer.defBorderWidth--; break;
        case SDLK_1: paint(); break;
        case SDLK_2: makeAnimation(); break;
        case SDLK_f: framebuffer.isFilled = !framebuffer.isFilled; break;
        default: break;
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
        if (butClear.IsMouseInside(mouse_position)) {
            makeAction(CLEAR);
        } else if (butLoad.IsMouseInside(mouse_position)) {
            makeAction(LOAD);
        } else if (butSave.IsMouseInside(mouse_position)) {
            makeAction(SAVE);
        } else if (butPencil.IsMouseInside(mouse_position)) {
            makeAction(PENCIL);
        } else if (butEraser.IsMouseInside(mouse_position)) {
            makeAction(ERASER);
        } else if (butLine.IsMouseInside(mouse_position)) {
            makeAction(LINE);
        } else if (butRectangle.IsMouseInside(mouse_position)) {
            makeAction(RECTANGLE);
        } else if (butTriangle.IsMouseInside(mouse_position)) {
            makeAction(TRIANGLE);
        } else if (butBlack.IsMouseInside(mouse_position)) {
            makeAction(BLACK);
        } else if (butWhite.IsMouseInside(mouse_position)) {
            makeAction(WHITE);
        } else if (butRed.IsMouseInside(mouse_position)) {
            makeAction(RED);
        } else if (butGreen.IsMouseInside(mouse_position)) {
            makeAction(GREEN);
        } else if (butBlue.IsMouseInside(mouse_position)) {
            makeAction(BLUE);
        } else if (butYellow.IsMouseInside(mouse_position)) {
            makeAction(YELLOW);
        } else if (butCyan.IsMouseInside(mouse_position)) {
            makeAction(CYAN);
        } else if (butPink.IsMouseInside(mouse_position)) {
            makeAction(PINK);
        }
        if (currTool == LINE) {
            *orig_mouse = mouse_position;
            framebuffer.SaveTGA("temp.tga");
        }
        else if (currTool == RECTANGLE) {
            *orig_mouse = mouse_position;
            framebuffer.SaveTGA("temp.tga");
        }
        else if (currTool == TRIANGLE) {
            if(mouse_position.y > 50) {
                switch(triCounter) {
                    case 0:
                        *p0 = mouse_position;
                        framebuffer.SaveTGA("temp.tga");
                        triCounter = 1;
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

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
        if(currTool == LINE) {
            loadTGA = true;
            framebuffer.LoadTGA("temp.tga", true);
            framebuffer.DrawLineDDA(orig_mouse->x, orig_mouse->y, mouse_position.x, mouse_position.y, framebuffer.defColor);
        }
        else if(currTool == RECTANGLE) {
            loadTGA = true;
            framebuffer.LoadTGA("temp.tga", true);
            int *data = framebuffer.CompRect(*orig_mouse, mouse_position);
            framebuffer.DrawRect(data[0], data[1], data[2], data[3], framebuffer.defColor, framebuffer.defBorderWidth, framebuffer.isFilled, framebuffer.defColor);
            delete data;
        }
	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    if(currTool == PENCIL) {
        if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            if(mouse_position.y > 50) {
                if(prev_mouse->x > 0) {
                    if((time-prev_time)<0.05) {
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
                    framebuffer.DrawRect(prev_data[0], prev_data[1], prev_data[2], prev_data[3], Color::BLACK, framebuffer.defBorderWidth, framebuffer.isFilled);
                    framebuffer.DrawRect(data[0], data[1], data[2], data[3], framebuffer.defColor, framebuffer.defBorderWidth, framebuffer.isFilled);
                    delete prev_data;
                }
                prev_data = data;
            }
        }
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
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
