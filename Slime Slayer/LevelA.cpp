

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 30
#define ENEMY_COUNT 3

bool goal_appear = false;
enum play_goal {No, Play, Off};
play_goal goal_sound = No;
play_goal death_sound = No;

constexpr char SPRITESHEET_FILEPATH[] = "assets/player.png",
ENEMY0_FILEPATH[] = "assets/green.png",
ENEMY1_FILEPATH[] = "assets/blue.png",
ENEMY2_FILEPATH[] = "assets/yellow.png",
GOAL_FILEPATH[] = "assets/goal.png",
FONT_FILEPATH[] = "assets/font1.png";

GLuint font_texture_id;

unsigned int LEVEL_A_DATA[] =
{
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
11, 12, 12, 12, 7, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 42, 12, 12, 12, 42, 12, 12, 7, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 12, 12, 42, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 9, 12, 12, 12, 12, 12, 12, 12, 21, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 12, 12, 26,
11, 12, 42, 12, 12, 12, 12, 20, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 42, 7, 12, 10, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 21, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 5, 34, 34, 37, 12, 33, 34, 34, 6, 12, 12, 15, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 15, 12, 12, 12, 12, 12, 12, 12, 11, 12, 12, 16, 1, 1, 1, 1, 2, 12, 12, 12, 12, 12, 26,
11, 12, 12, 11, 12, 7, 12, 15, 12, 12, 12, 10, 12, 12, 12, 11, 12, 12, 12, 12, 12, 12, 12, 15, 12, 12, 7, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 15, 12, 12, 12, 21, 12, 12, 12, 11, 12, 12, 12, 12, 27, 28, 12, 16, 1, 1, 4, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 15, 12, 12, 12, 12, 12, 12, 12, 11, 12, 12, 12, 12, 38, 39, 12, 12, 12, 12, 15, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 16, 1, 1, 1, 1, 1, 1, 1, 17, 12, 7, 12, 12, 12, 12, 12, 12, 12, 12, 15, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 15, 12, 12, 26,
11, 12, 12, 11, 12, 12, 42, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 42, 12, 12, 12, 15, 12, 12, 26,
11, 12, 12, 11, 12, 12, 12, 12, 12, 12, 7, 12, 12, 12, 12, 12, 12, 12, 9, 12, 12, 12, 12, 12, 12, 12, 15, 12, 12, 26,
11, 12, 12, 22, 12, 42, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 20, 12, 12, 12, 12, 32, 12, 12, 15, 12, 12, 26,
11, 12, 12, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 6, 12, 12, 12, 12, 12, 12, 12, 43, 12, 12, 15, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 40, 12, 12, 12, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 15, 42, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 11, 12, 12, 12, 7, 12, 12, 12, 12, 12, 12, 15, 12, 12, 26,
11, 12, 12, 12, 12, 5, 6, 12, 12, 42, 12, 12, 12, 12, 12, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 15, 12, 12, 26,
11, 12, 7, 12, 12, 16, 17, 12, 12, 12, 12, 12, 12, 12, 12, 11, 12, 12, 12, 42, 12, 12, 12, 12, 7, 12, 15, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 42, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 42, 12, 12, 12, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 42, 9, 12, 12, 12, 12, 12, 7, 12, 12, 12, 32, 12, 12, 12, 12, 12, 5, 6, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 12, 20, 12, 12, 12, 12, 12, 12, 12, 12, 12, 43, 12, 12, 12, 12, 12, 16, 17, 12, 12, 12, 12, 12, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 42, 12, 12, 12, 12, 12, 12, 42, 12, 12, 12, 26,
11, 12, 42, 12, 12, 7, 12, 12, 12, 12, 42, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 12, 26,
11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 12, 12, 12, 12, 12, 12, 12, 12, 12, 26,
33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 37
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.attack_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/taiga.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_DATA, map_texture_id, 1.0f, 11, 4);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int moving_animation[4][4] =
    {
        { 0, 4, 8, 12 },  //  left,
        { 2, 6, 10, 14 }, //  right,
        { 3, 7, 11, 15 }, // die while looking right - UP
        { 1, 5, 9, 13 }   // die while looking left - DOWN
    };

    
	int goal_animation[1][6] =
	{
		{ 0, 1, 2, 3, 4, 5 }
	};

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        moving_animation,          // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.5f,                      // width
        0.5f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(2.0f, -2.0f, 0.1f));
    
    /**
     Enemies' stuff */
    GLuint enemy0_texture_id = Utility::load_texture(ENEMY0_FILEPATH);
    GLuint enemy1_texture_id = Utility::load_texture(ENEMY1_FILEPATH);
    GLuint enemy2_texture_id = Utility::load_texture(ENEMY2_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		m_game_state.enemies[i] = Entity(
            enemy1_texture_id,         // texture id
            3.0f,                      // speed
            acceleration,              // acceleration
            0.0f,                      // jumping power
            moving_animation,          // animation index sets
            0.0f,                      // animation time
            4,                         // animation frame amount
            0,                         // current animation index
            4,                         // animation column amount
            4,                         // animation row amount
            1.0f,                      // width
            1.0f,                     // height
            ENEMY
        );
	}

	

	m_game_state.enemies[0].set_texture_id(enemy1_texture_id);
    m_game_state.enemies[0].set_ai_type(RUNAWAY);
	m_game_state.enemies[0].set_ai_state(IDLE);
	m_game_state.enemies[0].set_position(glm::vec3(18.0f, -5.0f, 0.0f));
	m_game_state.enemies[0].set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
	//m_game_state.enemies[0].set_left_boundary(14.0f);
	//m_game_state.enemies[0].set_right_boundary(19.0f);

	m_game_state.enemies[1].set_texture_id(enemy2_texture_id);
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -10.0f, 0.0f));
    m_game_state.enemies[1].set_ai_type(WALKER);
	m_game_state.enemies[1].set_ai_state(WALKING);
	m_game_state.enemies[1].set_position(glm::vec3(20.0f, -18.0f, 0.0f));
	m_game_state.enemies[1].set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
	m_game_state.enemies[1].set_left_boundary(17.0f);
	m_game_state.enemies[1].set_right_boundary(24.0f);

    m_game_state.enemies[2].set_texture_id(enemy0_texture_id);
    m_game_state.enemies[2].set_ai_type(GUARD_V);
    m_game_state.enemies[2].set_ai_state(IDLE);
    m_game_state.enemies[2].set_position(glm::vec3(10.0f, -15.0f, 0.0f));
	m_game_state.enemies[2].set_upper_boundary(-13.0f);
	m_game_state.enemies[2].set_lower_boundary(-16.0f);
    m_game_state.enemies[2].set_scale(glm::vec3(1.0f, 1.0f, 1.0f));

    /**
    GOAL
    */
	GLuint goal_texture_id = Utility::load_texture(GOAL_FILEPATH);
    m_game_state.goal = new Entity(
        goal_texture_id,           // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        moving_animation,          // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.9f,                      // width
        0.64f,                     // height
        GOAL
    );
	m_game_state.goal->set_position(glm::vec3(25.0f, -21.0f, 0.0f));
    m_game_state.goal->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
	m_game_state.goal->set_scale(glm::vec3(1.5f, 1.5f, 1.0f));
    
	/**
    SFX
	*/
    m_game_state.attack_sfx = Mix_LoadWAV("assets/attack.wav"); 
	Mix_VolumeChunk(m_game_state.attack_sfx, 30);
	m_game_state.goal_sfx = Mix_LoadWAV("assets/goal.wav");
	Mix_VolumeChunk(m_game_state.goal_sfx, 30);
	m_game_state.death_sfx = Mix_LoadWAV("assets/death.wav");
	Mix_VolumeChunk(m_game_state.death_sfx, 60);
	font_texture_id = Utility::load_texture(FONT_FILEPATH);   
}

