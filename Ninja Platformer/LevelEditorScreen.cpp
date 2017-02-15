#include "LevelEditorScreen.h"
#include <Bengine/ResourceManager.h>

const int MOUSE_LEFT = 0;
const int MOUSE_RIGHT = 1;
const float ROTATION_SPEED = 0.01f;
const float SIZE_CHANGE_SPEED = 0.05f;
const float LIGHT_SELECT_RADIUS = 0.5f;
const b2Vec2 GRAVITY(0.0f, -25.0f);

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

    // Init cameras
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setScale(32.0f);

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    m_debugRenderer.init();

    // Init UI
    initUI();

    // Init sprite batch
    m_spriteBatch.init();

    m_world = std::make_unique<b2World>(GRAVITY);

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
    m_uiCamera.update();
    checkInput();

    // Box properties change
    if ((m_selectionMode == SelectionMode::PLACE && m_objectMode == ObjectMode::PLATFORM) || m_selectedBox != NO_BOX) {
        // Size changes
        if (m_inputManager.isKeyDown(SDLK_UP) || m_inputManager.isKeyDown(SDLK_w)) {
            m_heightSpinner->setCurrentValue(m_heightSpinner->getCurrentValue() + SIZE_CHANGE_SPEED);
        }
        else if (m_inputManager.isKeyDown(SDLK_DOWN) || m_inputManager.isKeyDown(SDLK_s)) {
            m_heightSpinner->setCurrentValue(m_heightSpinner->getCurrentValue() - SIZE_CHANGE_SPEED);
        }
        if (m_inputManager.isKeyDown(SDLK_LEFT) || m_inputManager.isKeyDown(SDLK_a)) {
            m_widthSpinner->setCurrentValue(m_widthSpinner->getCurrentValue() - SIZE_CHANGE_SPEED);
        }
        else if (m_inputManager.isKeyDown(SDLK_RIGHT) || m_inputManager.isKeyDown(SDLK_d)) {
            m_widthSpinner->setCurrentValue(m_widthSpinner->getCurrentValue() + SIZE_CHANGE_SPEED);
        }

        // Rotation change
        if (m_inputManager.isKeyDown(SDLK_e)) {
            double newValue = m_rotationSpinner->getCurrentValue() - ROTATION_SPEED;
            if (newValue < 0.0) newValue += M_PI * 2.0;

            m_rotationSpinner->setCurrentValue(newValue);
        }
        else if (m_inputManager.isKeyDown(SDLK_q)) {
            double newValue = m_rotationSpinner->getCurrentValue() + ROTATION_SPEED;
            if (newValue > M_PI * 2.0) newValue -= M_PI * 2.0;

            m_rotationSpinner->setCurrentValue(newValue);
        }

        refreshSelectedBox();
    }
    
    // Light size scaling
    if ((m_selectionMode == SelectionMode::PLACE && m_objectMode == ObjectMode::LIGHT) || m_selectedLight != NO_LIGHT) {
        // Size changes
        if (m_inputManager.isKeyDown(SDLK_UP) || m_inputManager.isKeyDown(SDLK_w)) {
            m_sizeSpinner->setCurrentValue(m_sizeSpinner->getCurrentValue() + SIZE_CHANGE_SPEED);
        }
        else if (m_inputManager.isKeyDown(SDLK_DOWN) || m_inputManager.isKeyDown(SDLK_s)) {
            m_sizeSpinner->setCurrentValue(m_sizeSpinner->getCurrentValue() - SIZE_CHANGE_SPEED);
        }

        refreshSelectedLight();
    }

    // Delete selected object
    if (m_inputManager.isKeyPressed(SDLK_DELETE)) {
        if (m_selectedLight != NO_LIGHT) {
            m_lights.erase(m_lights.begin() + m_selectedLight);
            m_selectedLight = NO_LIGHT;
        }
        else if (m_selectedBox != NO_BOX) {
            m_boxes.erase(m_boxes.begin() + m_selectedBox);
            m_selectedBox = NO_BOX;
        }
    }

    m_gui.update();
}

