#include "GameplayScreen.h"
#include "SDL/SDL.h"
#include "Light.h"
#include "FlashLight.h"
#include <Bengine/IMainGame.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/Vertex.h>
#include <iostream>
#include <random>
#include <ctime>
#include "ScreenIndices.h"

GameplayScreen::GameplayScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_GAMEPLAY;
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
    return SCREEN_INDEX_MAIN_MENU;
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

    releaseKeys();

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
        randColor.a = 50;
        Box newBox;

        newBox.init(m_world.get(), glm::vec2(xPos(randGenerator), yPos(randGenerator)), glm::vec2(size(randGenerator), size(randGenerator)), m_texture, randColor, true);
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

	// Compile flashlight shader
	m_flashLightProgram.compileShaders("Shaders/flashLightShading.vert", "Shaders/flashLightShading.frag");
	m_flashLightProgram.addAttribute("vertexPosition");
	m_flashLightProgram.addAttribute("vertexColor");
	m_flashLightProgram.addAttribute("vertexUV");
	m_flashLightProgram.linkShaders();

    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setScale(32.0f); ///< Scale out because the world is in meters

    // Init player
    m_player.init(m_world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(2.0f), glm::vec2(1.0f, 1.8f), Bengine::ColorRGBA8(255, 255, 255, 50));

    // Init UI
    //initUI();
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); LINES :D

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

	// If flashlight is on
	if (m_lights) {
		// Send the position of the light
		GLint lightPosUniform = m_textureProgram.getUniformLocation("flashLightPosition");
		glUniform2f(lightPosUniform, glm::vec2(960, 1080).x, glm::vec2(960, 1080).y);

		// Get mouse coordinates
		glm::vec2 mouseCoords = m_game->inputManager.getMouseCoords();
		mouseCoords.y = m_window->getScreenHeight() - mouseCoords.y;

		// Set flashlight direction
		glm::vec2 direction = glm::normalize(
			glm::vec2(
				m_player.getPosition().x + m_window->getScreenWidth() / 32.0f / 2.0f,
				m_player.getPosition().y + m_window->getScreenHeight() / 32.0f / 2.0f
			)
			- glm::vec2(960, 1080) / 32.0f
		);

		// Send the direction of the light
		GLint directionUniform = m_textureProgram.getUniformLocation("flashLightDirection");
		glUniform2f(directionUniform, direction.x, direction.y);

		// Send the color of the light
		GLint colorUniform = m_textureProgram.getUniformLocation("flashLightColor");
		glUniform4f(colorUniform, m_flashLightColor.r, m_flashLightColor.g, m_flashLightColor.b, m_flashLightColor.a);

		// Additive blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	// Tell the shader if the light is on
	GLint colorOnUniform = m_textureProgram.getUniformLocation("flashLightOn");
	glUniform1i(colorOnUniform, m_lights);
		
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

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Render some lights
    // TODO: DONT HARDCODE THIS!!
	/*
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
	*/

	// Render Flashlight
	if (m_lights) {
		FlashLight flashLight;
		flashLight.color = m_flashLightColor;
		flashLight.position = glm::vec2(960, 1080);
		flashLight.size = (float)m_window->getScreenWidth();

		// Get mouse coordinates
		glm::vec2 mouseCoords = m_game->inputManager.getMouseCoords();
		mouseCoords.y = m_window->getScreenHeight() - mouseCoords.y;

		// Set flashlight direction
		flashLight.direction = glm::normalize(
			glm::vec2 (
				m_player.getPosition().x + m_window->getScreenWidth() / 32.0f / 2.0f,
				m_player.getPosition().y + m_window->getScreenHeight() / 32.0f / 2.0f
			)
			- flashLight.position / 32.0f
		);

		m_flashLightProgram.use();

		// Send the camera matrix
		pUniform = m_flashLightProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		// Send the position of the light
		GLint lightPosUniform = m_flashLightProgram.getUniformLocation("lightPosition");
		glUniform2f(lightPosUniform, flashLight.position.x, flashLight.position.y);

		// Send the direction of the light
		GLint directionUniform = m_flashLightProgram.getUniformLocation("dir");
		glUniform2f(directionUniform, flashLight.direction.x, flashLight.direction.y);

		// Additive blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		m_spriteBatch.begin();
		flashLight.draw(m_spriteBatch, m_window->getScreenWidth(), m_window->getScreenHeight());
		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_flashLightProgram.unuse();

		// Reset to regural blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

    // m_gui.draw();

    glEnable(GL_BLEND);
}

void GameplayScreen::releaseKeys()
{
    m_game->inputManager.releaseKey(SDLK_LEFT);
    m_game->inputManager.releaseKey(SDLK_RIGHT);
    m_game->inputManager.releaseKey(SDLK_UP);
    m_game->inputManager.releaseKey(SDLK_a);
    m_game->inputManager.releaseKey(SDLK_w);
    m_game->inputManager.releaseKey(SDLK_d);
	m_game->inputManager.releaseKey(SDLK_s);
    m_game->inputManager.releaseKey(SDLK_LSHIFT);
    m_game->inputManager.releaseKey(SDLK_LCTRL);
    m_game->inputManager.releaseKey(SDLK_SPACE);
}

void GameplayScreen::initUI()
{
    // Init the UI
    m_gui.init("GUI");
    m_gui.loadScheme("TaharezLook.scheme");
    m_gui.loadScheme("AlfiskoSkin.scheme");
    m_gui.setFont("DejaVuSans-10");

    // Create a button
    CEGUI::PushButton* testButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.5f, 0.5f, 0.1f, 0.05f), glm::vec4(0.0f), "TestButton"));
    testButton->setText("Exit Game");

    // Bind event
    testButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::onExitClicked, this));

    CEGUI::Combobox* testCombobox = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.2f, 0.2f, 0.1f, 0.05f), glm::vec4(0.0f), "TestCombobox"));

    m_gui.setMouseCursor("TaharezLook/MouseArrow");
    m_gui.showMouseCursor();
    SDL_ShowCursor(0); ///< Hide regural mouse cursor
}

void GameplayScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
        //m_gui.onSDLEvent(evnt);

        switch (evnt.type) {
        case SDL_QUIT:
            onExitClicked();
            break;

		case SDL_MOUSEWHEEL:
			if (evnt.wheel.y > 0) {
				m_camera.setScale(m_camera.getScale() + 1.0f);
			}
			else if (evnt.wheel.y < 0) {
				m_camera.setScale(m_camera.getScale() - 1.0f);
			}
        }
    }

    if (m_game->inputManager.isKeyPressed(SDLK_LCTRL)) m_renderDebug = !m_renderDebug;
    if (m_game->inputManager.isKeyPressed(SDLK_LSHIFT)) m_lights = !m_lights;
}

void GameplayScreen::onExitClicked()
{
    m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
}
