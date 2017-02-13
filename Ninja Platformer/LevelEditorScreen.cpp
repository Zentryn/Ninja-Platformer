#include "LevelEditorScreen.h"
#include <Bengine/ResourceManager.h>

LevelEditorScreen::LevelEditorScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_EDITOR;
}

LevelEditorScreen::~LevelEditorScreen()
{
}

int LevelEditorScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

int LevelEditorScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAIN_MENU;
}

void LevelEditorScreen::build()
{

}

void LevelEditorScreen::destroy()
{

}

void LevelEditorScreen::onEntry()
{
    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init UI
    initUI();

    // Init sprite batch
    m_spriteBatch.init();

    // Init shaders
    initShaders();

    m_blankTexture = Bengine::ResourceManager::getTexture("Assets/blank.png");

    m_spriteFont = std::make_unique<Bengine::SpriteFont>("Fonts/chintzy.ttf", 32);
}

void LevelEditorScreen::onExit()
{
    m_gui.destroy();
    m_textureProgram.dispose();
}

void LevelEditorScreen::update()
{
    m_camera.update();
    checkInput();
}

void LevelEditorScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void LevelEditorScreen::initUI()
{
    // Init the UI
    m_gui.init("GUI");
    m_gui.loadScheme("TaharezLook.scheme");
    m_gui.loadScheme("AlfiskoSkin.scheme");
    m_gui.setFont("DejaVuSans-10");

    { // Add the color picker
        const float X_DIM = 0.015f, Y_DIM = 0.1f;
        const float X_POS = 0.05f, Y_POS = 0.05f;
        const float PADDING = 0.01f;

        m_rSlider = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "RedSlider"));
        m_rSlider->setMaxValue(255.0f);
        m_rSlider->setCurrentValue(m_colorPickerRed);
        m_rSlider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onColorPickerRedChange, this));
        m_rSlider->setClickStep(1.0f);

        m_gSlider = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(X_POS + X_DIM + PADDING, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "GreenSlider"));
        m_gSlider->setMaxValue(255.0f);
        m_gSlider->setCurrentValue(m_colorPickerGreen);
        m_gSlider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onColorPickerGreenChange, this));
        m_gSlider->setClickStep(1.0f);

        m_bSlider = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(X_POS + (X_DIM + PADDING) * 2, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "BlueSlider"));
        m_bSlider->setMaxValue(255.0f);
        m_bSlider->setCurrentValue(m_colorPickerBlue);
        m_bSlider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onColorPickerBlueChange, this));
        m_bSlider->setClickStep(1.0f);
    }

    { // Add the physics mode radio buttons
        const float Y_POS = 0.20f;
        const float DIMS_PERC = 0.02f;
        const float DIMS_PIXELS = 20.0f;
        const float PADDING = 0.02f;

        m_rigidRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.05f, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "RigidRadioButton"));

        m_dynamicRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.05f + DIMS_PERC + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DynamicRadioButton"));
        
        m_rigidRadioButton->setSelected(true);
        m_dynamicRadioButton->setSelected(false);

        // Set up events
        m_rigidRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onRigidButtonMouseClick, this));
        m_dynamicRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onDynamicButtonMouseClick, this));
    }

    m_gui.setMouseCursor("TaharezLook/MouseArrow");
    m_gui.showMouseCursor();
    SDL_ShowCursor(0); ///< Hide regural mouse cursor
}

void LevelEditorScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    m_textureProgram.use();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    {// Draw the color picker quad
        const float QUAD_SIZE = 75.0f;

        // Kill me
        glm::vec4 destRect(
            m_bSlider->getXPosition().d_scale * m_window->getScreenWidth() + 40.0f - m_window->getScreenWidth() / 2.0f,
            m_bSlider->getYPosition().d_scale * m_window->getScreenHeight() + m_window->getScreenHeight() / 2.0f -
            m_bSlider->getHeight().d_scale * m_window->getScreenHeight() * 1.5f - QUAD_SIZE / 2.0f,
            QUAD_SIZE, QUAD_SIZE
        );

        m_spriteBatch.draw(
            destRect,
            glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
            m_blankTexture.id,
            0.0f,
            Bengine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255)
        );
    }

    { // Draw labels for radio buttons
        glm::vec2 pos(
            m_rigidRadioButton->getXPosition().d_scale * m_window->getScreenWidth() - m_window->getScreenWidth() / 2.0f + m_rigidRadioButton->getWidth().d_offset / 2.0f,
            m_window->getScreenHeight() / 2.0f - m_rigidRadioButton->getYPosition().d_scale * m_window->getScreenHeight()
        );

        m_spriteFont->draw(
            m_spriteBatch,
            "Rigid",
            pos,
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255),
            Bengine::Justification::MIDDLE
        );

        pos = glm::vec2(
            m_dynamicRadioButton->getXPosition().d_scale * m_window->getScreenWidth() - m_window->getScreenWidth() / 2.0f + m_dynamicRadioButton->getWidth().d_offset / 2.0f,
            m_window->getScreenHeight() / 2.0f - m_dynamicRadioButton->getYPosition().d_scale * m_window->getScreenHeight()
        );

        m_spriteFont->draw(
            m_spriteBatch,
            "Dynamic",
            pos,
            glm::vec2(0.7f),
            0.0f,
            Bengine::ColorRGBA8(255, 255, 255, 255),
            Bengine::Justification::MIDDLE
        );
    }

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();

    m_gui.draw();
}

void LevelEditorScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_gui.onSDLEvent(evnt);

        switch (evnt.type) {
        case SDL_QUIT:
            onExitClicked();
            break;
        }
    }
}

void LevelEditorScreen::onColorPickerRedChange()
{
    m_colorPickerRed = m_rSlider->getCurrentValue();
}

void LevelEditorScreen::onColorPickerGreenChange()
{
    m_colorPickerGreen = m_gSlider->getCurrentValue();
}

void LevelEditorScreen::onColorPickerBlueChange()
{
    m_colorPickerBlue = m_bSlider->getCurrentValue();
}

void LevelEditorScreen::onRigidButtonMouseClick()
{
    m_physicsMode = PhysicsMode::RIGID;
}

void LevelEditorScreen::onDynamicButtonMouseClick()
{
    m_physicsMode = PhysicsMode::DYNAMIC;
}

void LevelEditorScreen::onExitClicked()
{
    m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
}
