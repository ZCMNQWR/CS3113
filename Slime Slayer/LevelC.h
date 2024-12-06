

#include "Scene.h"

class LevelC : public Scene {
public:
    // ————— DESTRUCTOR ————— //
    ~LevelC();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, glm::mat4 view_matrix);
};
#pragma once
