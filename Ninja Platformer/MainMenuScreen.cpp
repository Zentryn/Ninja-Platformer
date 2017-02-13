#include "MainMenuScreen.h"


MainMenuScreen::MainMenuScreen(Bengine::Window* window) :
    m_window(window)
{
    m_screenIndex = SCREEN_INDEX_MAIN_MENU;
}

MainMenuScreen::~MainMenuScreen()
{
}

int MainMenuScreen::getNextScreenIndex() const
{
    return m_nextScreenIndex;
}

int MainMenuScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build()
{

}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
    // Init camera
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    // Init UI
    initUI();
}

void MainMenuScreen::onExit()
{
    m_gui.destroy();
}

void MainMenuScreen::update()
{
    m_camera.update();
    checkInput();
}

void MainMenuScreen::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    m_gui.draw();
}

void MainMenuScreen::initUI()
{
    // Init the UI
    m_gui.init("GUI");
    m_gui.loadScheme("TaharezLook.scheme");
    m_gui.loadScheme("AlfiskoSkin.scheme");
    m_gui.setFont("DejaVuSans-10");

    // New Game button
    CEGUI::PushButton* startGameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.4f, 0.1f, 0.05f), glm::vec4(0.0f), "StartGameButton"));
    startGameButton->setText("Start Game");

    // Level Editor button
    CEGUI::PushButton* levelEditorButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.48f, 0.1f, 0.05f), glm::vec4(0.0f), "LevelEditorButton"));
    levelEditorButton->setText("Level Editor");

    // Exit button
    CEGUI::PushButton* exitButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.56f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitButton"));
    exitButton->setText("Exit Game");

    // Bind events
    startGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onNewGameClicked, this));
    levelEditorButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onLevelEditorClicked, this));
    exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onExitClicked, this));

    m_gui.setMouseCursor("TaharezLook/MouseArrow");
    m_gui.showMouseCursor();
    SDL_ShowCursor(0); ///< Hide regural mouse cursor
}

void MainMenuScreen::checkInput()
{
    SDL_Event evnt;

    while (SDL_PollEvent(&evnt)) {
        m_gui.onSDLEvent(evnt);

        switch (evnt.type) {
        case SDL_QUIT:
            onExitClicked();
            break;
        }
    }
}

void MainMenuScreen::onNewGameClicked()
{
    m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    m_currentState = Bengine::ScreenState::CHANGE_NEXT;
}

void MainMenuScreen::onLevelEditorClicked()
{
    m_nextScreenIndex = SCREEN_INDEX_EDITOR;
    m_currentState = Bengine::ScreenState::CHANGE_NEXT;
}

void MainMenuScreen::onExitClicked()
{
    m_currentState = Bengine::ScreenState::EXIT_APPLICATION;
}
