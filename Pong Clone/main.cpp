#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <ctime>
#include "cmath"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH = 800,
WINDOW_HEIGHT = 600;

constexpr float BG_RED = 0.05f,
BG_GREEN = 0.15f,
BG_BLUE = 0.3f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char LEFT_PLAYER_FILEPATH[] = "assets/hammer-sheet.png",
RIGHT_PLAYER_FILEPATH[] = "assets/shark-sheet.png",
BALL_FILEPATH[] = "assets/fish-sheet.png",
FONTSHEET_FILEPATH[] = "assets/font1.png";

constexpr GLint NUMBER_OF_TEXTURES = 1,
LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

/* SPRITESHEET */
constexpr int FONTBANK_SIZE = 16;

int shark_moving[2][6] = { 
    {1, 2, 3, 4, 5, 12}, //up
	{6, 7, 8, 9, 8, 7} //down
};
int fish_moving[2][2] = {
	{0, 1}, //right
	{2, 3} //left
};

GLuint right_player_id;
GLuint left_player_id;
GLuint ball_id;
GLuint g_font_texture_id;

float SPEED_PLAYER = 15.0f,
SPEED_BALL = 7.0f;

int* animation_indices = shark_moving[0];
int* animation_indices_hammer = shark_moving[0];
int* animation_indices_fish[3] = { fish_moving[0], fish_moving[0], fish_moving[0]};
int animation_frame = 6;
int animation_frame_fish[3] = { 2, 2, 2 };
int animation_index= 0;
int animation_index_fish[3] = { 0, 0, 0 };

float animation_time = 0.0f;
float animation_time_fish[3] = { 0.0f, 0.0f, 0.0f };
void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index,
    int rows, int cols);
void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position);
/*----------------------*/

SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
right_player_matrix,
left_player_matrix,
ball_matrix[3],
g_projection_matrix;

float previous_ticks = 0.0f;

glm::vec3 right_player_pos = glm::vec3(9.0f, 0.0f, 0.0f),
INIT_SCALE_PLAYER_R = glm::vec3(1.5f, 3.0f, 0.0f),
right_player_movement = glm::vec3(0.0f, 0.0f, 0.0f),
left_player_pos = glm::vec3(-9.0f, 0.0f, 0.0f),
INIT_SCALE_PLAYER_L = glm::vec3(1.5f, 3.0f, 0.0f),
left_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 ball_position[3] = { glm::vec3(0.0f, 0.0f, 0.0f), 
                                glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f) };
glm::vec3 ball_movement[3] = { glm::vec3(0.0f, 0.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 0.0f) };
glm::vec3 INIT_SCALE_BALL = glm::vec3(1.0f, 0.6f, 0.0f);

float WALL_RIGHT = 10.0f,
WALL_LEFT = -10.0f,
WALL_UP = 7.5f,
WALL_DOWN = -7.5f;

int num_balls = 0;

int SINGLE_PLAYER = -1;
int MULTI_PLAYER = 1;
int gamemode_status = MULTI_PLAYER;

enum direction { UP, DOWN};
direction r_move = UP, l_move = UP;


enum winner { LEFT, RIGHT, NONE };
winner win = NONE;


void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath);
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id);

void draw_sprite_from_texture_atlas(ShaderProgram* shaderProgram, GLuint texture_id, int index,
    int rows, int cols)
{
    // Calculate the UV location of the indexed frame
    float u_coord = (float)(index % cols) / (float)cols;
    float v_coord = (float)(index / cols) / (float)rows;

    // Calculate its UV size
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;

    // Match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
        v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(shaderProgram->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices);
    glEnableVertexAttribArray(shaderProgram->get_position_attribute());

    glVertexAttribPointer(shaderProgram->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        tex_coords);
    glEnableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(shaderProgram->get_position_attribute());
    glDisableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());
}

void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int)text[i]; 
        float offset = (font_size + spacing) * i;

        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    shader_program->set_model_matrix(model_matrix);
    glUseProgram(shader_program->get_program_id());

    glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(shader_program->get_position_attribute());

    glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(shader_program->get_position_attribute());
    glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
        STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Ocean Pong!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    right_player_matrix = glm::mat4(1.0f);
	left_player_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    left_player_id = load_texture(LEFT_PLAYER_FILEPATH);
	right_player_id = load_texture(RIGHT_PLAYER_FILEPATH);
	ball_id = load_texture(BALL_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ball_movement[0] = glm::vec3(1.0f, .3f, 0.0f);
	ball_movement[1] = glm::vec3(1.3f, .4f, 0.0f);
	ball_movement[2] = glm::vec3(1.5f, -.5f, 0.0f);

	glm::normalize(ball_movement[0]);

	g_font_texture_id = load_texture(FONTSHEET_FILEPATH);

}

