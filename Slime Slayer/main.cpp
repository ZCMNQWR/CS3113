/**
* Author: [Steven Ha]
* Assignment: [Slime Slayer]
* Date due: [December 6, 2024], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 10
#define LEVEL1_LEFT_EDGE 5.0f
#define ENEMY_COUNT 3

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640*2,
          WINDOW_HEIGHT = 480*2;

constexpr float BG_RED     = 0.2f,
            BG_BLUE    = 0.5f,
            BG_GREEN   = 0.9f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };
enum WinCondition { WIN, LOSE, ONGOING };

Mix_Music* bgm;

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Menu* g_menu;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;

Scene* g_levels[3];


SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
WinCondition g_win_status = ONGOING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;


void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Slime Slayer!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
	// ————— MENU ————— //
	g_menu = new Menu();
	
    // ————— LEVEL SETUPS ————— //
	g_level_a = new LevelA();
	g_level_b = new LevelB();
	g_level_c = new LevelC();
    
	g_levels[0] = g_level_a;
	g_levels[1] = g_level_b;
	g_levels[2] = g_level_c;
    
    switch_to_scene(g_menu);
    //switch_to_scene(g_level_a);
    //switch_to_scene(g_level_b);
    //switch_to_scene(g_level_c);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    bgm = Mix_LoadMUS("assets/music.mp3");
    Mix_PlayMusic(bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
}

void process_input()
{
    if (g_current_scene != g_menu)
    {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
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
                        
                    case SDLK_a:
                        if (g_current_scene != g_menu)
                        {
                            if (g_current_scene->get_state().player->get_is_active())
                            {
                                Mix_PlayChannel(-1, g_current_scene->get_state().attack_sfx, 0);
                                g_current_scene->get_state().player->set_attack(true);
                                g_current_scene->get_state().player->check_kill(g_current_scene->get_state().enemies, 
                                    g_current_scene->get_number_of_enemies());
                                g_current_scene->get_state().player->set_texture_id(Utility::load_texture("assets/attack.png"));
                                g_current_scene->get_state().player->set_animation_index(1);
                            }
                        }
                        break;
                    
                        
					case SDLK_RETURN:
						// ————— SWITCHING SCENES ————— //

                        if (g_current_scene == g_menu)
                        {
                            g_level_a = new LevelA();
                            switch_to_scene(g_level_a);
                        }
                    default:
                        break;
                }
                
            default:
                break;
        }
		
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (g_current_scene != g_menu && g_win_status == ONGOING)
    {
        
        if (key_state[SDL_SCANCODE_LEFT] && g_current_scene->get_state().player->get_is_active())
            g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT] && g_current_scene->get_state().player->get_is_active())
            g_current_scene->get_state().player->move_right();
		else if (key_state[SDL_SCANCODE_UP] && g_current_scene->get_state().player->get_is_active())
			g_current_scene->get_state().player->move_up();
		else if (key_state[SDL_SCANCODE_DOWN] && g_current_scene->get_state().player->get_is_active())
			g_current_scene->get_state().player->move_down();
        
        if (key_state[SDL_SCANCODE_LSHIFT] && g_current_scene->get_state().player->get_is_active())
            g_current_scene->get_state().player->set_speed(8.0f);
        else g_current_scene->get_state().player->set_speed(3.0f);

        

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    if (g_current_scene != g_menu)
    {

        if (g_current_scene->get_state().player->get_animation_index() == 0 && g_current_scene->get_state().player->get_attack())
        {
			g_current_scene->get_state().player->set_attack(false);
            g_current_scene->get_state().player->set_texture_id(Utility::load_texture("assets/player.png"));
            
        }
    }
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;


    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);

    if (g_current_scene != g_menu)
    {

        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));



        int curr_life = g_current_scene->get_state().player->get_life();
        if (g_current_scene == g_level_a && g_current_scene->get_state().goal->get_goal_reached())
        {
            switch_to_scene(g_level_b);
            g_current_scene->get_state().player->set_life(curr_life);
            g_current_scene->get_state().goal->not_reached_goal();
        }
        if (g_current_scene == g_level_b && g_current_scene->get_state().goal->get_goal_reached())
        {
            switch_to_scene(g_level_c);
            g_current_scene->get_state().player->set_life(curr_life);
            g_current_scene->get_state().goal->not_reached_goal();
        }
        if (g_current_scene == g_level_c && g_current_scene->get_state().goal->get_goal_reached())
        {
            g_win_status = WIN;
        }
        if (g_current_scene->get_state().player->get_life() == 0)

        {
            Mix_VolumeMusic(0);
            GLint keyLocation = glGetUniformLocation(g_shader_program.get_program_id(), "key");
            glUniform1i(keyLocation, 1);
        }
        if (g_current_scene == g_level_c && g_current_scene->get_state().goal->get_goal_reached())
        {
			Mix_VolumeMusic(0);
			GLint keyLocation = glGetUniformLocation(g_shader_program.get_program_id(), "key");
			glUniform1i(keyLocation, 2);
        }
    }
    
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);

    

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program, g_view_matrix);
    
    SDL_GL_SwapWindow(g_display_window);



}

void shutdown()
{    
    SDL_Quit();
    
    // ————— DELETING LEVEL DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
	delete g_level_b;
	delete g_level_c;

	// ————— DELETING MENU DATA ————— //
	delete g_menu;


	// ————— DELETING WINDOW ————— //
	SDL_DestroyWindow(g_display_window);
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
