#include "GUI.h"

GUI::GUI(){
	glViewport(0, 0, 1024, 768);

	CEGUI::OpenGL3Renderer& myRenderer = CEGUI::OpenGL3Renderer::create();
	CEGUI::System::create(myRenderer);

	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("schemes", "datafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets", "datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "datafiles/lua_scripts/");

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-10");

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	myRoot = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("Debug.layout");// Debug.layout");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);

	lastTime = glfwGetTime();
};

void GUI::Render(){
	CalculateDebug();
	// Renders the GUI:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
	CEGUI::System::getSingleton().renderAllGUIContexts();
}



void GUI::CalculateDebug(){
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0){
		frameTime = 1000.0 / double(nbFrames);
		FPS = 1000.0 / frameTime;
		myRoot->getChild("testWindow/Listbox/frameTimeLabel")->setText("Frame Time: " + std::to_string(frameTime));
		myRoot->getChild("testWindow/Listbox/FPSLabel")->setText("FPS: " + std::to_string(FPS));

		nbFrames = 0;
		lastTime += 1.0;
	}
}