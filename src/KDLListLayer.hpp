#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "CreatorLeaderboardLayer.hpp"
#include "LeaderboardLayer.hpp"
#include "KDLBackground.hpp"
#include "KDLCreditsPopup.hpp"

#define BASE_URL "https://the-kdl.com/api/list/"

using namespace geode::prelude;

struct KDLLevelEntry {
    int id;
    std::string name;
};

class KDLListLayer : public CCLayer, public LevelManagerDelegate {
protected:
    std::vector<int> m_allIds;
    std::vector<KDLLevelEntry> m_allLevels;
    Ref<CCMenu> m_searchBarMenu;
    TextInput* m_searchBar = nullptr;
    std::string m_query;
    GJListLayer* m_listLayer = nullptr;
    CCMenu* m_tabMenu = nullptr;
    std::vector<CCScale9Sprite*> m_tabBkgsGreen;
    std::vector<CCScale9Sprite*> m_tabBkgsBlue;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    CCMenuItemSpriteExtra* m_discordButton = nullptr;
    CCMenuItemSpriteExtra* m_siteButton = nullptr;
    std::vector<CCMenuItemSpriteExtra*> m_tabButtons;
    std::vector<CCLabelBMFont*> m_tabLabels;
    bool m_platformer = false;
    int m_currentTab = 2;
    CCNode* m_modeNode = nullptr;
    CCSprite* m_modeIcon = nullptr;
    int m_currentPage = 0;
    std::string m_currentUrl;

