#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/CreatorLayer.hpp>
#include "KDLListLayer.hpp"

class $modify(MyCreatorLayer, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) {
			return false;
		}

		auto KDLButton = CCMenuItemSpriteExtra::create(
            CCSprite::create((Mod::get()->getResourcesDir() / "icon.png").string().c_str()),
			this,
			menu_selector(MyCreatorLayer::onKDLButton)
		);

		auto winSize = CCDirector::get()->getWinSize();
        auto listMenu = CCMenu::create();

		listMenu->addChild(KDLButton);
		KDLButton->setID("kdl-Button"_spr);
		listMenu->updateLayout();
		if (Loader::get()->isModLoaded("arcticwoof.rated_layouts")) {
            listMenu->setPosition({24.05f, 67.5f});
        } else {
            listMenu->setPosition({24.05f, 23.5f});
        }
		this->addChild(listMenu, 10);

		return true;
	}

	void onKDLButton(CCObject*) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, KDLListLayer::scene()));
	}
};