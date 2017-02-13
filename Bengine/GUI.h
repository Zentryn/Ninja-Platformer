#pragma once

#include <CEGUI/CEGUI.h>
#include <string>
#include <glm/glm.hpp>
#include <SDL/SDL_events.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

namespace Bengine {

class GUI
{
public:
    void init(const std::string& resourceDirectory);
    void destroy();

    void draw();
    void update();

    void setMouseCursor(const std::string& imageFile);
    void showMouseCursor();
    void hideMouseCursor();

    void onSDLEvent(SDL_Event& evnt);

    void loadScheme(const std::string& schemeFile);
    void setFont(const std::string& fontFile);

    // Create widget to root
    CEGUI::Window* createWidget(
        const std::string& type,
        const glm::vec4& destRectPerc,
        const glm::vec4& destRectPix,
        const std::string& name = ""
    );

    // Create widget to custom parent
    CEGUI::Window* createWidget(
        CEGUI::Window* parent,
        const std::string& type,
        const glm::vec4& destRectPerc,
        const glm::vec4& destRectPix,
        const std::string& name = ""
    );

    static void setWidgetDimensions(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix);

    // Getters
    static CEGUI::OpenGL3Renderer* getRenderer() { return m_renderer; }
    CEGUI::GUIContext* getContext() const { return m_context; }
private:
    static CEGUI::OpenGL3Renderer* m_renderer;
    CEGUI::GUIContext* m_context = nullptr;
    CEGUI::Window* m_root = nullptr;
    unsigned int m_lastTime = 0;
};

}