    bool init() {
        if (!CCLayer::init()) return false;
        if (Mod::get()->getSettingValue<bool>("custom-menu-music")) {
            GameManager::sharedState()->fadeInMusic(geode::utils::string::pathToString(Mod::get()->getResourcesDir() / "music.mp3").c_str());
        }
        
		auto winSize = CCDirector::get()->getWinSize();
        this->setID("kdl-list-layer");
        this->setKeyboardEnabled(true);
        this->setKeypadEnabled(true);
		
        // background
        addKDLBackground(this, {40, 0, 80});


        // back arrow
		auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backButton = CCMenuItemSpriteExtra::create(
            backSprite, this, menu_selector(KDLListLayer::onBack)
        );
        backButton->setID("back-button");

        // Leaderboard button

        // node
        auto leaderboardNode = CCNode::create();
        leaderboardNode->setContentSize({30.0f, 30.0f});

        // btn bkg
        auto leaderboardBkg = CCScale9Sprite::create("GJ_button_01.png");
        leaderboardBkg->setContentSize({30.0f, 30.0f});
        leaderboardBkg->setPosition({15.0f, 15.0f});
        leaderboardNode->addChild(leaderboardBkg);

        auto trophySprite = CCSprite::createWithSpriteFrameName("rankIcon_1_001.png");
        trophySprite->setScale(0.6f);
        trophySprite->setPosition({15.0f, 15.0f});
        leaderboardNode->addChild(trophySprite);
        auto leaderboardButton = CCMenuItemSpriteExtra::create(
            leaderboardNode, this, menu_selector(KDLListLayer::onOpenLeaderboard)
        );

        leaderboardNode->setID("leaderboard-node");
        leaderboardBkg->setID("button-bkg");
        leaderboardButton->setID("leaderboard-button");

        // Creator Leaderboard button

        // node
        auto creatorLeaderboardNode = CCNode::create();
        creatorLeaderboardNode->setContentSize({30.0f, 30.0f});
        
        // btn bkg
        auto creatorLeaderboardBkg = CCScale9Sprite::create("GJ_button_01.png");
        creatorLeaderboardBkg->setContentSize({30.0f, 30.0f});
        creatorLeaderboardBkg->setPosition({15.0f, 15.0f});
        creatorLeaderboardNode->addChild(creatorLeaderboardBkg);

        
        auto creatorSprite = CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
        creatorSprite->setScale(0.6f);
        creatorSprite->setPosition({15.0f, 15.0f});
        creatorLeaderboardNode->addChild(creatorSprite);
        auto creatorLeaderboardButton = CCMenuItemSpriteExtra::create(
            creatorLeaderboardNode, this, menu_selector(KDLListLayer::onOpenCreatorLeaderboard)
        );

        creatorLeaderboardNode->setID("creator-leaderboard-node");
        creatorLeaderboardBkg->setID("button-bkg");
        creatorLeaderboardButton->setID("creator-leaderboard-button");

        // Credits button

        auto creditsNode = CCNode::create();
        creditsNode->setContentSize({30.0f, 30.0f});

        auto creditsBkg = CCScale9Sprite::create("GJ_button_01.png");
        creditsBkg->setContentSize({30.0f, 30.0f});
        creditsBkg->setPosition({15.0f, 15.0f});
        creditsNode->addChild(creditsBkg);

        auto creditsSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        creditsSprite->setScale(0.6f);
        creditsSprite->setPosition({15.0f, 15.0f});
        creditsNode->addChild(creditsSprite);
        auto creditsButton = CCMenuItemSpriteExtra::create(
            creditsNode, this, menu_selector(KDLListLayer::onOpenCredits)
        );

        creditsNode->setID("credits-node");
        creditsBkg->setID("button-bkg");
        creditsButton->setID("credits-button");

        // list type toggle button

        auto modeNode = CCNode::create();
        modeNode->setContentSize({30.0f, 30.0f});

        auto modeBkg = CCScale9Sprite::create("GJ_button_01.png");
        modeBkg->setContentSize({30.0f, 30.0f});
        modeBkg->setPosition({15.0f, 15.0f});
        modeNode->addChild(modeBkg);
        m_modeNode = modeNode;

        auto modeButton = CCMenuItemSpriteExtra::create(
            modeNode, this, menu_selector(KDLListLayer::onModeToggle)
        );

        modeNode->setID("mode-node");
        modeBkg->setID("button-bkg");
        modeButton->setID("mode-toggle-button");

        // top "stuff"
		auto topMenu = CCMenu::create();
        topMenu->setID("top-menu");
        topMenu->addChild(backButton);
        topMenu->addChild(leaderboardButton);
        topMenu->addChild(creatorLeaderboardButton);
        topMenu->addChild(creditsButton);
        topMenu->addChild(modeButton);
        topMenu->setPosition({0.0f, 0.0f});
        backButton->setPosition({25.0f, winSize.height - 25.0f});
        leaderboardButton->setPosition({25.0f, winSize.height - 65.0f});
        creatorLeaderboardButton->setPosition({25.0f, winSize.height - 100.0f});
        creditsButton->setPosition({25.0f, winSize.height - 135.0f});
        modeButton->setPosition({25.0f, winSize.height - 170.0f});
        this->addChild(topMenu, 2);

        // not so top "stuff"  
        m_tabMenu = CCMenu::create();
        m_tabMenu->setID("tab-menu");
        m_tabMenu->setPosition({0.0f, 0.0f});
        m_tabMenu->setContentSize({winSize.width, 50.0f});

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

        m_prevButton->setID("prev-page-button");
        m_nextButton->setID("next-page-button");
        m_prevButton->setPosition({winSize.width / 2.0f - 210.0f, winSize.height / 2.0f});
        m_nextButton->setPosition({winSize.width / 2.0f + 210.0f, winSize.height / 2.0f});
        m_tabMenu->addChild(m_prevButton);
        m_tabMenu->addChild(m_nextButton);

        auto refreshSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        auto refreshBtn = CCMenuItemSpriteExtra::create(
            refreshSpr, this, menu_selector(KDLListLayer::onRefresh)
        );

        auto refreshMenu = CCMenu::create();
        refreshMenu->addChild(refreshBtn);
        refreshMenu->setPosition({ winSize.width - 25.f, winSize.height - 25.f });
        this->addChild(refreshMenu, 2);

        const char* tabLabels[] = {
            "Nerfed\nVerified",
            "Nerfed\nUnverified",
            "Buffed\nVerified",
            "Buffed\nUnverified",
        };

        float tabW = 75.0f;
        float tabGap = 8.0f;
        float totalTabW = 4 * tabW + 3 * tabGap;
        float tabStartX = winSize.width / 2.0f - totalTabW / 2.0f + tabW / 2.0f;

        int defaultTab = 2;
        int tabIndex = 0;
        for (auto& tabLabel : tabLabels) {
            auto lbl = CCLabelBMFont::create(tabLabel, "bigFont.fnt");
            lbl->setID("label");
            lbl->setScale(0.3f);
            m_tabLabels.push_back(lbl);

            bool active = tabIndex == defaultTab;

            auto tabBkgGreen = CCScale9Sprite::create("GJ_button_01.png");
            tabBkgGreen->setID("bkg-green");
            tabBkgGreen->setContentSize({75.0f, 40.0f});
            tabBkgGreen->setPosition({37.5f, 20.0f});
            tabBkgGreen->setVisible(!active);
            m_tabBkgsGreen.push_back(tabBkgGreen);

            auto tabBkgBlue = CCScale9Sprite::create("GJ_button_02.png");
            tabBkgBlue->setID("bkg-blue");
            tabBkgBlue->setContentSize({75.0f, 40.0f});
            tabBkgBlue->setPosition({37.5f, 20.0f});
            tabBkgBlue->setVisible(active);
            m_tabBkgsBlue.push_back(tabBkgBlue);

            auto node = CCNode::create();
            node->setID(fmt::format("tab-node-{}", tabIndex));
            node->setContentSize({75.0f, 40.0f});
            lbl->setPosition({37.5f, 20.0f});
            node->addChild(tabBkgGreen);
            node->addChild(tabBkgBlue);
            node->addChild(lbl);

            auto button = CCMenuItemSpriteExtra::create(
                node, this, menu_selector(KDLListLayer::onTabPressed)
            );
            button->setID(fmt::format("tab-button-{}", tabIndex));
            button->setTag(tabIndex);
            button->setPosition({tabStartX + tabIndex * (tabW + tabGap), 25.0f});
            m_tabMenu->addChild(button);
            m_tabButtons.push_back(button);
            tabIndex++;
        }

        auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
        m_discordButton = CCMenuItemSpriteExtra::create(
            discordSprite, this, menu_selector(KDLListLayer::onDiscordButton)
        );

        m_discordButton->setPosition({winSize.width / 2.0f + 210.0f, 23.0f});

        m_tabMenu->addChild(m_discordButton);


        auto siteSprite = CCSprite::createWithSpriteFrameName("geode.loader/homepage.png");
        m_siteButton = CCMenuItemSpriteExtra::create(
            siteSprite, this, menu_selector(KDLListLayer::onSiteButton)
        );

        m_siteButton->setPosition({winSize.width / 2.0f + 245.0f, 23.0f});

        m_tabMenu->addChild(m_siteButton);





        m_listLayer = GJListLayer::create(nullptr, "KDL", {0, 0, 0, 180}, 356.0f, 220.0f, 0);
        m_listLayer->setID("list-layer");
        m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2);
        this->addChild(m_listLayer, 1);
        this->addChild(m_tabMenu, 2);
        // search bar "borrowed" from the DDL Integration
        m_searchBarMenu = CCMenu::create();
        m_searchBarMenu->setContentSize({356.0f, 30.0f});
        m_searchBarMenu->setPosition({0.0f, 190.0f});
        m_searchBarMenu->setID("search-bar-menu");
        m_listLayer->addChild(m_searchBarMenu);

