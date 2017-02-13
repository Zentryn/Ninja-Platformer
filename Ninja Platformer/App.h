#pragma once

#include <Bengine/IMainGame.h>
#include <memory>
#include "MainMenuScreen.h"
#include "GameplayScreen.h"
#include "LevelEditorScreen.h"

class App : public Bengine::IMainGame
{
public:
    App();
    ~App();

    virtual void onInit() override;
    virtual void addScreens() override;
    virtual void onExit() override;

private:
    std::unique_ptr<MainMenuScreen> m_mainMenuScreen = nullptr;
    std::unique_ptr<GameplayScreen> m_gameplayScreen = nullptr;
    std::unique_ptr<LevelEditorScreen> m_levelEditorScreen = nullptr;
};