void LevelA::update(float delta_time)
{
	int enemies_alive = ENEMY_COUNT;
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
	m_game_state.goal->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
        
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
        if (!m_game_state.enemies[i].get_is_active())
        {
            enemies_alive--;
        }
    }
	if (enemies_alive == 0)
	{
		goal_appear = true;
		if (goal_sound == No)
		{
			Mix_PlayChannel(-1, m_game_state.goal_sfx, 0);
			goal_sound = Play;
		}
	}
    if (goal_sound == Play)
    {
        Mix_PlayChannel(-1, m_game_state.goal_sfx, 0);
        goal_sound = Off;
    }
	if (m_game_state.player->get_life() == 0)
	{
		if (death_sound == No)
		{
			Mix_PlayChannel(-1, m_game_state.death_sfx, 0);
			death_sound = Play;
			death_sound = Off;
		}
	}
}


void LevelA::render(ShaderProgram *g_shader_program, glm::mat4 g_view_matrix)
{

    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)   
            m_game_state.enemies[i].render(g_shader_program);
    if (goal_appear)
    {
        m_game_state.goal->render(g_shader_program);
		m_game_state.goal->activate();
    }
    else m_game_state.goal->deactivate();
    glm::vec3 cam_position = -glm::vec3(g_view_matrix[3]);
	Utility::draw_text(g_shader_program, font_texture_id, "Stage 1", 0.5f, -0.2f, cam_position + glm::vec3(-9.5f, 7.0f, 0.0f));
    if (m_game_state.player->get_life() == 3)
    {
        Utility::draw_text(g_shader_program, font_texture_id, "Life: 3", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
    }
	else if (m_game_state.player->get_life() == 2)
	{
		Utility::draw_text(g_shader_program, font_texture_id, "Life: 2", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
	}
	else if (m_game_state.player->get_life() == 1)
	{
		Utility::draw_text(g_shader_program, font_texture_id, "Life: 1", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
	}
	else if (m_game_state.player->get_life() == 0)
	{
		Utility::draw_text(g_shader_program, font_texture_id, "Life: 0", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
        Utility::draw_text(g_shader_program, font_texture_id, "You lose!", 0.5f, 0.0f,
            glm::vec3(m_game_state.player->get_position().x - 2.0f, m_game_state.player->get_position().y + 1.0f, 0.0f));
	}
}
