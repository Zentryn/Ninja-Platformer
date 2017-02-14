#include "LevelEditorScreen.h"
#include <Bengine/ResourceManager.h>

const int MOUSE_LEFT = 0;
const int MOUSE_RIGHT = 1;

LevelEditorScreen::LevelEditorScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_EDITOR;
}

LevelEditorScreen::~LevelEditorScreen()
{
}

void WidgetLabel::draw(Bengine::SpriteBatch& sp, Bengine::SpriteFont& sf, Bengine::Window* w)
{
    if (!widget->isVisible()) return;

    glm::vec2 pos;
    pos.x = widget->getXPosition().d_scale * w->getScreenWidth() - w->getScreenWidth() / 2.0f +
        widget->getWidth().d_offset / 2.0f;
    pos.y = w->getScreenHeight() / 2.0f - widget->getYPosition().d_scale * w->getScreenHeight();

    sf.draw(sp, text.c_str(), pos, glm::vec2(scale), 0.0f, color, Bengine::Justification::MIDDLE);
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
    m_mouseButtons[MOUSE_LEFT] = false;
    m_mouseButtons[MOUSE_RIGHT] = false;

    m_debugRenderer.init();

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
    m_lightProgram.dispose();
}

void LevelEditorScreen::update()
{
    m_camera.update();
    checkInput();
    m_gui.update();
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

    // Add group box back panel
    m_groupBox = static_cast<CEGUI::GroupBox*>(m_gui.createWidget("TaharezLook/GroupBox", glm::vec4(0.001f, 0.0f, 0.18f, 0.72f), glm::vec4(0.0f), "GroupBox"));
    // Group box should be behind everything.
    m_groupBox->setAlwaysOnTop(false);
    m_groupBox->moveToBack();
    m_groupBox->disable(); // If you don't disable it, clicking on it will move it to the foreground and it will steal events from other widgets.

    { // Add the color picker
        const float X_POS = 0.01f;
        const float X_DIM = 0.015f, Y_DIM = 0.1f;
        const float Y_POS = 0.05f;
        const float PADDING = 0.005f;

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

        m_aSlider = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(X_POS + (X_DIM + PADDING) * 3, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "AlphaSlider"));
        m_aSlider->setMaxValue(255.0f);
        m_aSlider->setCurrentValue(m_colorPickerAlpha);
        m_aSlider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onColorPickerAlphaChange, this));
        m_aSlider->setClickStep(1.0f);
    }

    { // Add Object type radio buttons
        const float X_POS = 0.02f;
        const float Y_POS = 0.20f;
        const float DIMS_PIXELS = 20.0f;
        const float PADDING = 0.042f;
        const float TEXT_SCALE = 0.6f;
        const int GROUP_ID = 1;
        m_playerRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlayerButton"));
        m_playerRadioButton->setSelected(true);
        m_playerRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onPlayerMouseClick, this));
        m_playerRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_playerRadioButton, "Player", TEXT_SCALE);

        m_platformRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlatformButton"));
        m_platformRadioButton->setSelected(false);
        m_platformRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onPlatformMouseClick, this));
        m_platformRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_platformRadioButton, "Platform", TEXT_SCALE);

        m_finishRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "FinishedButton"));
        m_finishRadioButton->setSelected(false);
        m_finishRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onFinishMouseClick, this));
        m_finishRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_finishRadioButton, "Finish", TEXT_SCALE);

        m_lightRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING * 3.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "LightButton"));
        m_lightRadioButton->setSelected(false);
        m_lightRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onLightMouseClick, this));
        m_lightRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_lightRadioButton, "Light", TEXT_SCALE);

        m_objectMode = ObjectMode::PLAYER;
    }

    { // Add the physics mode radio buttons as well as rotation and size spinner
        const float X_POS = 0.02f;
        const float Y_POS = 0.28f;
        const float DIMS_PIXELS = 20.0f;
        const float PADDING = 0.04f;
        const float TEXT_SCALE = 0.7f;
        const int GROUP_ID = 2;
        m_rigidRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "RigidButton"));
        m_rigidRadioButton->setSelected(true);
        m_rigidRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onRigidMouseClick, this));
        m_rigidRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_rigidRadioButton, "Rigid", TEXT_SCALE);

        m_dynamicRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DynamicButton"));
        m_dynamicRadioButton->setSelected(false);
        m_dynamicRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onDynamicMouseClick, this));
        m_dynamicRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_dynamicRadioButton, "Dynamic", TEXT_SCALE);

        // Rotation spinner
        m_rotationSpinner = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "RotationSpinner"));
        m_rotationSpinner->setMinimumValue(0.0);
        m_rotationSpinner->setMaximumValue(M_PI * 2.0);
        m_rotationSpinner->setCurrentValue(m_rotation);
        m_rotationSpinner->setStepSize(0.01);
        m_rotationSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        m_rotationSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onRotationValueChange, this));
        m_widgetLabels.emplace_back(m_rotationSpinner, "Rotation", TEXT_SCALE);

        // Light size
        m_sizeSpinner = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "SizeSpinner"));
        m_sizeSpinner->setMinimumValue(0.0);
        m_sizeSpinner->setMaximumValue(100.0);
        m_sizeSpinner->setCurrentValue(m_lightSize);
        m_sizeSpinner->setStepSize(0.1);
        m_sizeSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        m_sizeSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onSizeValueChange, this));
        m_widgetLabels.emplace_back(m_sizeSpinner, "Size", TEXT_SCALE);

        m_physicsMode = PhysicsMode::RIGID;
    }

    { // Add platform dimension spinners
        const float X_POS = 0.02f;
        const float Y_POS = 0.35f;
        const float DIMS_PIXELS = 20.0f;
        const float PADDING = 0.04f;
        const float TEXT_SCALE = 0.7f;
        m_widthSpinner = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "WidthSpinner"));
        m_widthSpinner->setMinimumValue(0.0);
        m_widthSpinner->setMaximumValue(10000.0);
        m_widthSpinner->setCurrentValue(0.0f);
        m_widthSpinner->setStepSize(0.1);
        m_widthSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        m_widthSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onWidthValueChange, this));
        m_widgetLabels.emplace_back(m_widthSpinner, "Width", TEXT_SCALE);

        m_heightSpinner = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(X_POS + PADDING * 2.0, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS * 5, DIMS_PIXELS * 2), "HeightSpinner"));
        m_heightSpinner->setMinimumValue(0.0);
        m_heightSpinner->setMaximumValue(10000.0);
        m_heightSpinner->setCurrentValue(0.0f);
        m_heightSpinner->setStepSize(0.1);
        m_heightSpinner->setTextInputMode(CEGUI::Spinner::FloatingPoint);
        m_heightSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&LevelEditorScreen::onHeightValueChange, this));
        m_widgetLabels.emplace_back(m_heightSpinner, "Height", TEXT_SCALE);
    }

    { // Add Selection mode radio buttons and debug render toggle
        const float X_POS = 0.03f;
        const float Y_POS = 0.45f;
        const float DIMS_PIXELS = 20.0f;
        const float PADDING = 0.04f;
        const float TEXT_SCALE = 0.7f;
        const int GROUP_ID = 3;
        m_selectRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "SelectButton"));
        m_selectRadioButton->setSelected(true);
        m_selectRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onSelectMouseClick, this));
        m_selectRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_selectRadioButton, "Select", TEXT_SCALE);

        m_placeRadioButton = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(X_POS + PADDING, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "PlaceButton"));
        m_placeRadioButton->setSelected(false);
        m_placeRadioButton->subscribeEvent(CEGUI::RadioButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onPlaceMouseClick, this));
        m_placeRadioButton->setGroupID(GROUP_ID);
        m_widgetLabels.emplace_back(m_placeRadioButton, "Place", TEXT_SCALE);

        m_debugToggle = static_cast<CEGUI::ToggleButton*>(m_gui.createWidget("TaharezLook/Checkbox", glm::vec4(X_POS + PADDING * 2.5, Y_POS, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DebugToggle"));
        m_debugToggle->setSelected(false);
        m_debugToggle->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelEditorScreen::onDebugToggleClick, this));
        m_widgetLabels.emplace_back(m_debugToggle, "Debug", TEXT_SCALE);

        m_selectionMode = SelectionMode::SELECT;
    }

