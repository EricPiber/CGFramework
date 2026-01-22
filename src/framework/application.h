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
        int colorful = 0;
    };

    Particle particles[MAX_PARTICLES];

    void Init(int w, int h) {
        width = w; height = h;
        for (int i = 0; i < MAX_PARTICLES; ++i)
            Respawn(particles[i], true);
    }

    void Render(Image* framebuffer) {
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

    void Update(float dt) {
        for (int i = 0; i < MAX_PARTICLES; ++i) {
            Particle& p = particles[i];
            if (p.inactive) {
                p.colorful = rand() % 10;
                Respawn(p, false);
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

    void OnResize(int w, int h) {
        width = w; height = h;
    }

private:
    int width = 0;
    int height = 0;

    // helper random in [0,1]
    float frand01() {
        return (float)rand() / (float)RAND_MAX;
    }

    // respawn particle near center with outward velocity
    void Respawn(Particle& p, bool initial)
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
