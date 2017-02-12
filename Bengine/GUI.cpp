#include "GUI.h"

namespace Bengine {

CEGUI::OpenGL3Renderer* GUI::m_renderer = nullptr;

void GUI::init(const std::string& resourceDirectory)
{
    // Check if the renderer and the system were already initialized
    if (m_renderer == nullptr) {
        m_renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();

        // Make the resource provider
        CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

        // Set up directories
        rp->setResourceGroupDirectory("imagesets", resourceDirectory + "/imagesets/");
        rp->setResourceGroupDirectory("schemes", resourceDirectory + "/schemes/");
        rp->setResourceGroupDirectory("fonts", resourceDirectory + "/fonts/");
        rp->setResourceGroupDirectory("layouts", resourceDirectory + "/layouts/");
        rp->setResourceGroupDirectory("looknfeels", resourceDirectory + "/looknfeel/");
        rp->setResourceGroupDirectory("lua_scripts", resourceDirectory + "/lua_scripts/");

        CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
        CEGUI::Scheme::setDefaultResourceGroup("schemes");
        CEGUI::Font::setDefaultResourceGroup("fonts");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
        CEGUI::WindowManager::setDefaultResourceGroup("layouts");
        CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
    }

    m_context = &CEGUI::System::getSingleton().createGUIContext(m_renderer->getDefaultRenderTarget());
    m_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
    m_context->setRootWindow(m_root);
}

void GUI::destroy()
{
    CEGUI::System::getSingleton().destroyGUIContext(*m_context); ///< Destroy the context
}

void GUI::draw()
{
    m_renderer->beginRendering();
    m_context->draw();
    m_renderer->endRendering();

    glDisable(GL_SCISSOR_TEST);
}

void GUI::loadScheme(const std::string& schemeFile)
{
    CEGUI::SchemeManager::getSingleton().createFromFile(schemeFile); ///< Loads a scheme
}

void GUI::setFont(const std::string& fontFile)
{
    CEGUI::FontManager::getSingleton().createFromFile(fontFile + ".font"); ///< Loads the font
    m_context->setDefaultFont(fontFile); ///< Sets the default font
}

CEGUI::Window* GUI::createWidget(const std::string& type,
                                 const glm::vec4& destRectPerc,
                                 const glm::vec4& destRectPix,
                                 const std::string& name /*= ""*/)
{
    // Create the window
    CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow(type, name);
    m_root->addChild(newWindow);

    // Set the window position and size
    setWidgetDimensions(newWindow, destRectPerc, destRectPix);

    return newWindow;
}

void GUI::setWidgetDimensions(CEGUI::Window* widget, const glm::vec4& destRectPerc, const glm::vec4& destRectPix)
{
    widget->setPosition(CEGUI::UVector2(CEGUI::UDim(destRectPerc.x, destRectPix.x), CEGUI::UDim(destRectPerc.y, destRectPix.y)));
    widget->setSize(CEGUI::USize(CEGUI::UDim(destRectPerc.z, destRectPix.z), CEGUI::UDim(destRectPerc.w, destRectPix.w)));
}

}
