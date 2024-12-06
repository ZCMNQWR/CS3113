

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;
        case GUARD_V:
            ai_guard_v(player);
            break;
            
        default:
            break;
    }
}

void Entity::ai_walk()
{
    if (m_position.x < m_left_boundary && direction == LEFT) {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        face_right();
        direction = RIGHT;
    }
    else if (m_position.x > m_right_boundary && direction == RIGHT) {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        face_left();
        direction = LEFT;
    }
    else if (m_position.x > m_left_boundary && direction == LEFT)
    {
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        face_left();
        direction = LEFT;
    }
    else if (m_position.x < m_right_boundary && direction == RIGHT)
    {
        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        face_right();
        direction = RIGHT;

    }
}

void Entity::ai_guard(Entity *player)
{
    switch (m_ai_state) {
    case IDLE:
        if (m_position.x > player->get_position().x)
        {
            face_left();
            direction = LEFT;
        }
        else
        {
            face_right();
            direction = RIGHT;
        }
        if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = WALKING;;
        break;

    case WALKING:
        if (m_position.x > player->get_position().x) {
            face_left();
            if (m_position.x >= m_left_boundary && m_position.x <= (m_right_boundary + 1.0f)) m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            else m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        else if (m_position.x < player->get_position().x) {
            face_right();
            if (m_position.x >= (m_left_boundary - 1.0f) && m_position.x <= m_right_boundary) m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            else m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (glm::distance(m_position, player->get_position()) > 10.0f)
        {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            m_ai_state = IDLE;
        }
        break;
    default:
        break;
    }
}

void Entity::ai_guard_v(Entity* player)
{
    switch (m_ai_state) {
    case IDLE:
        if (m_position.x > player->get_position().x)
        {
            face_left();
            direction = LEFT;
        }
        else
        {
            face_right();
            direction = RIGHT;
        }
        if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = WALKING;;
        break;

    case WALKING:
        if (m_position.x > player->get_position().x) face_left();
        else if (m_position.x < player->get_position().x) face_right();

		if (m_position.y < player->get_position().y)
        {
            if (m_position.y <= m_upper_boundary && m_position.y >= (m_lower_boundary - 1.0f)) m_movement = glm::vec3(0.0f, 1.0f, 0.0f);
            else m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        }
		else if (m_position.y > player->get_position().y)
        {
            if (m_position.y <= (m_upper_boundary + 1.0f) && m_position.y >= m_lower_boundary) m_movement = glm::vec3(0.0f, -1.0f, 0.0f);
            else m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (glm::distance(m_position, player->get_position()) > 10.0f)
        {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            m_ai_state = IDLE;
        }
        break;
    default:
        break;
    }
}

void Entity::ai_jump(Entity* player)
{
    if ((m_position.x > player->get_position().x))
    {
        face_left();
    }
    else
    {
        face_right();
    }
    switch (m_ai_state)
    {
    case IDLE:
        if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = JUMPING;
        break;
    case JUMPING:
        if (glm::distance(m_position, player->get_position()) > 5.0f)
        {
            m_ai_state = IDLE;
        }
        else if (get_velocity().y == 0.0f) {
            jump();
            break;
        }
    default:
        break;
    }
}

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_moving[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed),m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    face_right();
    set_walking(walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed,  float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_moving[i][j] = 0;
}
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState): m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
// Initialize m_walking with zeros or any default value
for (int i = 0; i < SECONDS_PER_FRAME; ++i)
    for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_moving[i][j] = 0;
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
	if (m_animation_cols == 0 || m_animation_rows == 0) return;
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::check_kill(Entity* other, int collidable_entity_count) // check death of the other
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        float left_bound = m_position.x - m_width / 2.0f;
        float right_bound = m_position.x + m_width / 2.0f;
        float upper_bound = m_position.y + m_height / 2.0f;
        float lower_bound = m_position.y - m_height / 2.0f;

        float other_left_bound = other[i].get_position().x - (other[i].get_width() / 2.0f);
        float other_right_bound = other[i].get_position().x + (other[i].get_width() / 2.0f);
        float other_upper_bound = other[i].get_position().y + (other[i].get_height() / 2.0f);
        float other_lower_bound = other[i].get_position().y - (other[i].get_height() / 2.0f);

        if (m_entity_type == PLAYER)
        {
            // enemy hits player's head
            if ((upper_bound + 0.5f > (other_lower_bound + 0.1f) && upper_bound < (other_lower_bound + 0.2f))
                && lower_bound < other_lower_bound &&
                ((left_bound < other_right_bound && left_bound > other_left_bound) ||
                    (right_bound > other_left_bound && right_bound < other_right_bound)) && other[i].get_is_active())
            {
				other[i].face_down();
                other[i].deactivate();
				other[i].set_height(-1.0f);
				other[i].set_width(-1.0f);
            }
            // enemy hits player's right side
            else if
                (other_right_bound + 0.7f > left_bound && other_right_bound < right_bound &&
                    other_left_bound < left_bound && other_upper_bound > lower_bound &&
                    other_lower_bound < upper_bound &&  other[i].get_is_active())
            {
                other[i].face_down();
                other[i].deactivate();
                other[i].set_height(-1.0f);
                other[i].set_width(-1.0f);
            }
            // enemy hits player's left side
            else if
                (other_left_bound -0.7f < right_bound && other_left_bound > left_bound &&
                    other_right_bound > right_bound && other_upper_bound > lower_bound &&
                    other_lower_bound < upper_bound && other[i].get_is_active())
            {
                other[i].face_down();
                other[i].deactivate();
                other[i].set_height(-1.0f);
                other[i].set_width(-1.0f);

            }
            // enemy hits player's feet
            else if (lower_bound -0.5f < (other_upper_bound - 0.1f) && (lower_bound > other_upper_bound - 0.2f)
                && upper_bound > other_upper_bound &&
                ((left_bound < other_right_bound && left_bound > other_left_bound) ||
                    (right_bound > other_left_bound && right_bound < other_right_bound)) && other[i].get_is_active())
            {
                other[i].face_down();
                other[i].deactivate();
                other[i].set_height(-1.0f);
                other[i].set_width(-1.0f);
            }
        }
    }
}

