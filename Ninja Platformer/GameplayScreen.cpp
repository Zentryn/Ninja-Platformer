#include "GameplayScreen.h"
#include "SDL/SDL.h"
#include "Light.h"
#include <Bengine/IMainGame.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <iostream>
#include <random>
#include <ctime>

GameplayScreen::GameplayScreen(Bengine::Window* window) :
    m_window(window)
{
}


GameplayScreen::~GameplayScreen()
{
}

int GameplayScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

int GameplayScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

void GameplayScreen::build()
{
    std::cout << "Build\n";
}

void GameplayScreen::destroy()
{
    std::cout << "Destroy\n";
}

void GameplayScreen::onEntry()
{
    m_debugRenderer.init();

    // Same gravity as earth
    b2Vec2 gravity(0.0f, -34.0f);
    // Make the world
    m_world = std::make_unique<b2World>(gravity);

    // Make the ground
    b2BodyDef groundBodyDef;
    // Create ground body definition
    groundBodyDef.position.Set(0.0f, -26.86f);
    // Create the ground body
    b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
    // Make the ground fixture
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Load the texture
    m_texture = Bengine::ResourceManager::getTexture("Assets/bricks_top.png");

    // Make a bunch of boxes
    std::mt19937 randGenerator((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> xPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> yPos(-10.0f, 15.0f);
    std::uniform_real_distribution<float> size(1.0f, 2.5f);
    std::uniform_int_distribution<int> colr(0, 255);

    const int NUM_BOXES = 15;

    for (size_t i = 0; i < NUM_BOXES; i++) {
        Bengine::ColorRGBA8 randColor;
        randColor.r = colr(randGenerator);
        randColor.g = colr(randGenerator);
        randColor.b = colr(randGenerator);
        randColor.a = 255;
        Box newBox;

        newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(size(randGenerator), size(randGenerator)), m_texture, randColor);
        m_boxes.push_back(newBox);
    }

    // Initialize sprite batch
    m_spriteBatch.init();

    // Shader init
    // Compile texture shader
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();

    // Compile light shader
    m_lightProgram.compileShaders("Shaders/lightShading.vert", "Shaders/lightShading.frag");
    m_lightProgram.addAttribute("vertexPosition");
    m_lightProgram.addAttribute("vertexColor");
    m_lightProgram.addAttribute("vertexUV");
    m_lightProgram.linkShaders();

    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setScale(32.0f); ///< Scale out because the world is in meters

    // Init player
    m_player.init(m_world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(2.0f), glm::vec2(1.0f, 1.8f), Bengine::ColorRGBA8(255, 255, 255, 255));

    
}

void GameplayScreen::onExit()
{
    m_debugRenderer.dispose();
}

void GameplayScreen::update()
{
    m_camera.update();
    checkInput();
    m_player.update(m_game->inputManager);

    // Update the physics simulation
    m_world->Step(1.0f / 144.0f, 6, 2);
}

void GameplayScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    m_textureProgram.use();
    m_spriteBatch.begin();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Draw all the boxes
    for (auto& box : m_boxes) {
        box.draw(m_spriteBatch);
    }

    // Draw the player
    m_player.draw(m_spriteBatch);

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();

    // Debug rendering
    if (m_renderDebug) {
        Bengine::ColorRGBA8 color(255, 255, 255, 255);

        // Draw collision boxes for boxes
        for (auto& box : m_boxes) {
            glm::vec4 destRect(
                box.getBody()->GetPosition().x - box.getDimensions().x / 2.0f,
                box.getBody()->GetPosition().y - box.getDimensions().y / 2.0f,
                box.getDimensions().x, box.getDimensions().y
            );

            m_debugRenderer.drawBox(
                destRect,
                color,
                box.getBody()->GetAngle()
            );
        }

        m_player.drawDebug(m_debugRenderer);
        
        m_debugRenderer.end();
        m_debugRenderer.render(projectionMatrix, 2.0f);
    }

    // Render some lights
    // TODO: DONT HARDCODE THIS!!
    if (m_lights) {
        Light playerLight;
        playerLight.color = Bengine::ColorRGBA8(50, 50, 255, 128);
        playerLight.position = m_player.getPosition();
        playerLight.size = 25.0f;

        Light mouseLight;
        mouseLight.color = Bengine::ColorRGBA8(255, 0, 255, 80);
        mouseLight.position = m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords());
        mouseLight.size = 45.0f;

        m_lightProgram.use();
        pUniform = m_lightProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        // Additive blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        m_spriteBatch.begin();
        playerLight.draw(m_spriteBatch);
        mouseLight.draw(m_spriteBatch);
        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_lightProgram.unuse();

        // Reset to regural blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}


void GameplayScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
    }

    if (m_game->inputManager.isKeyPressed(SDLK_LCTRL)) m_renderDebug = !m_renderDebug;
    if (m_game->inputManager.isKeyPressed(SDLK_LSHIFT)) m_lights = !m_lights;
}
