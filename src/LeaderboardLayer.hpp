#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LeaderboardLayer : public CCLayer {
protected:
    CCNode* m_listNode = nullptr;
    GJListLayer* m_listLayer = nullptr;

    bool init() {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::get()->getWinSize();
        this->setKeyboardEnabled(true);
        this->setKeypadEnabled(true);

        // background
        auto background = CCSprite::create("GJ_gradientBG.png");
        background->setScaleX(winSize.width / background->getContentSize().width);
        background->setScaleY(winSize.height / background->getContentSize().height);
        background->setPosition(winSize / 2);
        background->setColor({0, 40, 80});
        this->addChild(background, -1);

        // top "stuff"
        auto topBar = CCLayerColor::create({0, 0, 0, 120}, winSize.width, 50.0f);
        topBar->setPosition({0, winSize.height - 50.0f});
        this->addChild(topBar);

        // title
        auto titleLabel = CCLabelBMFont::create("K.D.L List Leaderboard", "goldFont.fnt");
        titleLabel->setScale(0.7f);
        titleLabel->setPosition({ winSize.width / 2.0f, winSize.height - 25.0f });
        this->addChild(titleLabel);

        //back btn
        auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backButton = CCMenuItemSpriteExtra::create(
            backSprite, this, menu_selector(LeaderboardLayer::onBack)
        );
        auto backMenu = CCMenu::create();
        backMenu->addChild(backButton);
        backMenu->setPosition({25.0f, winSize.height - 25.0f});
        this->addChild(backMenu);
        
        auto refreshSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        auto refreshBtn = CCMenuItemSpriteExtra::create(
            refreshSpr, this, menu_selector(LeaderboardLayer::onRefresh)
        );

        auto refreshMenu = CCMenu::create();
        refreshMenu->addChild(refreshBtn);
        refreshMenu->setPosition({ winSize.width - 25.f, winSize.height - 25.f });
        this->addChild(refreshMenu);

        auto emptyArray = CCArray::create();
        auto listView = ListView::create(emptyArray, 40.0f, 356.0f, 220.0f);
        m_listLayer = GJListLayer::create(listView, "", {0, 0, 0, 180}, 356.0f, 220.0f, 0);
        m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2);
        this->addChild(m_listLayer);
        
        loadLeaderboard();

        return true;
    }

    void loadLeaderboard() {
        auto res = web::WebRequest().getSync("https://raw.githubusercontent.com/Therealkeanan00/Keanan-Demon-List-Updated-Geometry-Dash/main/Leaderboard.json");
        if (!res.ok()) return;

        auto jsonRes = res.json();
        if (!jsonRes) return;

        auto json = jsonRes.unwrap();
        auto arr = json.asArray();
        if (!arr) return;

        auto winSize = CCDirector::get()->getWinSize();

        auto entries = arr.unwrap();
        float rowHeight = 40.0f;
        float listWidth = 356.0f;
        float listHeight = 220.0f;

        float totalHeight = entries.size() * rowHeight;
        float contentHeight = std::max(totalHeight, listHeight);

        auto contentLayer = CCLayer::create();
        contentLayer->setContentSize({listWidth, contentHeight});

        int rank = 1;
        for (auto& entry : entries) {
            auto name = entry.get<std::string>("name").unwrapOr("Unknown");
            auto points = entry.get<int>("points").unwrapOr(0);
            auto id = entry.get<int>("id").unwrapOr(0);

            float y = contentHeight - rank * rowHeight + rowHeight / 2.0f;

            // row background
            auto rowBackground = CCLayerColor::create(
                rank % 2 == 0 ? ccColor4B{ 0, 0, 0, 60 } : ccColor4B{ 255, 255, 255, 15 },
                listWidth, rowHeight
            );
            rowBackground->setPosition({ 0, y - rowHeight / 2.0f });
            contentLayer->addChild(rowBackground);

            // rank label
            auto rankLeaderboardLayer = CCLabelBMFont::create(
                ("#" + std::to_string(rank)).c_str(), "bigFont.fnt"
            );
            rankLeaderboardLayer->setScale(0.35f);
            rankLeaderboardLayer->setAnchorPoint({ 0.0f, 0.5f });
            rankLeaderboardLayer->setPosition({ 8.0f, y });
            contentLayer->addChild(rankLeaderboardLayer);

            // name label
            auto nameLeaderboardLayer = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
            nameLeaderboardLayer->setScale(0.4f);
            nameLeaderboardLayer->setAnchorPoint({0.0f, 0.5f});
            nameLeaderboardLayer->setPosition({45.0f, y});
            contentLayer->addChild(nameLeaderboardLayer);

            // points label
            auto pointsLeaderboardLayer = CCLabelBMFont::create(std::to_string(points).c_str(), "bigFont.fnt");
            pointsLeaderboardLayer->setScale(0.35f);
            pointsLeaderboardLayer->setAnchorPoint({1.0f, 0.5f});
            pointsLeaderboardLayer->setPosition({listWidth - 28.0f, y});
            contentLayer->addChild(pointsLeaderboardLayer);

            auto pointsIcon = CCSprite::createWithSpriteFrameName("difficulty_10_btn_001.png");
            pointsIcon->setScale(0.4f);
            pointsIcon->setAnchorPoint({1.0f, 0.5f});
            pointsIcon->setPosition({listWidth - 8.0f, y});
            contentLayer->addChild(pointsIcon);

            rank++;
        }

        auto scrollLayer = ScrollLayer::create({listWidth, listHeight - 8.0f});
        scrollLayer->m_contentLayer->setContentSize({listWidth, contentHeight});
        scrollLayer->m_contentLayer->addChild(contentLayer);

        if (contentHeight > listHeight) {
            scrollLayer->m_contentLayer->setPositionY(listHeight - contentHeight);
        }

        if (m_listLayer) {
            m_listLayer->removeFromParent();
        }

        m_listLayer = GJListLayer::create(
            ListView::create(CCArray::create(), rowHeight, listWidth, listHeight),
            "", { 0, 0, 0, 180 }, listWidth, listHeight, 0
        );
        m_listLayer->setPosition(winSize / 2 - m_listLayer->getScaledContentSize() / 2);
        this->addChild(m_listLayer);

        scrollLayer->setPosition(m_listLayer->getPosition() + CCPoint{0.0f, 4.0f});
        this->addChild(scrollLayer);
    }

    void onRefresh(CCObject*) {
        loadLeaderboard();
    }

    void onBack(CCObject*) {
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void keyBackClicked() override {
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

public:
    static LeaderboardLayer* create() {
        auto ret = new LeaderboardLayer();

        if (ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    static CCScene* scene() {
        auto s = CCScene::create();
        s->addChild(LeaderboardLayer::create());
        return s;
    }
};