void Entity::check_death(Entity* other, int collidable_entity_count) 
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        float left_bound = m_position.x - m_width / 2.0f;
        float right_bound = m_position.x + m_width / 2.0f;
        float upper_bound = m_position.y + m_height / 2.0f;
        float lower_bound = m_position.y - m_height / 2.0f;

        float other_left_bound = other[i].get_position().x - (other[i].get_width() / 2.0f);
        float other_right_bound = other[i].get_position().x + (other[i].get_width() / 2.0f);
        float other_upper_bound = other[i].get_position().y + (other[i].get_height() / 2.0f);
        float other_lower_bound = other[i].get_position().y - (other[i].get_height() / 2.0f);
		float half_height = m_height / 2.0f;

        if (m_entity_type == PLAYER)
        {
			// enemy hits player's head
            if ((upper_bound > (other_lower_bound + 0.1f) && upper_bound < (other_lower_bound + 0.2f))
                && lower_bound < other_lower_bound &&
                ((left_bound < other_right_bound && left_bound > other_left_bound) ||
                    (right_bound > other_left_bound && right_bound < other_right_bound)) && other[i].get_is_active())
            {
                if (direction == LEFT) face_down();
                else if (direction == RIGHT) face_up();
                decrease_life();
                set_is_hit(true);
                if (life > 0) set_position(glm::vec3(2.0f, -2.0f, 0.1f));
            }
            // enemy hits player's right side
            else if
                (other_right_bound > left_bound && other_right_bound < right_bound &&
                    other_left_bound < left_bound && other_upper_bound > lower_bound &&
                    other_lower_bound < upper_bound && other[i].get_is_active())
            {
                if (direction == LEFT) face_down();
                else if (direction == RIGHT) face_up();
                decrease_life();
                set_is_hit(true);
                if (life > 0) set_position(glm::vec3(2.0f, -2.0f, 0.1f));
			}
			// enemy hits player's left side
			else if
				(other_left_bound < right_bound && other_left_bound > left_bound &&
					other_right_bound > right_bound && other_upper_bound > lower_bound &&
					other_lower_bound < upper_bound && other[i].get_is_active())
			{
				if (direction == LEFT) face_down();
				else if (direction == RIGHT) face_up();
                decrease_life();
                set_is_hit(true);
                if (life > 0) set_position(glm::vec3(2.0f, -2.0f, 0.1f));
                
			}
			// enemy hits player's feet
            else if (lower_bound < (other_upper_bound - 0.1f) && (lower_bound > other_upper_bound - 0.2f)
                && upper_bound > other_upper_bound &&
                ((left_bound < other_right_bound && left_bound > other_left_bound) ||
                    (right_bound > other_left_bound && right_bound < other_right_bound)) && other[i].get_is_active())
            {
                if (direction == LEFT) face_down();
                else if (direction == RIGHT) face_up();
                decrease_life();
                set_is_hit(true);
                if(life > 0) set_position(glm::vec3(2.0f, -2.0f, 0.1f));
            }
        }
    }
}

