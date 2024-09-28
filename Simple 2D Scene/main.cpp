/**
* Author: [Steven Ha]
* Assignment: Simple 2D Scene
* Date due: 2023-09-28, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr int	WINDOW_WIDTH = 640 * 2,
				WINDOW_HEIGHT = 480 * 2;

constexpr float	BG_RED = 0.5f,
				BG_GREEN = 0.9f,
				BG_BLUE = 0.7f,
				BG_OPACITY = 1.0f;

constexpr int	VIEWPORT_X = 0,
				VIEWPORT_Y = 0,
				VIEWPORT_WIDTH = WINDOW_WIDTH,
				VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char	V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
				F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1,
				LEVEL_OF_DETAIL = 0,
				TEXTURE_BORDER = 0; 

constexpr char  PKMN_PATH[] = "sceptile.png",
				BALL_PATH[] = "pokeball.png",
                SHADOW_PATH[] = "shadow.png";

constexpr glm::vec3 INIT_SCALE_PKMN = glm::vec3(4.0f, 4.8f, 0.0f),
					INIT_POS_PKMN = glm::vec3(2.0f, 0.0f, 0.0f),
	                INIT_SCALE_BALL = glm::vec3(0.5f, 0.5f, 0.0f),
                    INIT_POS_BALL = glm::vec3(-2.0f, 0.0f, 0.0f),
                    INIT_SCALE_SHADOW = glm::vec3(4.0f, 0.51f, 0.0f),
                    INIT_POS_SHADOW = glm::vec3(2.0f, -2.0f, 0.0f);

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4	g_view_matrix,
			g_pkmn_matrix,
			g_ball_matrix,
        	g_shadow_matrix,
			g_projection_matrix;

constexpr float RADIUS = 2.0f,
                PKMN_RADIUS = 0.25f;

constexpr float PULSE_SPEED = 4.0f,
                PULSE_TIME = 0.2f,
                JUMP_SPEED = 8.0f,
                ORBIT_SPEED_BALL = 3.0f,
                ROTATION_SPEED_BALL = 7.0f;

float       g_angle_ball = 0.0f,
            g_angle_pkmn = 0.0f;
float       g_x_offset_ball = 0.0f,  
            g_y_offset_ball = 0.0f,
            g_x_offset_pkmn = 0.0f,
            g_y_offset_pkmn = 0.0f;
float   g_previous_ticks = 0.0f,
        g_pulse_time = 0.0f;
int direction = 1;

glm::vec3	g_translation_pkmn = glm::vec3(0.0f, 0.0f, 0.0f),
            g_translation_ball = glm::vec3(0.0f, 0.0f, 0.0f),
            g_translation_shadow = glm::vec3(0.0f, 0.0f, 0.0f),
            g_rotation_pkmn = glm::vec3(0.0f, 0.0f, 0.0f),
            g_rotation_ball = glm::vec3(0.0f, 0.0f, 0.0f),
            g_rotation_shadow = glm::vec3(0.0f, 0.0f, 0.0f),
            g_scale_pkmn = glm::vec3(1.0f, 1.0f, 1.0f),
            g_scale_ball = glm::vec3(1.0f, 1.0f, 1.0f),
	        g_scale_shadow = glm::vec3(1.0f, 1.0f, 1.0f);

GLuint	g_pkmn_texture_id,
		g_ball_texture_id,
        g_shadow_texture_id;

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Simple 2D Scene",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_pkmn_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::mat4(1.0f);
	g_shadow_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    g_pkmn_texture_id = load_texture(PKMN_PATH);
    g_ball_texture_id = load_texture(BALL_PATH);
	g_shadow_texture_id = load_texture(SHADOW_PATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    /* Delta time calculations */
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    /* Model matrix reset */
    g_pkmn_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::mat4(1.0f);
	g_shadow_matrix = glm::mat4(1.0f);

	/* Update transformations */
    g_pulse_time += delta_time * PULSE_SPEED;
    float scale_factor = PULSE_TIME * glm::sin(g_pulse_time);
    
    g_rotation_ball.z += ROTATION_SPEED_BALL * delta_time;
    g_angle_ball -= ORBIT_SPEED_BALL * delta_time;

    g_x_offset_ball = RADIUS * glm::cos(g_angle_ball);
    g_y_offset_ball = RADIUS * glm::sin(g_angle_ball);
    if (g_y_offset_ball < 0.0f) {
        g_ball_matrix = glm::translate(g_ball_matrix, glm::vec3(g_x_offset_ball, g_y_offset_ball, -10.0f));
    }
    else {
        g_ball_matrix = glm::translate(g_ball_matrix, glm::vec3(g_x_offset_ball, g_y_offset_ball, 0.0f));
    }
    
    g_angle_pkmn -= JUMP_SPEED * delta_time;
    g_x_offset_pkmn = PKMN_RADIUS * glm::cos(g_angle_pkmn);
    g_y_offset_pkmn = PKMN_RADIUS * abs(glm::sin(g_angle_pkmn));
    g_pkmn_matrix = glm::translate(g_pkmn_matrix, glm::vec3(g_x_offset_pkmn, g_y_offset_pkmn, 0.0f));

    /* Transformations */
    g_pkmn_matrix = glm::translate(g_pkmn_matrix, INIT_POS_PKMN);
    g_pkmn_matrix = glm::rotate(g_pkmn_matrix,
                                g_rotation_pkmn.y,
                                glm::vec3(0.0f, 1.0f, 0.0f));
    g_pkmn_matrix = glm::scale(g_pkmn_matrix, INIT_SCALE_PKMN + glm::vec3(scale_factor, scale_factor, 1.0f));

    g_ball_matrix = glm::translate(g_ball_matrix, INIT_POS_BALL);
    g_ball_matrix = glm::rotate(g_ball_matrix,
                                g_rotation_ball.z,
                                glm::vec3(.0f, 0.0f, 1.0f));
    g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);

    g_shadow_matrix = glm::translate(g_shadow_matrix, glm::vec3(g_x_offset_pkmn + INIT_POS_SHADOW.x, INIT_POS_SHADOW.y, INIT_POS_SHADOW.z));
    g_shadow_matrix = glm::rotate(g_shadow_matrix,
        g_rotation_shadow.y,
        glm::vec3(0.0f, 1.0f, 0.0f));
    g_shadow_matrix = glm::scale(g_shadow_matrix, INIT_SCALE_SHADOW);

}

void draw_object(glm::mat4& object_g_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 9);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
        0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_shadow_matrix, g_shadow_texture_id);
    draw_object(g_pkmn_matrix, g_pkmn_texture_id);
    draw_object(g_ball_matrix, g_ball_texture_id);
	

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


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