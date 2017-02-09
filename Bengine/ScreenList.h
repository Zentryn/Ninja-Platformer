#pragma once

#include <vector>

namespace Bengine {

class IGameScreen;
class IMainGame;

class ScreenList
{
public:
    ScreenList(IMainGame* game);
    ~ScreenList();

    IGameScreen* moveNext();
    IGameScreen* movePrevious();

    void addScreen(IGameScreen* newScreen);
    void setScreen(int nextScreen);
    IGameScreen* getCurrent();

    void destroy();
protected:
    IMainGame* m_game = nullptr;
    std::vector<IGameScreen*> m_screens;
    int m_currentScreenIndex = -1;
};

}