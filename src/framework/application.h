/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"

enum Action {
    CLEAR,
    LOAD,
    SAVE,
    PENCIL,
    ERASER,
    LINE,
    RECTANGLE,
    TRIANGLE,
    BLACK,
    WHITE,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    PINK
};

class Button
{
public:
    Image* image;
    const Vector2 *pos;
    Action type;
    
    // CONSTRUCTOR
    Button();
    Button(Image &image, int x, int y, Action type);
    
    // MAIN FUNCTIONS
    bool IsMouseInside(Vector2 mousePosition);
    void DrawButton(Image& framebuffer);
};

class ParticleSystem
{
public:
    static const int MAX_PARTICLES = 100;

    struct Particle {
        Vector2 position;
        Vector2 velocity;   // direction * base speed
        Color color;
        float acceleration; // adds speed over time
        float ttl;          // time to live (seconds)
        bool inactive;
		int colorful = 0;   // 1 or 7 = colorful stars, else black to erase previous stars
    };

    Particle particles[MAX_PARTICLES];

    void Init(int w, int h);
    void Render(Image* framebuffer);
    void Update(float dt);
    void OnResize(int w, int h);

private:
    int width = 0;
    int height = 0;

    // helper random in [0,1]
    float frand01();

    // respawn particle near center with outward velocity
    void Respawn(Particle& p, bool initial);
};

class Application
{
public:

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;
    bool loadTGA = false;
    Action currTool;

    ParticleSystem starfield;
    bool starfield_initialized = false;

	float time;
    float prev_time = 0;

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame
    Vector2 *prev_mouse = new Vector2();
    Vector2 *orig_mouse = new Vector2();
    int *prev_data = NULL;
    int triCounter = 0;
    Vector2 *p0 = new Vector2();
    Vector2 *p1 = new Vector2();
    Vector2 *p2 = new Vector2();

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;
    
    // OUR IMAGES
    Image clear;
    Image load;
    Image save;
    Image pencil;
    Image eraser;
    Image line;
    Image rectangle;
    Image triangle;
    Image black;
    Image white;
    Image red;
    Image green;
    Image blue;
    Image yellow;
    Image cyan;
    Image pink;
    

    Button butClear;
	Button butLoad;
	Button butSave;
	Button butPencil;
	Button butEraser;
	Button butLine;
	Button butRectangle;
	Button butTriangle;
	Button butBlack;
	Button butWhite;
	Button butRed;
	Button butGreen;
	Button butBlue;
	Button butYellow;
	Button butCyan;
	Button butPink;

    
	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );
	void makeAction(Action action);
	void makeAnimation();
	void paint();

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
};
