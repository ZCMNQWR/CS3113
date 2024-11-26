/**
* Author: [Steven Ha]
* Assignment: Platformer
* Date due: 2024-11-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"

constexpr char
FONT_FILEPATH[] = "assets/font1.png";

GLuint menu_font_texture_id;

Menu::~Menu()
{
	Mix_FreeMusic(m_game_state.bgm);
}

void Menu::initialise()
{
	menu_font_texture_id = Utility::load_texture(FONT_FILEPATH);

}

void Menu::update(float delta_time)
{
	
}

void Menu::render(ShaderProgram* g_shader_program, glm::mat4 view_matrix)
{
	Utility::draw_text(g_shader_program, menu_font_texture_id, "Arrows to move", 0.5f, -0.2f, glm::vec3(-2.0f, 4.0f, 0.0f));
	Utility::draw_text(g_shader_program, menu_font_texture_id, "Space to jump", 0.5f, -0.2f, glm::vec3(-1.9f, 3.0f, 0.0f));
	Utility::draw_text(g_shader_program, menu_font_texture_id, "Press q to quit", 0.5f, -0.2f, glm::vec3(-2.1f, 2.0f, 0.0f));
	Utility::draw_text(g_shader_program, menu_font_texture_id, "Avoid Slimes and get to the goal!", 0.5f, -0.2f, glm::vec3(-5.0f, 0.0f, 0.0f));
	Utility::draw_text(g_shader_program, menu_font_texture_id, "Press Enter to start", 0.5f, -0.2f, glm::vec3(-2.9f, -2.0f, 0.0f));
}