void process_input()
{
    right_player_movement = glm::vec3(0.0f);
	left_player_movement = glm::vec3(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE: g_app_status = TERMINATED; break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
			case SDLK_1: num_balls = 1; break;
			case SDLK_2: num_balls = 2; break;
			case SDLK_3: num_balls = 3; break;
            case SDLK_q: g_app_status = TERMINATED; break;
            case SDLK_t: gamemode_status *= -1; break;
            default: break;
            }

        default: break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (win == NONE)
    {
        if (key_state[SDL_SCANCODE_W])
        {
            if (left_player_pos.y < WALL_UP - (INIT_SCALE_PLAYER_L.y / 2.0f))
            {
                left_player_movement.y = 1.0f;
				animation_indices_hammer = shark_moving[0];
            }
        }
        if (key_state[SDL_SCANCODE_S])
        {
            if (left_player_pos.y > WALL_DOWN + (INIT_SCALE_PLAYER_L.y / 2.0f))
            {
                left_player_movement.y = -1.0f;
				animation_indices_hammer = shark_moving[1];
            }
        }
        if (key_state[SDL_SCANCODE_UP])
        {
            if (gamemode_status == MULTI_PLAYER)
            {
                if (right_player_pos.y < WALL_UP - (INIT_SCALE_PLAYER_R.y / 2.0f))
                {
                    right_player_movement.y = 1.0f;
                    animation_indices = shark_moving[0];
                }
            }

        }
        if (key_state[SDL_SCANCODE_DOWN])
        {
            if (gamemode_status == MULTI_PLAYER)
            {
                if (right_player_pos.y > WALL_DOWN + (INIT_SCALE_PLAYER_R.y / 2.0f))
                {
                    right_player_movement.y = -1.0f;
                    animation_indices = shark_moving[1];
                }
            }
        }
    }
}

