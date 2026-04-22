#pragma once
#include <Geode/Geode.hpp>
#include "CreatorLeaderboardLayer.hpp"
#include "LeaderboardLayer.hpp"

#define BASE_URL "https://raw.githubusercontent.com/Therealkeanan00/Keanan-Demon-List-Updated-Geometry-Dash/main/"

using namespace geode::prelude;

class KDLListLayer : public CCLayer, public LevelManagerDelegate {
protected:
    std::vector<int> m_allIds;
    GJListLayer* m_listLayer = nullptr;
    CCMenu* m_tabMenu = nullptr;
    std::vector<CCScale9Sprite*> m_tabBgsGreen;
    std::vector<CCScale9Sprite*> m_tabBgsBlue;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    int m_currentPage = 0;

    bool init() {
        if (!CCLayer::init()) return false;
		auto winSize = CCDirector::get()->getWinSize();
		
        // background
		auto background = CCSprite::create("GJ_gradientBG.png");
        background->setScaleX(winSize.width / background->getContentSize().width);
        background->setScaleY(winSize.height / background->getContentSize().height);
        background->setPosition(winSize / 2);
        background->setColor({40, 0, 80});
        this->addChild(background, -1);

        // back arrow
		auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backButton = CCMenuItemSpriteExtra::create(
            backSprite, this, menu_selector(KDLListLayer::onBack)
        );

        // Leaderboard button

        // node
        auto leaderboardNode = CCNode::create();
        leaderboardNode->setContentSize({30.f, 30.f});

        // btn bkg
        auto leaderboardBkg = CCScale9Sprite::create("GJ_button_01.png");
        leaderboardBkg->setContentSize({30.f, 30.f});
        leaderboardBkg->setPosition({15.f, 15.f});
        leaderboardNode->addChild(leaderboardBkg);

        auto trophySprite = CCSprite::createWithSpriteFrameName("rankIcon_1_001.png");
        trophySprite->setScale(0.6f);
        trophySprite->setPosition({15.f, 15.f});
        leaderboardNode->addChild(trophySprite);
        auto leaderboardButton = CCMenuItemSpriteExtra::create(
            leaderboardNode, this, menu_selector(KDLListLayer::onOpenLeaderboard)
        );

        // Creator Leaderboard button

        // node
        auto creatorLeaderboardNode = CCNode::create();
        creatorLeaderboardNode->setContentSize({30.f, 30.f});
        
        // btn bkg
        auto creatorLeaderboardBkg = CCScale9Sprite::create("GJ_button_01.png");
        creatorLeaderboardBkg->setContentSize({30.f, 30.f});
        creatorLeaderboardBkg->setPosition({15.f, 15.f});
        creatorLeaderboardNode->addChild(creatorLeaderboardBkg);

        
        auto creatorSprite = CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
        creatorSprite->setScale(0.6f);
        creatorSprite->setPosition({15.f, 15.f});
        creatorLeaderboardNode->addChild(creatorSprite);
        auto creatorLeaderboardButton = CCMenuItemSpriteExtra::create(
            creatorLeaderboardNode, this, menu_selector(KDLListLayer::onOpenCreatorLeaderboard)
        );

        // top "stuff"
		auto topMenu = CCMenu::create();
        topMenu->addChild(backButton);
        topMenu->addChild(leaderboardButton);
        topMenu->addChild(creatorLeaderboardButton);
        topMenu->setPosition({0.f, 0.f});
        backButton->setPosition({25.f, winSize.height - 25.f});
        leaderboardButton->setPosition({65.f, winSize.height - 25.f});
        creatorLeaderboardButton->setPosition({100.f, winSize.height - 25.f});
        this->addChild(topMenu);

        // not so top "stuff"  
        m_tabMenu = CCMenu::create();
        m_tabMenu->setPosition({0.f, 0.f});
        m_tabMenu->setContentSize({winSize.width, 50.f});

        auto prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        prevSprite->setScale(0.7f);
        m_prevButton = CCMenuItemSpriteExtra::create(
            prevSprite, this, menu_selector(KDLListLayer::onPrevPage)
        );

        auto nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        nextSprite->setScale(0.7f);
        nextSprite->setFlipX(true);
        m_nextButton = CCMenuItemSpriteExtra::create(
            nextSprite, this, menu_selector(KDLListLayer::onNextPage)
        );

        m_prevButton->setPosition({winSize.width / 2.f - 195.f, 25.f});
        m_nextButton->setPosition({winSize.width / 2.f + 195.f, 25.f});
        m_tabMenu->addChild(m_prevButton);
        m_tabMenu->addChild(m_nextButton);

        struct Tab { const char* label; const char* url; };
        Tab tabs[] = {
            {"Nerfed\nVerified", BASE_URL "NerfedVerified.json"},
            {"Nerfed\nUnverified", BASE_URL "NerfedUnverified.json"},
            {"Buffed\nVerified", BASE_URL "BuffedVerified.json"},
            {"Buffed\nUnverified", BASE_URL "BuffedUnverified.json"},
        };

        float tabW = 75.f;
        float tabGap = 8.f;
        float totalTabW = 4 * tabW + 3 * tabGap;
        float tabStartX = winSize.width / 2.f - totalTabW / 2.f + tabW / 2.f;

        int defaultTab = 2;
        int tabIndex = 0;
        for (auto& tab : tabs) {
            auto lbl = CCLabelBMFont::create(tab.label, "bigFont.fnt");
            lbl->setScale(0.3f);

            bool active = tabIndex == defaultTab;

            auto tabBgGreen = CCScale9Sprite::create("GJ_button_01.png");
            tabBgGreen->setContentSize({75.f, 40.f});
            tabBgGreen->setPosition({37.5f, 20.f});
            tabBgGreen->setVisible(!active);
            m_tabBgsGreen.push_back(tabBgGreen);

            auto tabBgBlue = CCScale9Sprite::create("GJ_button_02.png");
            tabBgBlue->setContentSize({75.f, 40.f});
            tabBgBlue->setPosition({37.5f, 20.f});
            tabBgBlue->setVisible(active);
            m_tabBgsBlue.push_back(tabBgBlue);

            auto node = CCNode::create();
            node->setContentSize({75.f, 40.f});
            lbl->setPosition({37.5f, 20.f});
            node->addChild(tabBgGreen);
            node->addChild(tabBgBlue);
            node->addChild(lbl);

            auto button = CCMenuItemSpriteExtra::create(
                node, this, menu_selector(KDLListLayer::onTabPressed)
            );
            button->setUserObject(CCString::create(tab.url));
            button->setPosition({tabStartX + tabIndex * (tabW + tabGap), 25.f});
            m_tabMenu->addChild(button);
            tabIndex++;
        }

    
        loadTab(BASE_URL "BuffedVerified.json");

        auto emptyArr = CCArray::create();
        auto listView = ListView::create(emptyArr, 40.f, 356.f, 220.f);
        m_listLayer = GJListLayer::create(listView, "KDL", {0, 0, 0, 180}, 356.f, 220.f, 0);
        m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2);
        this->addChild(m_listLayer, 1);
        this->addChild(m_tabMenu, 2);

