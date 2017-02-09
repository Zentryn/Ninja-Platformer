#include "IMainGame.h"
#include "Timing.h"
#include "ScreenList.h"
#include "IGameScreen.h"

namespace Bengine {
	
IMainGame::IMainGame()
{
    m_screenList = std::make_unique<ScreenList>(this);
}
	
	
IMainGame::~IMainGame()
{
    // Empty
}


void IMainGame::run()
{
    if (!init()) return;

    m_isRunning = true;

    FPSLimiter limiter;
    limiter.setMaxFPS(144.0f);

    while (m_isRunning) {
        limiter.begin();

        update();
        draw();

        m_fps = limiter.end();
        m_window.swapBuffer();
    }
}


void IMainGame::onSDLEvent(SDL_Event& evnt)
{
    switch (evnt.type) {
        case SDL_QUIT:
            m_isRunning = false;
            exitGame();
            break;
        case SDL_MOUSEMOTION:
            m_inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
            break;
        case SDL_KEYDOWN:
            m_inputManager.pressKey(evnt.key.keysym.sym);
            break;
        case SDL_KEYUP:
            m_inputManager.releaseKey(evnt.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            m_inputManager.pressKey(evnt.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            m_inputManager.releaseKey(evnt.button.button);
            break;
    }
}


bool IMainGame::init()
{
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    if (!initSystems()) return false;

    onInit();
    addScreens();

    m_currentScreen = m_screenList->getCurrent();
    m_currentScreen->setRunning();
    m_currentScreen->onEntry();

    return true;
}


bool IMainGame::initSystems()
{
    Bengine::init();
    m_window.create("Default", 1920, 1080, 0, 0);
    return true;
}


void IMainGame::exitGame()
{
    m_currentScreen->onExit();

    if (m_screenList) {
        m_screenList->destroy();
        m_screenList.reset();
    }

    m_isRunning = false;
    onExit();
}


void IMainGame::update()
{
    m_inputManager.update();

    if (m_currentScreen) {
        switch (m_currentScreen->getState()) {
        case ScreenState::RUNNING:
            m_currentScreen->update();
            break;
        case ScreenState::CHANGE_NEXT:
            m_currentScreen->onExit();
            m_currentScreen = m_screenList->moveNext();

            if (m_currentScreen != nullptr) {
                m_currentScreen->setRunning();
                m_currentScreen->onEntry();
            }

            break;
        case ScreenState::CHANGE_PREVIOUS:
            m_currentScreen->onExit();
            m_currentScreen = m_screenList->movePrevious();

            if (m_currentScreen != nullptr) {
                m_currentScreen->setRunning();
                m_currentScreen->onEntry();
            }

            break;
        case ScreenState::EXIT_APPLICATION:
            exitGame();
            break;
        default:
            break;
        }
    }
    else {
        // If there's no screen, exit game
        exitGame();
    }
}


void IMainGame::draw()
{
    glViewport(0, 0, m_window.getScreenWidth(), m_window.getScreenHeight());

    if (m_currentScreen && m_currentScreen->getState() == ScreenState::RUNNING) {
        m_currentScreen->draw();
    }
}


}
