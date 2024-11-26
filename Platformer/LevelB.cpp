/**
* Author: [Steven Ha]
* Assignment: Platformer
* Date due: 2024-11-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 10
#define ENEMY_COUNT 3

constexpr char SPRITESHEET_FILEPATH[] = "assets/player.png",
PLATFORM_FILEPATH[] = "assets/tileset.png",
ENEMY0_FILEPATH[] = "assets/green.png",
ENEMY1_FILEPATH[] = "assets/blue.png",
ENEMY2_FILEPATH[] = "assets/yellow.png",
FONT_FILEPATH[] = "assets/font1.png",
GOAL_FILEPATH[] = "assets/goal.png";

GLuint font_texture_id_b;

unsigned int LEVEL_B_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 2, 2, 1, 1, 1, 0, 0, 2, 2, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 4, 1);

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

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        moving_animation,          // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.5f,                      // width
        0.9f,                      // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(0.0f, -5.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 0.75f, 1.0f));

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
            5.0f,                      // jumping power
            moving_animation,         // animation index sets
            0.0f,                      // animation time
            4,                         // animation frame amount
            0,                         // current animation index
            4,                         // animation column amount
            4,                         // animation row amount
            0.9f,                      // width
            0.64f,                     // height
            ENEMY
        );
    }

    m_game_state.enemies[0].set_texture_id(enemy1_texture_id);
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(6.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(1.0f, 0.75f, 1.0f));
    m_game_state.enemies[0].set_left_boundary(6.0f);
    m_game_state.enemies[0].set_right_boundary(8.0f);

    m_game_state.enemies[1].set_texture_id(enemy2_texture_id);
    m_game_state.enemies[1].set_ai_type(WALKER);
    m_game_state.enemies[1].set_ai_state(WALKING);
    m_game_state.enemies[1].set_position(glm::vec3(19.0f, -2.0f, 0.0f));
    m_game_state.enemies[1].set_scale(glm::vec3(1.0f, 0.75f, 1.0f));
    m_game_state.enemies[1].set_left_boundary(18.0f);
    m_game_state.enemies[1].set_right_boundary(24.0f);

    m_game_state.enemies[2].set_texture_id(enemy0_texture_id);
    m_game_state.enemies[2].set_ai_type(JUMPER);
    m_game_state.enemies[2].set_ai_state(IDLE);
    m_game_state.enemies[2].set_position(glm::vec3(11.0f, -4.0f, 0.0f));
    m_game_state.enemies[2].set_scale(glm::vec3(1.0f, 0.75f, 1.0f));

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
        0.5f,                      // width
        1.0f,                      // height
        GOAL
    );
    m_game_state.goal->set_position(glm::vec3(26.0f, -5.0f, 0.0f));
    m_game_state.goal->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.goal->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));

    /**
    SFX
    */
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav"); // sound by dklon 
    Mix_VolumeChunk(m_game_state.jump_sfx, 10);

    font_texture_id_b = Utility::load_texture(FONT_FILEPATH);
    }

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.goal->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

    if (m_game_state.player->get_position().y < -20.0f)
    {
        m_game_state.next_scene_id = 2;
    }
}


void LevelB::render(ShaderProgram* g_shader_program, glm::mat4 g_view_matrix)
{

    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
    m_game_state.goal->render(g_shader_program);
    glm::vec3 cam_position = -glm::vec3(g_view_matrix[3]);
	Utility::draw_text(g_shader_program, font_texture_id_b, "Stage 2", 0.5f, -0.2f, cam_position + glm::vec3(-9.5f, 7.0f, 0.0f));
    if (m_game_state.player->get_life() == 3)
    {
        Utility::draw_text(g_shader_program, font_texture_id_b, "Life: 3", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
    }
    else if (m_game_state.player->get_life() == 2)
    {
        Utility::draw_text(g_shader_program, font_texture_id_b, "Life: 2", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
    }
    else if (m_game_state.player->get_life() == 1)
    {
        Utility::draw_text(g_shader_program, font_texture_id_b, "Life: 1", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
    }
    else if (m_game_state.player->get_life() == 0)
    {
        Utility::draw_text(g_shader_program, font_texture_id_b, "Life: 0", 0.5f, -0.2f, cam_position + glm::vec3(-5.5f, 7.0f, 0.0f));
        Utility::draw_text(g_shader_program, font_texture_id_b, "You lose!", 0.5f, 0.0f,
            glm::vec3(m_game_state.player->get_position().x - 2.0f, m_game_state.player->get_position().y + 1.0f, 0.0f));
    }
}