void update()
{
	// Single player mode
    if (gamemode_status == SINGLE_PLAYER && win == NONE)
    {
        if (r_move == UP) {
            if (right_player_pos.y < WALL_UP - (INIT_SCALE_PLAYER_R.y / 2.0f))
            {
                right_player_movement.y = 1.0f;
				animation_indices = shark_moving[0];
            }
            else if (right_player_pos.y >= WALL_UP - (INIT_SCALE_PLAYER_R.y / 2.0f)) {
                    r_move = DOWN;                
            }
		}
        else {
            if (right_player_pos.y > WALL_DOWN + (INIT_SCALE_PLAYER_R.y / 2.0f))
            {
                right_player_movement.y = -1.0f;
				animation_indices = shark_moving[1];
            }
            else if (right_player_pos.y <= WALL_DOWN + (INIT_SCALE_PLAYER_R.y / 2.0f)) {
                    r_move = UP;                
            }
        }
    }
    /* DELTA TIME */
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;

    /* ANIMATION */
	animation_time += delta_time;
	float frame_per_second = 1.0f / 6;

	if (animation_time >= frame_per_second)
	{
		animation_time = 0.0f;
		animation_index++;
		if (animation_index >= animation_frame)
		{
			animation_index = 0;
		}
	}

    for (int i = 0; i < num_balls; i++) 
    {
        animation_time_fish[i] += delta_time;
        float frame_per_second_fish = 1.0f / 6;

		if (animation_time_fish[i] >= frame_per_second_fish)
		{
			animation_time_fish[i] = 0.0f;
			animation_index_fish[i]++;
			if (animation_index_fish[i] >= animation_frame_fish[i])
			{
				animation_index_fish[i] = 0;
			}
		}
    }


    /* GAME LOGIC */
    right_player_pos += right_player_movement * SPEED_PLAYER * delta_time;
    left_player_pos += left_player_movement * SPEED_PLAYER * delta_time;
    for (int i = 0; i < num_balls; i++) {
        ball_position[i] += ball_movement[i] * SPEED_BALL * delta_time;
        ball_matrix[i] = glm::mat4(1.0f);
        ball_matrix[i] = glm::translate(ball_matrix[i], ball_position[i]);
        ball_matrix[i] = glm::scale(ball_matrix[i], INIT_SCALE_BALL);
    }

    /* TRANSFORMATIONS */
    right_player_matrix = glm::mat4(1.0f);
	right_player_matrix = glm::translate(right_player_matrix, right_player_pos);
 
	right_player_matrix = glm::scale(right_player_matrix, INIT_SCALE_PLAYER_R);

    left_player_matrix = glm::mat4(1.0f);
    left_player_matrix = glm::translate(left_player_matrix, left_player_pos);
	left_player_matrix = glm::scale(left_player_matrix, INIT_SCALE_PLAYER_L);

	/* COLLISION DETECTION */
    for (int i = 0; i < num_balls; i++) {

        //Win condition
        if (ball_position[i].x >= (WALL_RIGHT - (INIT_SCALE_BALL.x / 2.0f))){
            for (int i = 0; i < num_balls; i++) {
                ball_movement[i] = glm::vec3(0.0f, 0.0f, 0.0f);
				win = LEFT;
            }
        }
		else if (ball_position[i].x <= (WALL_LEFT + (INIT_SCALE_BALL.x / 2.0f))) {
			for (int i = 0; i < num_balls; i++) {
				ball_movement[i] = glm::vec3(0.0f, 0.0f, 0.0f);
				win = RIGHT;
			}
		}
        //Bounce on wall
        else if (ball_position[i].y >= (WALL_UP - (INIT_SCALE_BALL.y / 2.0f))) {
            ball_movement[i].y = -ball_movement[i].y;
        }
        else if (ball_position[i].y <= (WALL_DOWN + (INIT_SCALE_BALL.y / 2.0f))) {
            ball_movement[i].y = -ball_movement[i].y;
        }
        //Bounce on player
		else if (ball_position[i].x + (INIT_SCALE_BALL.x / 2.0f) >= (right_player_pos.x - (INIT_SCALE_PLAYER_R.x / 2.0f)) &&
			ball_position[i].y <= (right_player_pos.y + (INIT_SCALE_PLAYER_R.y / 2.0f)) &&
			ball_position[i].y >= (right_player_pos.y - (INIT_SCALE_PLAYER_R.y / 2.0f))) 
        {
			animation_indices_fish[i] = fish_moving[1];
            ball_movement[i].x = -fabs(ball_movement[i].x);
		}
		else if (ball_position[i].x - (INIT_SCALE_BALL.x / 2.0f) <= (left_player_pos.x + (INIT_SCALE_PLAYER_L.x / 2.0f)) &&
			ball_position[i].y <= (left_player_pos.y + (INIT_SCALE_PLAYER_L.y / 2.0f)) &&
			ball_position[i].y >= (left_player_pos.y - (INIT_SCALE_PLAYER_L.y / 2.0f))) 
        {
			animation_indices_fish[i] = fish_moving[0];
			ball_movement[i].x = fabs(ball_movement[i].x);
		}
    }
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    float texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, GL_FALSE, 0, texCoords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    g_shader_program.set_model_matrix(right_player_matrix);
	draw_sprite_from_texture_atlas(&g_shader_program, right_player_id, animation_indices[animation_index], 2, 6);
    
    g_shader_program.set_model_matrix(left_player_matrix);
	draw_sprite_from_texture_atlas(&g_shader_program, left_player_id, animation_indices_hammer[animation_index], 2, 6);
    
 	for (int i = 0; i < num_balls; i++)
	{
		g_shader_program.set_model_matrix(ball_matrix[i]);
        draw_sprite_from_texture_atlas(&g_shader_program, ball_id, animation_indices_fish[i][animation_index_fish[i]], 2, 2);
	}
    if (num_balls == 0) {
		draw_text(&g_shader_program, g_font_texture_id, "Press 1/2/3", 1.0f, 0.0f, glm::vec3(-5.5f, 4.5f, 0.0f));
    }

	if (win == LEFT && gamemode_status == MULTI_PLAYER) {
		draw_text(&g_shader_program, g_font_texture_id, "Left Player Wins!", 1.0f, 0.0f, glm::vec3(-8.0f, 4.5f, 0.0f));
	}
	else if (win == RIGHT && gamemode_status == MULTI_PLAYER) {
		draw_text(&g_shader_program, g_font_texture_id, "Right Player Wins!", 1.0f, 0.0f, glm::vec3(-8.2f, 4.5f, 0.0f));
	}
	else if (win == LEFT && gamemode_status == SINGLE_PLAYER) {
		draw_text(&g_shader_program, g_font_texture_id, "You Win!", 1.0f, 0.0f, glm::vec3(-3.5f, 4.5f, 0.0f));
	}
    else if (win == RIGHT && gamemode_status == SINGLE_PLAYER) {
        draw_text(&g_shader_program, g_font_texture_id, "You Lose!", 1.0f, 0.0f, glm::vec3(-3.7f, 4.5f, 0.0f));
    }
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