void LevelEditorScreen::initShaders()
{
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();

    m_lightProgram.compileShaders("Shaders/lightShading.vert", "Shaders/lightShading.frag");
    m_lightProgram.addAttribute("vertexPosition");
    m_lightProgram.addAttribute("vertexColor");
    m_lightProgram.addAttribute("vertexUV");
    m_lightProgram.linkShaders();
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

    glEnable(GL_BLEND); ///< Need to re-enable this so alpha bleding works. Box2D fucks it up!!
}

void LevelEditorScreen::drawWorld()
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

    { // Draw lights
        m_lightProgram.use();

        pUniform = m_lightProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        // Additive blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        m_spriteBatch.begin();

        for (auto& l : m_lights) l.draw(m_spriteBatch);

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_lightProgram.unuse();

        // Restore alpha blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Debug rendering
    if (m_debugRender) {
        if (m_hasPlayer) m_player.drawDebug(m_debugRenderer);

        for (auto& box : m_boxes) {
            Bengine::ColorRGBA8 color;

            if (box.getIsDynamic()) {
                color = Bengine::ColorRGBA8(0, 255, 0, 255);
            }
            else {
                color = Bengine::ColorRGBA8(255, 0, 0, 255);
            }

            glm::vec4 destRect(box.getPosition().x - box.getDimensions().x / 2.0f, box.getPosition().y - box.getDimensions().y / 2.0f, box.getDimensions());

            m_debugRenderer.drawBox(destRect, color, box.getBody()->GetAngle());
        }

        for (auto& l : m_lights) {
            m_debugRenderer.drawCircle(l.position, Bengine::ColorRGBA8(255, 0, 255, 255), LIGHT_SELECT_RADIUS);
        }

        // Draw debug lines going through origin
        // +X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(100000.0f, 0.0f), Bengine::ColorRGBA8(255, 0, 0, 200));
        // -X axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(-100000.0f, 0.0f), Bengine::ColorRGBA8(200, 0, 0, 100));
        // +Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, 100000.0f), Bengine::ColorRGBA8(0, 255, 0, 200));
        // -Y axis
        m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, -100000.0f), Bengine::ColorRGBA8(0, 200, 0, 100));
        
        m_debugRenderer.end();
        m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
    }
}

