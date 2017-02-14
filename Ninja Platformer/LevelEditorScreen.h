#pragma once

#include <Bengine/GUI.h>
#include <Box2D/Box2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/Camera2D.h>
#include <Bengine/Window.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/GLTexture.h>
#include <Bengine/DebugRenderer.h>
#include <memory>
#include "Box.h"
#include "Light.h"
#include "Player.h"
#include "ScreenIndices.h"
#include <vector>

enum class PhysicsMode {
    RIGID,
    DYNAMIC
};

enum class ObjectMode {
    PLAYER,
    PLATFORM,
    LIGHT,
    FINISH
};

enum class SelectionMode {
    SELECT,
    PLACE
};

// A label for a CEGUI widget.
class WidgetLabel {
public:
    WidgetLabel() {};
    WidgetLabel(CEGUI::Window* w, const std::string& Text, float Scale) :
    widget(w), text(Text), scale(Scale)
    {
        // Empty
    }

    // Draws the label above the widget
    void draw(Bengine::SpriteBatch& sp, Bengine::SpriteFont& sf, Bengine::Window* w);

    CEGUI::Window* widget = nullptr;
    std::string text = "";
    Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);
    float scale = 0.7f;
};

class LevelEditorScreen : public Bengine::IGameScreen
{
public:
    LevelEditorScreen(Bengine::Window* window);
    ~LevelEditorScreen();

    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    virtual void build() override;
    virtual void destroy() override;
    virtual void onEntry() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void draw() override;

private:
    void initShaders();
    void initUI();
    void checkInput();

    void drawWorld();
    void drawUI();

    void setObjectWidgetVisibility(bool visibility);
    void setSelectionModeWidgetVisibility(bool visibility);
    void setLightWidgetVisibility(bool visibility);

    /******************************************/
    /* Event handlers                         */
    /******************************************/
    void onRigidMouseClick();
    void onDynamicMouseClick();
    void onColorPickerRedChange();
    void onColorPickerGreenChange();
    void onColorPickerBlueChange();
    void onColorPickerAlphaChange();
    void onSizeValueChange();
    void onPlayerMouseClick();
    void onPlatformMouseClick();
    void onLightMouseClick();
    void onFinishMouseClick();
    void onSelectMouseClick();
    void onPlaceMouseClick();
    void onRotationValueChange();
    void onWidthValueChange();
    void onHeightValueChange();
    void onDebugToggleClick();

    void onExitClicked();

    float m_colorPickerRed = 255.0f;
    float m_colorPickerGreen = 255.0f;
    float m_colorPickerBlue = 255.0f;
    float m_colorPickerAlpha = 255.0f;

    /******************************************/
    /* Member variables                       */
    /******************************************/
    CEGUI::GroupBox* m_groupBox = nullptr;

    /* Sliders */
    CEGUI::Slider* m_rSlider = nullptr;
    CEGUI::Slider* m_gSlider = nullptr;
    CEGUI::Slider* m_bSlider = nullptr;
    CEGUI::Slider* m_aSlider = nullptr;

    /* Radio buttons */
    CEGUI::RadioButton* m_rigidRadioButton = nullptr;
    CEGUI::RadioButton* m_dynamicRadioButton = nullptr;
    CEGUI::RadioButton* m_playerRadioButton = nullptr;
    CEGUI::RadioButton* m_platformRadioButton = nullptr;
    CEGUI::RadioButton* m_lightRadioButton = nullptr;
    CEGUI::RadioButton* m_finishRadioButton = nullptr;
    CEGUI::RadioButton* m_selectRadioButton = nullptr;
    CEGUI::RadioButton* m_placeRadioButton = nullptr;

    /* Toggle buttons */
    CEGUI::ToggleButton* m_debugToggle = nullptr;

    /* Spinners */
    CEGUI::Spinner* m_rotationSpinner = nullptr;
    CEGUI::Spinner* m_widthSpinner = nullptr;
    CEGUI::Spinner* m_heightSpinner = nullptr;
    CEGUI::Spinner* m_sizeSpinner = nullptr;

    /* Big buttons */
    CEGUI::PushButton* m_backButton = nullptr;

    float m_rotation = 0.0f;
    bool m_debugRender = false;
    float m_lightSize = 0.0f;

    bool m_mouseButtons[2];
    bool m_hasPlayer = false;
    
    std::vector<Light> m_lights;
    std::vector<Box> m_boxes;

    Player m_player;

    Bengine::Camera2D m_camera;
    Bengine::Window* m_window;
    Bengine::GUI m_gui;

    Bengine::GLTexture m_blankTexture;

    PhysicsMode m_physicsMode = PhysicsMode::RIGID;
    ObjectMode m_objectMode = ObjectMode::PLAYER;
    SelectionMode m_selectionMode = SelectionMode::PLACE;

    std::vector<WidgetLabel> m_widgetLabels;

    Bengine::SpriteBatch m_spriteBatch;
    Bengine::GLSLProgram m_textureProgram;
    Bengine::GLSLProgram m_lightProgram;
    Bengine::DebugRenderer m_debugRenderer;
    std::unique_ptr<Bengine::SpriteFont> m_spriteFont;
};
