#pragma once

#include <Box2D/Box2D.h>
#include <Bengine/IGameScreen.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/Camera2D.h>
#include <Bengine/Window.h>
#include <Bengine/GLTexture.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/DebugRenderer.h>
#include <memory>
#include "Box.h"
#include "Player.h"
#include <vector>

class GameplayScreen : public Bengine::IGameScreen
{
public:
    GameplayScreen(Bengine::Window* window);
    ~GameplayScreen();

    virtual int getNextScreenIndex() const override;

    virtual int getPreviousScreenIndex() const override;

    virtual void build() override;

    virtual void destroy() override;

    virtual void onEntry() override;

    virtual void onExit() override;

    virtual void update() override;

    virtual void draw() override;

private:
    void checkInput();

    Bengine::SpriteBatch m_spriteBatch;
    std::unique_ptr<Bengine::SpriteFont> m_spriteFont;
    Bengine::GLSLProgram m_textureProgram;
    Bengine::GLSLProgram m_lightProgram;
    Bengine::Camera2D m_camera;
    Bengine::GLTexture m_texture;
    Bengine::Window* m_window;
    Bengine::DebugRenderer m_debugRenderer;

    bool m_renderDebug = false;
    bool m_lights = false;

    Player m_player;
    std::vector<Box> m_boxes;
    std::unique_ptr<b2World> m_world;
};