        return true;
	}

public:
    static KDLListLayer* create() {
        auto ret = new KDLListLayer();
        if (ret->init()) { 
            ret->autorelease();
            return ret;
        }
        
        delete ret;
        return nullptr;
    }

    static CCScene* scene() {
        auto s = CCScene::create();
        s->addChild(KDLListLayer::create());
        return s;
    }

    void loadTab(std::string url) {
        auto res = web::WebRequest().getSync(url);
        if (!res.ok()) {
            return;
        }
        auto jsonRes = res.json();
        if (!jsonRes) {
            return;
        }
        auto json = jsonRes.unwrap();
        applyJson(json);
    }

    void applyJson(matjson::Value const& json) {
        m_allIds.clear();
        auto result = json.asArray();
        if (!result) {
            return;
        }
        for (auto& entry : result.unwrap()) {
            auto id = entry.get<int>("id");
            if (!id) {
                continue;
            }
            m_allIds.push_back(id.unwrap());
        }
        fetchPage(0);
    }

    void fetchPage(int page) {
        int start = page * 10;
        int end = std::min(start + 10, (int)m_allIds.size());
        if (start >= (int)m_allIds.size()) return;

        std::string ids;
        for (int i = start; i < end; i++) {
            if (!ids.empty()) ids += ",";
            ids += std::to_string(m_allIds[i]);
        }

        auto glm = GameLevelManager::sharedState();
        glm->m_levelManagerDelegate = this;
        auto search = GJSearchObject::create(SearchType::Type19);
        search->m_searchQuery = ids;
        glm->getOnlineLevels(search);
    }

    void loadLevelsFinished(CCArray* levels, const char*) override {      
        auto winSize = CCDirector::get()->getWinSize();
        if (m_listLayer) {
            m_listLayer->removeFromParent();
            m_listLayer = nullptr;
        }

        auto listView = CustomListView::create(levels, BoomListType::Level, 190.f, 356.f);
        m_listLayer = GJListLayer::create(listView, "KDL", {0, 0, 0, 180}, 356.f, 190.f, 0);
        m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2);
        this->addChild(m_listLayer, 1);
    }

    void loadLevelsFailed(const char*) override {}

    void onPrevPage(CCObject*) {
        if (m_currentPage <= 0) return;
        m_currentPage--;
        if (m_listLayer) {
            m_listLayer->removeFromParent();
            m_listLayer = nullptr;
        }
        fetchPage(m_currentPage);
    }

    void onNextPage(CCObject*) {
        int totalPages = ((int)m_allIds.size() + 10 - 1) / 10;
        if (m_currentPage >= totalPages - 1) return;
        m_currentPage++;
        if (m_listLayer) {
            m_listLayer->removeFromParent();
            m_listLayer = nullptr;
        }
        fetchPage(m_currentPage);
    }

    void onExit() override {
        auto glm = GameLevelManager::sharedState();
        if (glm->m_levelManagerDelegate == this)
            glm->m_levelManagerDelegate = nullptr;
        CCLayer::onExit();
    }

    void onEnter() override {
        CCLayer::onEnter();
        GameLevelManager::sharedState()->m_levelManagerDelegate = this;
        if (!m_allIds.empty()) fetchPage(0);
    }

    void onTabPressed(CCObject* sender) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto urlObj = static_cast<CCString*>(btn->getUserObject());
        if (!urlObj) return;

        int i = 0;
        for (auto* child : CCArrayExt<CCNode*>(m_tabMenu->getChildren())) {
            if (i >= (int)m_tabBgsGreen.size()) break;
            bool active = child == btn;
            m_tabBgsGreen[i]->setVisible(!active);
            m_tabBgsBlue[i]->setVisible(active);
            i++;
        }

        loadTab(urlObj->getCString());
    }

	void onBack(CCObject*) {
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void onOpenLeaderboard(CCObject*) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, LeaderboardLayer::scene()));
    }

    void onOpenCreatorLeaderboard(CCObject*) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, CreatorLeaderboardLayer::scene()));
    }
};