#include <Geode/Geode.hpp>
#include <mutex>
#include <thread>

using namespace geode::prelude;

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/MapPackCell.hpp>
#include "KDLListLayer.hpp"
#include "MapPacks.hpp"

class $modify(MyCreatorLayer, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) {
			return false;
		}
        
        // I think the codebase is good enough now :) -antic

		auto KDLButton = CCMenuItemSpriteExtra::create(
			CCSprite::create("icon.png"_spr),
			this,
			menu_selector(MyCreatorLayer::onKDLButton)
		);
		KDLButton->setID("kdl-Button"_spr);

		if (auto menu = this->getChildByID("bottom-left-menu")) {
			menu->addChild(KDLButton);
			menu->updateLayout();
		} else {
			auto listMenu = CCMenu::create();
			listMenu->addChild(KDLButton);
			listMenu->setPosition({30.0f, 60.0f});
			this->addChild(listMenu);
		}

		return true;
	}

	void onKDLButton(CCObject*) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, KDLListLayer::scene()));
	}

	
};

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
		bool init(GJSearchObject* object) {
			if (!LevelBrowserLayer::init(object)) return false;

			if (object->m_searchType != SearchType::MapPack) return true;

			auto winSize = CCDirector::get()->getWinSize();
			auto demonlistSprite = CCSprite::create("mp.png"_spr);
			auto demonlistButton = CCMenuItemSpriteExtra::create(demonlistSprite, this, menu_selector(MyLevelBrowserLayer::onKDLButton));
			demonlistButton->setID("demonlist-button"_spr);
			auto menu = CCMenu::create();
			menu->addChild(demonlistButton);
			menu->setPosition({winSize.width - demonlistSprite->getContentHeight() / 2.0f - 4.0f, demonlistSprite->getContentHeight() / 2.0f + 54.0f});
			menu->setID("demonlist-menu"_spr);
			addChild(menu, 2);

			return true;
    }

    void onKDLButton(CCObject* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, MapPacks::scene()));
    }
};


constexpr int KDL_PACK_ID_MIN = 1000;

class $modify(KDLMapPackCell, MapPackCell) {
    void onClick(CCObject* sender) {
        if (m_mapPack && m_mapPack->m_packID >= KDL_PACK_ID_MIN) {
            std::string ids;
            if (m_mapPack->m_levels) {
                for (auto* obj : CCArrayExt<CCString*>(m_mapPack->m_levels)) {
                    if (!obj) continue;
                    if (!ids.empty()) ids += ",";
                    ids += obj->getCString();
                }
            }

            auto search = GJSearchObject::create(SearchType::Type19, ids);
            auto scene = LevelBrowserLayer::scene(search);
            CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
            return;
        }

        MapPackCell::onClick(sender);
    }
};

struct KdlEntry {
    int demonType;
    std::string rate;
};

static std::unordered_map<int, KdlEntry> g_kdlLevels;
static std::mutex g_kdlLevelsMutex;

$on_mod(Loaded) {
    std::thread([] {
        auto res = web::WebRequest().getSync("https://the-kdl.com/api/list/all");
        if (!res.ok()) return;

        auto jsonRes = res.json();
        if (!jsonRes) return;

        auto json = jsonRes.unwrap()["levels"];
        auto arr = json.asArray();
        if (!arr) return;

        for (auto& entry : arr.unwrap()) {
            auto id = entry.get<int>("id").unwrapOr(-1);
            auto type = entry.get<int>("demon_type").unwrapOr(1);
            auto rate = entry.get<std::string>("rate").unwrapOr("");
            if (id > 0) {
                std::lock_guard<std::mutex> lock(g_kdlLevelsMutex);
                g_kdlLevels[id] = { type, rate };
            }
        }
    }).detach();
}

static void rate(GJGameLevel* level) {
    KdlEntry entry;
    {
        std::lock_guard<std::mutex> lock(g_kdlLevelsMutex);
        auto it = g_kdlLevels.find(level->m_levelID);
        if (it == g_kdlLevels.end()) return;
        entry = it->second;
    }

    level->m_difficulty = GJDifficulty::Demon;
    level->m_demon = 1;
    level->m_demonDifficulty = entry.demonType;
    
    if (entry.rate == "featured") {
        level->m_featured = 1;
    } else if (entry.rate == "epic") {
        level->m_isEpic = 1;
    } else if (entry.rate == "legendary") {
        level->m_isEpic = 2;
    } else if (entry.rate == "mythic") {
        level->m_isEpic = 3;
    }
}

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        rate(level);
        if (!LevelInfoLayer::init(level, challenge)) return false;
        return true;
    }

    void levelDownloadFinished(GJGameLevel* level) {
        rate(level);
        LevelInfoLayer::levelDownloadFinished(level);
    }

    void levelUpdateFinished(GJGameLevel* level, UpdateResponse response) {
        rate(level);
        LevelInfoLayer::levelUpdateFinished(level, response);
    }
};

class $modify(MyLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        rate(level);
        LevelCell::loadFromLevel(level);
    }
};