#include "GameplayScreen.h"
#include "SDL/SDL.h"
#include <Bengine/IMainGame.h>
#include <iostream>


GameplayScreen::GameplayScreen()
{
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
    return SCREEN_INDEX_NO_SCREEN;
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
    std::cout << "On Entry\n";
}

void GameplayScreen::onExit()
{
    std::cout << "On Exit\n";
}

void GameplayScreen::update()
{
    checkInput();
}

void GameplayScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
}


void GameplayScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
    }
}
