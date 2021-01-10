#include "engine/lib/glm/glm.hpp"

class Renderer {
    public:
    Renderer init();
    bool compiled;
    void render(uint8_t mesh, glm::vec3 pos, glm::vec3 rot);


    createRenderPasses(); // based on lights, reflections, etc
    queueRenderPasses();

}


class VulkanLayer {
    createInstance()
    pickDevice();
    createLogicalDevice();
}



// choose file format that we like
class ModelLoader {
    void compile();
    uint8_t addMesh();
}


class SoundLoader {

}

