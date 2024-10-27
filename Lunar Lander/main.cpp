/**
* Author: [Steven Ha]
* Assignment: Lunar Lander
* Date due: 2024-10-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 33
#define LEVEL1_HEIGHT 20

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

// ————— GAME STATE ————— //
struct GameState
{
    Entity *player;
    Map *map;
	Map *success;
};

enum AppStatus { RUNNING, TERMINATED};
enum WinCondition { NOY_YET, WON, LOST };

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 1280,
          WINDOW_HEIGHT = 1024;

constexpr float BG_RED     = 1.0f,
            BG_BLUE    = 0.6,
            BG_GREEN   = 0.8,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char GAME_WINDOW_NAME[] = "UFO Landing!";

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char SPRITESHEET_FILEPATH[] = "assets/ship-sheet.png",
MAP_TILESET_FILEPATH[] = "assets/tileset.png",
BACKGROUND_FILEPATH[] = "assets/commands.png",
WIN_FILEPATH[] = "assets/win.png",
LOSE_FILEPATH[] = "assets/lose.png",
FUEL_FILEPATH[] = "assets/fuel.png";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;


unsigned int LEVEL_1_DATA[] =
{   0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 3,
    3, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3,
    3, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 3,
    3, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 3
};

unsigned int SUCCESS[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
WinCondition g_win_condition = NOY_YET;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f,
      g_accumulator    = 0.0f;

float fuel_left = 100.0f,
      fuel_used = 0.0f;

void initialise();
void process_input();
void update();
void render();
void render_background(ShaderProgram* program, GLuint texture_id);
void render_win(ShaderProgram* program, GLuint texture_id);
void render_fuel(ShaderProgram* program, GLuint texture_id);
void shutdown();

// ————— GENERAL FUNCTIONS ————— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return texture_id;
}

void initialise()
{
	g_win_condition = NOY_YET;
	fuel_left = 100.0f;
    fuel_used = 0.0f;

    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-15.0f, 15.0f, -11.25f, 11.25f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	

    

    // ————— UFO SET-UP ————— //

    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    int player_moving_animation[4][4] =
    {
        { 2, 6, 10, 14 }, { 0, 4, 8, 12 }, { 1, 5, 9, 13 }, { 3, 7, 11, 15 }

    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        3.0f,                      // jumping power
        player_moving_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.9f,                      // width
        0.9f,                       // height
        PLAYER
    );

    g_game_state.player->set_position(glm::vec3(0.0f, 5.0f, 0.0f));
    g_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    g_game_state.player->set_height(0.35f);
    g_game_state.player->face_down();

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 1);

    GLuint success_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.success = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, SUCCESS, success_texture_id, 1.0f, 4, 1);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_game_state.player->set_acceleration(glm::vec3(0.0f, -0.5f, 0.0f));
    float y = g_game_state.player->get_velocity().y;
    float x = g_game_state.player->get_velocity().x;
    if (y < 0 && y < x) {
        g_game_state.player->face_down();
    }
    else if (y > 0 && y > x) {
        g_game_state.player->face_up();
    }
    else if (x < 0 && x < y) {
        g_game_state.player->face_left();
    }
    else if (x > 0 && x > y) {
        g_game_state.player->face_right();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {

        
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
			g_app_status = TERMINATED;
            break;
            

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;
            case SDLK_r:
				// close current game and restart
				SDL_DestroyWindow(g_display_window);
                // shutdown(); <- Somehow it takes a while to initialise and crash sometimes
				initialise();
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        if (!g_game_state.player->get_collided_bottom() && 
            !g_game_state.player->get_collided_top() && 
            !g_game_state.player->get_collided_right() && 
            !g_game_state.player->get_collided_left() && fuel_left >= 0) {
            g_game_state.player->add_acceleration(glm::vec3(-3.0f, 0.0f, 0.0f));
			g_game_state.player->face_left();
			fuel_left -= 0.1f;
			fuel_used += 0.1f;
        }
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        if (!g_game_state.player->get_collided_bottom() && 
            !g_game_state.player->get_collided_top() && 
            !g_game_state.player->get_collided_right() && 
            !g_game_state.player->get_collided_left() && fuel_left >= 0) {
            g_game_state.player->add_acceleration(glm::vec3(3.0f, 0.0f, 0.0f));
			g_game_state.player->face_right();
			fuel_left -= 0.1f;
			fuel_used += 0.1f;
        }
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        if (!g_game_state.player->get_collided_bottom() && 
            !g_game_state.player->get_collided_top() && 
            !g_game_state.player->get_collided_right() && 
            !g_game_state.player->get_collided_left() && fuel_left >= 0) {
            g_game_state.player->add_acceleration(glm::vec3(0.0f, 3.0f, 0.0f));
			g_game_state.player->face_up();  
			fuel_left -= 0.1f;
			fuel_used += 0.1f;
        }
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        if (!g_game_state.player->get_collided_bottom() && 
            !g_game_state.player->get_collided_top() && 
            !g_game_state.player->get_collided_right() && 
            !g_game_state.player->get_collided_left() && fuel_left >= 0) {
            g_game_state.player->add_acceleration(glm::vec3(0.0f, -2.0f, 0.0f));
			g_game_state.player->face_down();
			fuel_left -= 0.1f;
        }
    }


    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->normalise_movement();
    }
}



void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        if (!g_game_state.player->get_collided_bottom() && !g_game_state.player->get_collided_top() && 
            !g_game_state.player->get_collided_left() && !g_game_state.player->get_collided_right())
        {
            g_game_state.player->update(0.0f, g_game_state.player, NULL, 0,
                g_game_state.success);
			if (g_game_state.player->get_collided_bottom())
			{
                g_game_state.player->face_down();
                g_game_state.player->set_animation_index(0);
				g_game_state.player->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
				g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
                if (!g_game_state.player->get_collided_bottom() && !g_game_state.player->get_collided_left() && !g_game_state.player->get_collided_right() && !g_game_state.player->get_collided_top()) {
					g_win_condition = WON;
					g_game_state.player->deactivate();
                }
                else {
					g_win_condition = LOST;
                }
				delta_time -= FIXED_TIMESTEP;
                continue;
			}
            g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0,
                g_game_state.map);
            
        }
        else {
			g_game_state.player->face_up();
			g_game_state.player->set_animation_index(0);
			g_game_state.player->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
            g_win_condition = LOST;;
            delta_time -= FIXED_TIMESTEP;
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;

    
    g_view_matrix = glm::mat4(1.0f);
    
    // Camera Follows the player
    const float LEFT_LIMIT = -4.0f;
    const float RIGHT_LIMIT = 4.0f;

    float player_x = g_game_state.player->get_position().x;
    float player_y = g_game_state.player->get_position().y;

    float camera_target_x = 0.0f;

    if (player_x >= LEFT_LIMIT && player_x <= RIGHT_LIMIT) {
        camera_target_x = player_x;
    }
    else if (player_x < LEFT_LIMIT) {
        camera_target_x = LEFT_LIMIT;
    }
    else {
        camera_target_x = RIGHT_LIMIT;
    }
    g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-camera_target_x, 0.0f, 0.0f));
    

}

void render_background(ShaderProgram* program, GLuint texture_id) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(-8.0f, 6.5f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(4.0f, 1.0f, 1.0f));
    program->set_model_matrix(model_matrix);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f
    };

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices + 2);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
void render_win(ShaderProgram* program, GLuint texture_id) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, -0.2f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 4.0f, 1.0f));
    program->set_model_matrix(model_matrix);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f
    };

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices + 2);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void render_fuel(ShaderProgram* program, GLuint texture_id) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
	if (fuel_left > 0.0f)
    {
    model_matrix = glm::translate(model_matrix, glm::vec3(3.2f - (fuel_used / 100.0f) * 7.0f, 7.2f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(7.0f * fuel_left / 100.0f, 0.2f, 1.0f));
	}
	else
	{
		model_matrix = glm::scale(model_matrix, glm::vec3(0.0f, 0.2f, 1.0f));
	}
	program->set_model_matrix(model_matrix);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f
	};

	glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices);
	glEnableVertexAttribArray(program->get_position_attribute());

	glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 4 * sizeof(float), vertices + 2);
	glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(program->get_position_attribute());
	glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    GLuint background_texture_id = load_texture(BACKGROUND_FILEPATH);
    render_background(&g_shader_program, background_texture_id);

	GLuint fuel_texture_id = load_texture(FUEL_FILEPATH);
	render_fuel(&g_shader_program, fuel_texture_id);

    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);
	g_game_state.success->render(&g_shader_program);
    
	if (g_win_condition == WON)
	{
		GLuint win_texture_id = load_texture(WIN_FILEPATH);
		render_win(&g_shader_program, win_texture_id);
	}
	else if (g_win_condition == LOST)
	{
		GLuint lose_texture_id = load_texture(LOSE_FILEPATH);
		render_win(&g_shader_program, lose_texture_id);
	}

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    delete    g_game_state.player;
    delete    g_game_state.map;
	delete    g_game_state.success;
}


// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
