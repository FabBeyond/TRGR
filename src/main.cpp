#include <Geode/Geode.hpp>
#include <sol/sol.hpp>
#include "include/luaAPI.hpp"
#include "include/scriptPopup.hpp"

using namespace geode::prelude;

$execute {
    setupLuaAPI();
}

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(MyEditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer *layer) {
        if (!EditorPauseLayer::init(layer)) return false;

        auto guidelinesMenu = this->getChildByID("guidelines-menu");
        if (!guidelinesMenu) {
            log::error("Couldn't find guidelines menu, not adding run button");
            return false;
        }

        auto button = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSprite("buttonSprite.png"_spr),
			this,

			menu_selector(MyEditorPauseLayer::openEditor)
		);
        button->setID("gdepl-run-button"_spr);
        guidelinesMenu->addChild(button);
        guidelinesMenu->updateLayout();

        return true;
    }

    void openEditor(CCObject*) {
        ScriptPopup::create()->show();
    }
};