

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