void LevelEditorScreen::drawUI()
{
    // Outlines
    if (m_selectionMode == SelectionMode::PLACE && !isMouseInUI()) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(x, y));

        // Draw the current box's boundaries
        if (m_objectMode == ObjectMode::PLATFORM) {
            m_debugRenderer.drawBox(glm::vec4(pos.x - m_width / 2.0f, pos.y - m_height / 2.0f, m_width, m_height), Bengine::ColorRGBA8(255, 255, 255, 255), m_rotation);
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
        // Draw the light's boundaries
        else if (m_objectMode == ObjectMode::LIGHT) {
            Light tempLight;
            tempLight.position = pos;
            tempLight.color = Bengine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, (GLubyte)m_colorPickerAlpha);
            tempLight.size = m_lightSize;

            // Draw light
            m_lightProgram.use();
            GLint pUniform = m_textureProgram.getUniformLocation("P");
            glUniformMatrix4fv(pUniform, 1, GL_FALSE, &m_camera.getCameraMatrix()[0][0]);

            // Additive blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            m_spriteBatch.begin();
            tempLight.draw(m_spriteBatch);
            m_spriteBatch.end();
            m_spriteBatch.renderBatch();
            m_lightProgram.unuse();

            // Restore alpha blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Selection radius
            m_debugRenderer.drawCircle(pos, Bengine::ColorRGBA8(255, 255, 255, 255), LIGHT_SELECT_RADIUS);

            // Outer circle
            m_debugRenderer.drawCircle(pos, tempLight.color, tempLight.size);

            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
    }
    // Draw the selected object's debug lines
    else {
        // Draw selected box
        if (m_selectedBox != NO_BOX) {
            const Box& b = m_boxes[m_selectedBox];

            glm::vec4 destRect(b.getPosition().x - b.getDimensions().x / 2.0f, b.getPosition().y - b.getDimensions().y / 2.0f, b.getDimensions());
            m_debugRenderer.drawBox(destRect, Bengine::ColorRGBA8(255, 255, 0, 255), b.getAngle());
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
        // Draw selected light
        if (m_selectedLight != NO_LIGHT) {
            const Light& l = m_lights[m_selectedLight];

            // Outer circle
            m_debugRenderer.drawCircle(l.position, Bengine::ColorRGBA8(255, 255, 0, 255), l.size);

            // Selection radius
            m_debugRenderer.drawCircle(l.position, l.color, LIGHT_SELECT_RADIUS);
            m_debugRenderer.end();
            m_debugRenderer.render(m_camera.getCameraMatrix(), 2.0f);
        }
    }

    m_textureProgram.use();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
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

    m_inputManager.update();

    while (SDL_PollEvent(&evnt)) {
        m_gui.onSDLEvent(evnt);

        switch (evnt.type) {
        case SDL_QUIT:
            onExitClicked();
            break;
        case SDL_MOUSEBUTTONDOWN:
            updateMouseDown(evnt);
            break;
        case SDL_MOUSEBUTTONUP:
            updateMouseUp(evnt);
            break;
        case SDL_MOUSEMOTION:
            updateMouseMotion(evnt);
            break;
        case SDL_MOUSEWHEEL:
            if (m_hasDragged) {
                m_camera.offsetScale(m_camera.getScale() * (float)evnt.wheel.y * 0.05f);

                if (m_camera.getScale() > 5000.0f) {
                    m_camera.setScale(5000.0f);
                }
                else {
                    if (evnt.wheel.y < 0 && m_camera.getScale() > 0.001f) m_dragSpeed *= 1.03f;
                    else m_dragSpeed *= 0.97f;
                }
            }
            break;
        case SDL_KEYDOWN:
            m_inputManager.pressKey(evnt.key.keysym.sym);
            break;
        case SDL_KEYUP:
            m_inputManager.releaseKey(evnt.key.keysym.sym);
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

void LevelEditorScreen::updateMouseDown(SDL_Event& evnt)
{
    const Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Assets/bricks_top.png");
    Bengine::ColorRGBA8 color((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255);

    glm::vec2 pos;
    Box box;
    Light light;

    switch (evnt.button.button) {
    case SDL_BUTTON_LEFT:
        m_mouseButtons[MOUSE_LEFT] = true;
        if (isMouseInUI()) return;

        // Place mode
        if (m_selectionMode == SelectionMode::PLACE) {
            switch (m_objectMode) {
            case ObjectMode::PLAYER:
                if (m_hasPlayer) m_player.destroy(m_world.get());
                pos = m_camera.convertScreenToWorld(glm::vec2(evnt.button.x, evnt.button.y));
                m_player.init(m_world.get(), pos, glm::vec2(2.0f), glm::vec2(1.0f, 1.8f), color);
                m_hasPlayer = true;
                break;
            case ObjectMode::PLATFORM:
                if (m_width > 0.0f && m_height > 0.0f) {
                    pos = m_camera.convertScreenToWorld(glm::vec2(evnt.button.x, evnt.button.y));
                    glm::vec4 uvRect(pos.x, pos.x, m_width, m_height);
                    box.init(m_world.get(), pos, glm::vec2(m_width, m_height), texture, color, m_physicsMode == PhysicsMode::DYNAMIC, m_rotation, false, uvRect);
                    m_boxes.push_back(box);
                    std::cout << "Is dynamic: " << (m_physicsMode == PhysicsMode::DYNAMIC) << "\n";
                }
                break;
            case ObjectMode::LIGHT:
                light.position = m_camera.convertScreenToWorld(glm::vec2(evnt.button.x, evnt.button.y));
                light.size = m_lightSize;
                color.a = (GLubyte)m_colorPickerAlpha;
                light.color = color;
                m_lights.push_back(light);
                break;
            case ObjectMode::FINISH:
                // TODO: Implement this
                break;
            }
        }
        // Select mode
        else {
            pos = m_camera.convertScreenToWorld(glm::vec2(evnt.button.x, evnt.button.y));

            m_selectedLight = NO_LIGHT;

            // Check for lights
            for (size_t i = 0; i < m_lights.size(); i++) {
                if (inLightSelect(m_lights[i], pos)) {
                    m_selectedLight = i;
                    break;
                }
            }

            // If a light was seleccted
            if (m_selectedLight != NO_LIGHT) {
                onLightMouseClick();
                m_selectOffset = pos - m_lights[m_selectedLight].position;
                m_selectedBox = NO_BOX;
                m_isDragging = true;

                // Set variables to the selected light's variables
                m_rSlider->setCurrentValue(m_lights[m_selectedLight].color.r);
                m_gSlider->setCurrentValue(m_lights[m_selectedLight].color.g);
                m_bSlider->setCurrentValue(m_lights[m_selectedLight].color.b);
                m_aSlider->setCurrentValue(m_lights[m_selectedLight].color.a);

                m_sizeSpinner->setCurrentValue(m_lights[m_selectedLight].size);
                m_lightRadioButton->setSelected(true);

                m_objectMode = ObjectMode::LIGHT;

                break; ///< If we selected a light, don't check for boxes
            }

            m_selectedBox = NO_BOX;

            // Check for boxes
            for (size_t i = 0; i < m_boxes.size(); i++) {
                if (m_boxes[i].pointInBox(pos.x, pos.y)) {
                    m_selectedBox = i;
                    break;
                }
            }

            // If a box was selected
            if (m_selectedBox != NO_BOX) {
                onPlatformMouseClick();
                m_selectOffset = pos - m_boxes[m_selectedBox].getPosition();
                m_isDragging = true;

                // Set variables to the selected box's variables
                m_rSlider->setCurrentValue(m_boxes[m_selectedBox].getColor().r);
                m_gSlider->setCurrentValue(m_boxes[m_selectedBox].getColor().g);
                m_bSlider->setCurrentValue(m_boxes[m_selectedBox].getColor().b);

                m_widthSpinner->setCurrentValue(m_boxes[m_selectedBox].getDimensions().x);
                m_heightSpinner->setCurrentValue(m_boxes[m_selectedBox].getDimensions().y);
                m_rotationSpinner->setCurrentValue(m_boxes[m_selectedBox].getAngle());

                if (m_boxes[m_selectedBox].getIsDynamic()) {
                    m_dynamicRadioButton->setSelected(true);
                    m_physicsMode = PhysicsMode::DYNAMIC;
                }
                else {
                    m_rigidRadioButton->setSelected(true);
                    m_physicsMode = PhysicsMode::RIGID;
                }

                m_platformRadioButton->setSelected(true);
                m_objectMode = ObjectMode::PLATFORM;
            }
        }

        break;
    case SDL_BUTTON_RIGHT:
        m_mouseButtons[MOUSE_RIGHT] = true;
        break;
    }
}

void LevelEditorScreen::updateMouseUp(SDL_Event& evnt)
{
    switch (evnt.button.button) {
    case SDL_BUTTON_LEFT:
        m_mouseButtons[MOUSE_LEFT] = false;
        m_isDragging = false;
        break;
    case SDL_BUTTON_RIGHT:
        m_mouseButtons[MOUSE_RIGHT] = false;
        break;
    }
}

void LevelEditorScreen::updateMouseMotion(SDL_Event& evnt)
{
    if (m_mouseButtons[MOUSE_RIGHT]) {
        m_hasDragged = true;
        m_camera.offsetPosition(glm::vec2(-evnt.motion.xrel, evnt.motion.yrel * m_camera.getAspectRatio()) * m_dragSpeed);
    }
    else if (m_isDragging && m_mouseButtons[MOUSE_LEFT]) {
        // Drag selected box
        if (m_selectedBox != NO_BOX) {
            glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(evnt.motion.x, evnt.motion.y)) - m_selectOffset;
            refreshSelectedBox(pos);
        }
        // Drag selectedLight
        else if (m_selectedLight != NO_LIGHT) {
            glm::vec2 pos = m_camera.convertScreenToWorld(glm::vec2(evnt.motion.x, evnt.motion.y)) - m_selectOffset;
            refreshSelectedLight(pos);
        }
    }
}

void LevelEditorScreen::refreshSelectedBox()
{
    if (m_selectedBox == NO_BOX) return;
    refreshSelectedBox(m_boxes[m_selectedBox].getPosition());
}

void LevelEditorScreen::refreshSelectedBox(const glm::vec2& pos)
{
    Box newBox;
    static Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Assets/bricks_top.png");
    Bengine::ColorRGBA8 color((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, 255);
    glm::vec4 uvRect(pos.x, pos.y, m_width, m_height);

    newBox.init(m_world.get(), pos, glm::vec2(m_width, m_height), texture, color, m_physicsMode == PhysicsMode::DYNAMIC, m_rotation, false, uvRect);

    // Destroy the old box and replace it with the new one
    m_boxes[m_selectedBox].destroy(m_world.get());
    m_boxes[m_selectedBox] = newBox;
}

void LevelEditorScreen::refreshSelectedLight()
{
    if (m_selectedLight == NO_LIGHT) return;
    refreshSelectedLight(m_lights[m_selectedLight].position);
}

void LevelEditorScreen::refreshSelectedLight(const glm::vec2& pos)
{
    Light newLight;
    newLight.position = pos;
    newLight.size = m_lightSize;
    newLight.color = Bengine::ColorRGBA8((GLubyte)m_colorPickerRed, (GLubyte)m_colorPickerGreen, (GLubyte)m_colorPickerBlue, (GLubyte)m_colorPickerAlpha);

    m_lights[m_selectedLight] = newLight;
}

bool LevelEditorScreen::isMouseInUI()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    const float SW = (float)m_window->getScreenWidth();
    const float SH = (float)m_window->getScreenHeight();

    return (x >= m_groupBox->getXPosition().d_scale * SW && x <= m_groupBox->getXPosition().d_scale * SW + m_groupBox->getWidth().d_scale  * SW &&
            y >= m_groupBox->getYPosition().d_scale * SH && y <= m_groupBox->getYPosition().d_scale * SH + m_groupBox->getHeight().d_scale * SH);
}

bool LevelEditorScreen::inLightSelect(Light light, glm::vec2 pos)
{
    return (glm::length(pos - light.position) <= LIGHT_SELECT_RADIUS);
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
    m_objectMode = ObjectMode::PLAYER;
    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(false);
}

void LevelEditorScreen::onPlatformMouseClick()
{
    m_objectMode = ObjectMode::PLATFORM;
    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(true);
}

void LevelEditorScreen::onLightMouseClick()
{
    m_objectMode = ObjectMode::LIGHT;
    setLightWidgetVisibility(true);
    setObjectWidgetVisibility(false);
}

void LevelEditorScreen::onFinishMouseClick()
{
    m_objectMode = ObjectMode::FINISH;
    setLightWidgetVisibility(false);
    setObjectWidgetVisibility(false);
    // TODO: Implement this
}

void LevelEditorScreen::onSelectMouseClick()
{
    m_selectionMode = SelectionMode::SELECT;
}

void LevelEditorScreen::onPlaceMouseClick()
{
    m_selectedBox = NO_BOX;
    m_selectedLight = NO_LIGHT;
    m_selectionMode = SelectionMode::PLACE;
}

void LevelEditorScreen::onRotationValueChange()
{
    m_rotation = (float)m_rotationSpinner->getCurrentValue();
}

void LevelEditorScreen::onWidthValueChange()
{
    m_width = (float)m_widthSpinner->getCurrentValue();
}

void LevelEditorScreen::onHeightValueChange()
{
    m_height = (float)m_heightSpinner->getCurrentValue();
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