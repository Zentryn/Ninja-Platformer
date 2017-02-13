#pragma once

#include <Bengine/IGameScreen.h>
#include <Bengine/GUI.h>
#include <Bengine/Camera2D.h>
#include <Bengine/Window.h>
#include "ScreenIndices.h"

class MainMenuScreen : public Bengine::IGameScreen
{
public:
    MainMenuScreen(Bengine::Window* window);
    ~MainMenuScreen();

    virtual int getNextScreenIndex() const override;

    virtual int getPreviousScreenIndex() const override;

    virtual void build() override;

    virtual void destroy() override;

    virtual void onEntry() override;

    virtual void onExit() override;

    virtual void update() override;

    virtual void draw() override;

private:
    void initUI();
    void checkInput();
    void onNewGameClicked();
    void onLevelEditorClicked();
    void onExitClicked();

    int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    Bengine::Camera2D m_camera;
    Bengine::Window* m_window;
    Bengine::GUI m_gui;
};

