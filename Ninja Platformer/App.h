#pragma once

#include <Bengine/IMainGame.h>
#include <memory>
#include "GameplayScreen.h"

class App : public Bengine::IMainGame
{
public:
    App();
    ~App();

    virtual void onInit() override;
    virtual void addScreens() override;
    virtual void onExit() override;

private:
    std::unique_ptr<GameplayScreen> m_gameplayScreen = nullptr;
};