void Entity::check_goal(Entity* other)
{
	float left_bound = m_position.x - m_width / 4.0f;
	float right_bound = m_position.x + m_width / 4.0f;
	float upper_bound = m_position.y + m_height / 4.0f;
	float lower_bound = m_position.y - m_height / 4.0f;

	float other_left_bound = other->get_position().x - (other->get_width() / 8.0f);
	float other_right_bound = other->get_position().x + (other->get_width() / 8.0f);
	float other_upper_bound = other->get_position().y + (other->get_height() / 8.0f);
	float other_lower_bound = other->get_position().y - (other->get_height() / 8.0f);

	if (lower_bound < (other_upper_bound - 0.1f) && (lower_bound > other_upper_bound - 0.2f)
		&& upper_bound > other_upper_bound &&
		((left_bound < other_right_bound && left_bound > other_left_bound) ||
			(right_bound > other_left_bound && right_bound < other_right_bound)) &&
		m_velocity.y < 0.0f)
	{
		reached_goal();
	}
	else if (other_right_bound > left_bound && other_right_bound < right_bound &&
		other_left_bound < left_bound && other_upper_bound > lower_bound &&
		other_lower_bound < upper_bound && m_velocity.y >= 0.0f)
	{
		reached_goal();
	}
	else if (other_left_bound < right_bound && other_left_bound > left_bound &&
		other_right_bound > right_bound && other_upper_bound > lower_bound &&
		other_lower_bound < upper_bound && m_velocity.y >= 0.0f)
	{
		reached_goal();
	}
	else if (lower_bound < (other_upper_bound - 0.1f) && (lower_bound > other_upper_bound - 0.2f)
		&& upper_bound > other_upper_bound &&
		((left_bound < other_right_bound && left_bound > other_left_bound) ||
			(right_bound > other_left_bound && right_bound < other_right_bound)) &&
		m_velocity.y < 0.0f)
	{
		reached_goal();
	}
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                //m_position.y   -= y_overlap;
                m_velocity.y    = 0;

                // Collision!
                m_collided_top  = true;
            } else if (m_velocity.y < 0)
            {
                //m_position.y      += y_overlap;
                m_velocity.y       = 0;

                // Collision!
                m_collided_bottom  = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                //m_position.x     -= x_overlap;
                m_velocity.x      = 0;

                // Collision!
                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                //m_position.x    += x_overlap;
                m_velocity.x     = 0;
 
                // Collision!
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void const Entity::set_attack(bool new_attack) {
    m_attack = new_attack;
    if (m_attack) {
        set_scale(glm::vec3(2.0f, 2.0f, 1.0f)); // Scale up during attack
    }
    else {
        set_scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Reset scale after attack
    }
}

void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{

	if (m_entity_type == PLAYER && life == 0) deactivate();
 
    if (m_animation_indices != NULL)
    {
        //if (glm::length(m_movement) != 0) // If the entity is moving
		
        if (((m_animation_indices == m_moving[DOWN] && m_animation_index == 3) || (m_animation_indices == m_moving[UP] && m_animation_index == 3)) && get_entity_type() == ENEMY) return;
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second)
        {
            m_animation_time = 0.0f;
            m_animation_index++;

            if (m_animation_index >= m_animation_frames)
            {
                m_animation_index = 0;
            }
        }


    }
   

    if (!m_is_active)
    {
        return;
    }

    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    if (m_entity_type == ENEMY) ai_activate(player);
    
    if (m_animation_indices != NULL)
    {
        //if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
	m_velocity.y = m_movement.y * m_speed;
    m_velocity += m_acceleration * delta_time;    
    
    
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
	m_position.y += m_velocity.y * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
	check_death(collidable_entities, collidable_entity_count);
	if (life == 0) deactivate();

	check_goal(player);

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    if (m_animation_index == 0 && !m_attack) set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
	m_model_matrix = glm::scale(m_model_matrix, m_scale); 
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);


    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());

    set_is_hit(false);
}