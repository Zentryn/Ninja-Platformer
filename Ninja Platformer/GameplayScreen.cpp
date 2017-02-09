#include "GameplayScreen.h"
#include "SDL/SDL.h"
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
    // Same gravity as earth
    b2Vec2 gravity(0.0f, -9.81);
    // Make the world
    m_world = std::make_unique<b2World>(gravity);

    // Make the ground
    b2BodyDef groundBodyDef;
    // Create ground body definition
    groundBodyDef.position.Set(0.0f, -25.0f);
    // Create the ground body
    b2Body* groundBody = m_world->CreateBody(&groundBodyDef);
    // Make the ground fixture
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Make a bunch of boxes
    std::mt19937 randGenerator;
    std::uniform_real_distribution<float> xPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> yPos(-10.0f, 15.0f);
    std::uniform_real_distribution<float> size(1.0f, 2.5f);
    std::uniform_int_distribution<int> colr(0, 255);

    const int NUM_BOXES = 30;

    for (size_t i = 0; i < NUM_BOXES; i++) {
        Bengine::ColorRGBA8 randColor;
        randColor.r = colr(randGenerator);
        randColor.g = colr(randGenerator);
        randColor.b = colr(randGenerator);
        randColor.a = 255;
        Box newBox;

        float s = size(randGenerator);
        newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(s, s), randColor);
        m_boxes.push_back(newBox);
    }

    // Initialize sprite batch
    m_spriteBatch.init();

    // Shader init
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();

    // Load the texture
    m_texture = Bengine::ResourceManager::getTexture("Assets/bricks_top.png");

    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setScale(32.0f); ///< Scale out because the world is in meters
}

void GameplayScreen::onExit()
{
    std::cout << "On Exit\n";
}

void GameplayScreen::update()
{
    m_camera.update();
    checkInput();

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
        glm::vec4 destRect(
            box.getBody()->GetPosition().x - box.getDimensions().x / 2.0f,
            box.getBody()->GetPosition().y - box.getDimensions().y / 2.0f,
            box.getDimensions()
        );
        const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

        m_spriteBatch.draw(
            destRect,
            uvRect,
            m_texture.id,
            0.0f,
            box.getColor(),
            box.getBody()->GetAngle()
        );
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
    m_textureProgram.unuse();
}


void GameplayScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
    }
}