        auto searchBackground = CCLayerColor::create({194, 114, 62, 255}, 356.0f, 30.0f);
        searchBackground->setID("search-bar-background");
        m_searchBarMenu->addChild(searchBackground);

        auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
        searchSprite->setScale(0.7f);
        auto searchButton = CCMenuItemSpriteExtra::create(
            searchSprite, this, menu_selector(KDLListLayer::onSearch)
        );
        searchButton->setPosition({337.0f, 15.0f});
        searchButton->setID("search-button");
        m_searchBarMenu->addChild(searchButton);

        m_searchBar = TextInput::create(310.0f, "Search...");
        m_searchBar->setPosition({165.0f, 15.0f});
        m_searchBar->setTextAlign(TextInputAlign::Left);
        auto inputNode = m_searchBar->getInputNode();
        inputNode->setLabelPlaceholderScale(0.4f);
        inputNode->setMaxLabelScale(0.4f);
        auto searchBgSprite = m_searchBar->getBGSprite();
        searchBgSprite->setContentSize({620.0f, 40.0f});
        searchBgSprite->setScale(0.5f);
        m_searchBar->setID("search-bar");
        m_searchBarMenu->addChild(m_searchBar);

        this->setTouchEnabled(true);

        updateModeIcon();
        selectTab(defaultTab);

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
        m_currentUrl = url;
        auto res = web::WebRequest().getSync(url);
        if (!res.ok()) {
            return;
        }
        auto jsonRes = res.json();
        if (!jsonRes) {
            return;
        }
        auto json = jsonRes.unwrap();
        applyJson(json["levels"]);
    }

    void applyJson(matjson::Value const& json) {
        m_allLevels.clear();
        auto result = json.asArray();
        if (!result) {
            return;
        }
        for (auto& entry : result.unwrap()) {
            auto id = entry.get<int>("id");
            if (!id) {
                continue;
            }
            m_allLevels.push_back({id.unwrap(), entry.get<std::string>("name").unwrapOr("")});
        }
        applyFilter();
    }

    void applyFilter() {
        m_allIds.clear();
        m_currentPage = 0;
        auto query = utils::string::toLower(m_query);
        for (auto& lvl : m_allLevels) {
            if (query.empty()
                || utils::string::toLower(lvl.name).find(query) != std::string::npos
                || std::to_string(lvl.id).find(query) != std::string::npos) {
                m_allIds.push_back(lvl.id);
            }
        }
        updatePageArrows();
        fetchPage(0);
    }

    void onSearch(CCObject*) {
        if (m_searchBar) m_searchBar->defocus();

        auto query = m_searchBar->getString();
        if (m_query != query) {
            m_query = query;
            applyFilter();
        }
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(this, -600, false);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        if (m_searchBar && m_searchBarMenu && m_searchBarMenu->isVisible()) {
            auto localPos = m_searchBarMenu->convertToNodeSpace(touch->getLocation());
            if (!m_searchBar->boundingBox().containsPoint(localPos)) {
                m_searchBar->defocus();
            }
        }
        return false;
    }
    void fetchPage(int page) {
        int start = page * 10;
        int end = std::min(start + 10, (int)m_allIds.size());
        if (start >= (int)m_allIds.size()) {
            loadLevelsFinished(CCArray::create(), "");
            return;
        }

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
        if (auto listView = m_listLayer->m_listView) {
            listView->removeFromParent();
            listView->release();
            m_listLayer->m_listView = nullptr;
        }

        auto listView = CustomListView::create(levels, BoomListType::Level, 190.0f, 356.0f);
        listView->retain();
        m_listLayer->addChild(listView, 6, 9);
        m_listLayer->m_listView = listView;
    }

    void loadLevelsFailed(const char*) override {}

    void updatePageArrows() {
        int totalPages = ((int)m_allIds.size() + 10 - 1) / 10;
        if (m_prevButton) m_prevButton->setVisible(m_currentPage > 0);
        if (m_nextButton) m_nextButton->setVisible(m_currentPage < totalPages - 1);
    }

    void onPrevPage(CCObject*) {
        if (m_currentPage <= 0) return;
        m_currentPage--;
        fetchPage(m_currentPage);
        updatePageArrows();
    }

    void onNextPage(CCObject*) {
        int totalPages = ((int)m_allIds.size() + 10 - 1) / 10;
        if (m_currentPage >= totalPages - 1) return;
        m_currentPage++;
        fetchPage(m_currentPage);
        updatePageArrows();
    }

    void onExit() override {
        if (m_searchBar) m_searchBar->defocus();
        auto glm = GameLevelManager::sharedState();
        if (glm->m_levelManagerDelegate == this)
            glm->m_levelManagerDelegate = nullptr;
        CCLayer::onExit();
    }

    void onRefresh(CCObject*) {
        if (!m_currentUrl.empty())
            loadTab(m_currentUrl);
    }

    void onEnter() override {
        CCLayer::onEnter();
        GameLevelManager::sharedState()->m_levelManagerDelegate = this;
    }

    std::string urlForTab(int idx) {
        static const char* classicUrls[] = {
            BASE_URL "nerfverif/levels",
            BASE_URL "nerfunverif/levels",
            BASE_URL "buffverif/levels",
            BASE_URL "buffunverif/levels",
        };
        static const char* platUrls[] = {
            "",
            "",
            BASE_URL "platverif/levels",
            BASE_URL "platunverif/levels",
        };
        return m_platformer ? platUrls[idx] : classicUrls[idx];
    }

    void selectTab(int idx) {
        if (m_platformer && idx < 2) return;
        m_currentTab = idx;

        for (int i = 0; i < (int)m_tabButtons.size(); i++) {
            bool active = i == idx;
            m_tabBkgsGreen[i]->setVisible(!active);
            m_tabBkgsBlue[i]->setVisible(active);
        }

        m_query = "";
        if (m_searchBar) {
            m_searchBar->setString("");
            m_searchBar->defocus();
        }

        loadTab(urlForTab(idx));
    }

    void onTabPressed(CCObject* sender) {
        selectTab(static_cast<CCMenuItemSpriteExtra*>(sender)->getTag());
    }

    void updateModeIcon() {
        if (m_modeIcon) m_modeIcon->removeFromParent();
        m_modeIcon = CCSprite::createWithSpriteFrameName(
            m_platformer ? "GJ_bigMoon_001.png" : "GJ_bigStar_001.png"
        );
        if (!m_modeIcon) return;
        m_modeIcon->setScale(20.0f / m_modeIcon->getContentHeight());
        m_modeIcon->setPosition({15.0f, 15.0f});
        m_modeIcon->setID("mode-icon");
        m_modeNode->addChild(m_modeIcon);
    }

    void onModeToggle(CCObject*) {
        m_platformer = !m_platformer;
        updateModeIcon();

        ccColor3B col = m_platformer ? ccColor3B{90, 90, 90} : ccColor3B{255, 255, 255};
        for (int i = 0; i < 2; i++) {
            m_tabButtons[i]->setEnabled(!m_platformer);
            m_tabBkgsGreen[i]->setColor(col);
            m_tabBkgsBlue[i]->setColor(col);
            m_tabLabels[i]->setColor(col);
            m_tabLabels[i]->setOpacity(m_platformer ? 160 : 255);
        }

        selectTab(m_platformer && m_currentTab < 2 ? 2 : m_currentTab);
    }

    void onDiscordButton(CCObject* sender) {
        geode::utils::web::openLinkInBrowser("https://discord.gg/fpKkEb7SD5");
    }

    void onSiteButton(CCObject* sender) {
        geode::utils::web::openLinkInBrowser("https://the-kdl.com/");
    }

	void onBack(CCObject*) {
        if (m_searchBar) m_searchBar->defocus();
        GameManager::sharedState()->fadeInMenuMusic();
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void keyBackClicked() override {
        GameManager::sharedState()->fadeInMenuMusic();
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void onOpenLeaderboard(CCObject*) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, LeaderboardLayer::scene()));
    }

    void onOpenCreatorLeaderboard(CCObject*) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, CreatorLeaderboardLayer::scene()));
    }

    void onOpenCredits(CCObject*) {
        KDLCreditsPopup::create()->show();
    }
};