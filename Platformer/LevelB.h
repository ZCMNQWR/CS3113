﻿/**
* Author: [Steven Ha]
* Assignment: Platformer
* Date due: 2024-11-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, glm::mat4 view_matrix);
};