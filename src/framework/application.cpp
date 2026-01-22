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
    butClear = Button(clear, 50, 10, CLEAR);
    
}

// Render one frame
void Application::Render(void)
{
	// ...
    
    
    
    framebuffer.Fill(Color::BLACK);
    
    
    
    Vector2 p0(100, 100);
    Vector2 p1(170, 230);
    Vector2 p2(550, 50);
    
    framebuffer.DrawTriangle(p0, p1, p2, Color::RED, true, Color::BLUE);

    Vector2 p3(200, 140);
    Vector2 p4(500, 700);
    Vector2 p5(700, 30);
    
    framebuffer.DrawTriangle(p3, p4, p5, Color::GREEN, true, Color::BLACK);
    
    
    framebuffer.DrawImage(pencil, 10, 10);
    
    butClear.DrawButton(framebuffer);
    
    
    
    /*int x=50;
    int y=50;
    int w=300;
    int h=150;
    //framebuffer.defBorderWidth = 10;
    framebuffer.DrawRect(x, y, w, h, Color::WHITE, framebuffer.defBorderWidth, true, Color::RED);
    framebuffer.DrawRect(400, 400, w, h, Color::WHITE, framebuffer.defBorderWidth);*/
    // hola

	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
        case SDLK_PLUS: framebuffer.defBorderWidth++; break;
        case SDLK_MINUS: framebuffer.defBorderWidth--; break;
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
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
    x = 0; y = 0;
}

Button::Button(Image &image, int x, int y, Action type) {
    this->image = &image;
    this->x = x;
    this->y = y;
    this->type = type;
}

bool Button::IsMouseInside(Vector2 mousePosition) {
    if((x<=mousePosition.x) && (mousePosition.x<=x+image->width) && (y<=mousePosition.y) && (mousePosition.y<=y+image->height)) {
        return true;
    }
    return false;
}

void Button::DrawButton(Image& framebuffer) {
    if(image) {
        framebuffer.DrawImage(*image, x, y);
    }
}