//     { // Add save and back buttons
//         m_saveButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.03f, 0.5f, 0.1f, 0.05f), glm::vec4(0.0f), "SaveButton"));
//         m_saveButton->setText("Save");
//         m_saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onSaveMouseClick, this));
// 
//         m_saveButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.03f, 0.57f, 0.1f, 0.05f), glm::vec4(0.0f), "LoadButton"));
//         m_saveButton->setText("Load");
//         m_saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onLoadMouseClick, this));
// 
//         m_backButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.03f, 0.64f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
//         m_backButton->setText("Back");
//         m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onBackMouseClick, this));
//     }
// 
//     { // Add save window widgets
//         m_saveWindow = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("TaharezLook/FrameWindow", glm::vec4(0.3f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "SaveWindow"));
//         m_saveWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onSaveCancelClick, this));
//         m_saveWindow->setText("Save Level");
//         // Don't let user drag window around
//         m_saveWindow->setDragMovingEnabled(false);
// 
//         // Children of saveWindow
//         m_saveWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.createWidget(m_saveWindow, "TaharezLook/Combobox", glm::vec4(0.1f, 0.1f, 0.8f, 0.4f), glm::vec4(0.0f), "SaveCombobox"));
//         m_saveWindowSaveButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_saveWindow, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "SaveCancelButton"));
//         m_saveWindowSaveButton->setText("Save");
//         m_saveWindowSaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onSave, this));
// 
//         // Start disabled
//         m_saveWindow->setAlpha(0.0f);
//         m_saveWindow->disable();
//     }
// 
//     { // Add load window widgets
//         m_loadWindow = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("TaharezLook/FrameWindow", glm::vec4(0.3f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "LoadWindow"));
//         m_loadWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onLoadCancelClick, this));
//         m_loadWindow->setText("Load Level");
//         // Don't let user drag window around
//         m_loadWindow->setDragMovingEnabled(false);
// 
//         // Children of loadWindow
//         m_loadWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.createWidget(m_loadWindow, "TaharezLook/Combobox", glm::vec4(0.1f, 0.1f, 0.8f, 0.4f), glm::vec4(0.0f), "LoadCombobox"));
//         m_loadWindowLoadButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_loadWindow, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "LoadCancelButton"));
//         m_loadWindowLoadButton->setText("Load");
//         m_loadWindowLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LevelEditorScreen::onLoad, this));
// 
//         // Start disabled
//         m_loadWindow->setAlpha(0.0f);
//         m_loadWindow->disable();
//     }

    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(false);
    m_gui.setMouseCursor("TaharezLook/MouseArrow");
    m_gui.showMouseCursor();
    SDL_ShowCursor(0); ///< Hide regural mouse cursor
}

void LevelEditorScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    drawWorld();
    drawUI();
}

void LevelEditorScreen::drawWorld()
{
    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    { // Draw all of the boxes and the player
        m_spriteBatch.begin();

        for (auto& box : m_boxes) {
            box.draw(m_spriteBatch);
        }
        if (m_hasPlayer) m_player.draw(m_spriteBatch);

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();
        m_textureProgram.unuse();
    }

    // Debug rendering
    if (m_debugRender) {
        if (m_hasPlayer) m_player.drawDebug(m_debugRenderer);

        for (auto& box : m_boxes) {
            // TODO: Implement this
        }

        // Draw debug lines going through origin
        // +X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(100000.0f, 0.0f), Bengine::ColorRGBA8(255, 0, 0, 200));
        // -X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(-100000.0f, 0.0f), Bengine::ColorRGBA8(200, 0, 0, 50));
        // +Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, 100000.0f), Bengine::ColorRGBA8(0, 255, 0, 200));
        // -Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, -100000.0f), Bengine::ColorRGBA8(0, 200, 0, 50));
        
        m_debugRenderer.end();
        m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
    }
}

void LevelEditorScreen::drawUI()
{
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
        auto parent = m_bSlider->getParent();

        // Kill me
        glm::vec4 destRect(
            m_aSlider->getXPosition().d_scale * m_window->getScreenWidth() + 10.0f - m_window->getScreenWidth() / 2.0f + QUAD_SIZE / 2.0f,
           m_window->getScreenHeight() / 2.0f - m_aSlider->getYPosition().d_scale * m_window->getScreenHeight() -
            m_aSlider->getHeight().d_scale * m_window->getScreenHeight() * 0.5f - QUAD_SIZE / 4.0f,
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

    for (auto& label : m_widgetLabels) {
        label.draw(m_spriteBatch, *m_spriteFont, m_window);
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

void LevelEditorScreen::setObjectWidgetVisibility(bool visibility)
{
    m_rigidRadioButton->setVisible(visibility);
    m_dynamicRadioButton->setVisible(visibility);
    m_widthSpinner->setVisible(visibility);
    m_heightSpinner->setVisible(visibility);
    m_rotationSpinner->setVisible(visibility);
}

void LevelEditorScreen::setSelectionModeWidgetVisibility(bool visibility)
{
    m_placeRadioButton->setVisible(visibility);
    m_selectRadioButton->setVisible(visibility);
}

void LevelEditorScreen::setLightWidgetVisibility(bool visibility)
{
    m_aSlider->setVisible(visibility);
    m_sizeSpinner->setVisible(visibility);
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

void LevelEditorScreen::onColorPickerAlphaChange()
{
    m_colorPickerAlpha = m_aSlider->getCurrentValue();
}

void LevelEditorScreen::onSizeValueChange()
{
    m_lightSize = (float)m_sizeSpinner->getCurrentValue();
}

void LevelEditorScreen::onPlayerMouseClick()
{
    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(false);
}

void LevelEditorScreen::onPlatformMouseClick()
{
    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(true);
}

void LevelEditorScreen::onLightMouseClick()
{
    setLightWidgetVisibility(true);
    setObjectWidgetVisibility(false);
}

void LevelEditorScreen::onFinishMouseClick()
{

}

void LevelEditorScreen::onSelectMouseClick()
{

}

void LevelEditorScreen::onPlaceMouseClick()
{

}

void LevelEditorScreen::onRotationValueChange()
{

}

void LevelEditorScreen::onWidthValueChange()
{

}

void LevelEditorScreen::onHeightValueChange()
{

}

void LevelEditorScreen::onDebugToggleClick()
{
    m_debugRender = m_debugToggle->isSelected();
}

void LevelEditorScreen::onRigidMouseClick()
{
    m_physicsMode = PhysicsMode::RIGID;
}

void LevelEditorScreen::onDynamicMouseClick()
{
    m_physicsMode = PhysicsMode::DYNAMIC;
}

void LevelEditorScreen::onExitClicked()
{
    m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
}