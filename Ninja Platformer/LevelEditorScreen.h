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
#include "Player.h"
#include "ScreenIndices.h"
#include <vector>

enum class PhysicsMode { RIGID, DYNAMIC };

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

    void onColorPickerRedChange();
    void onColorPickerGreenChange();
    void onColorPickerBlueChange();

    void onRigidButtonMouseClick();
    void onDynamicButtonMouseClick();

    void onExitClicked();

    float m_colorPickerRed = 255.0f;
    float m_colorPickerGreen = 255.0f;
    float m_colorPickerBlue = 255.0f;
    Bengine::GLTexture m_blankTexture;

    CEGUI::Slider* m_rSlider = nullptr;
    CEGUI::Slider* m_gSlider = nullptr;
    CEGUI::Slider* m_bSlider = nullptr;
    CEGUI::RadioButton* m_rigidRadioButton = nullptr;
    CEGUI::RadioButton* m_dynamicRadioButton = nullptr;

    Bengine::Camera2D m_camera;
    Bengine::Window* m_window;
    Bengine::GUI m_gui;

    PhysicsMode m_physicsMode = PhysicsMode::RIGID;

    Bengine::SpriteBatch m_spriteBatch;
    Bengine::GLSLProgram m_textureProgram;
    std::unique_ptr<Bengine::SpriteFont> m_spriteFont;
};

