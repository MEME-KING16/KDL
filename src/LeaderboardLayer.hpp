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

        // background
        auto background = CCSprite::create("GJ_gradientBG.png");
        background->setScaleX(winSize.width / background->getContentSize().width);
        background->setScaleY(winSize.height / background->getContentSize().height);
        background->setPosition(winSize / 2);
        background->setColor({0, 40, 80});
        this->addChild(background, -1);

        // top "stuff"
        auto topBar = CCLayerColor::create({0, 0, 0, 120}, winSize.width, 50.f);
        topBar->setPosition({0, winSize.height - 50.f});
        this->addChild(topBar);

        // title
        auto titleLabel = CCLabelBMFont::create("K.D.L Leaderboard", "goldFont.fnt");
        titleLabel->setScale(0.7f);
        titleLabel->setPosition({ winSize.width / 2.f, winSize.height - 25.f });
        this->addChild(titleLabel);

        //back btn
        auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backButton = CCMenuItemSpriteExtra::create(
            backSprite, this, menu_selector(LeaderboardLayer::onBack)
        );
        auto backMenu = CCMenu::create();
        backMenu->addChild(backButton);
        backMenu->setPosition({25.f, winSize.height - 25.f});
        this->addChild(backMenu);

        auto emptyArray = CCArray::create();
        auto listView = ListView::create(emptyArray, 40.f, 356.f, 220.f);
        m_listLayer = GJListLayer::create(listView, "", {0, 0, 0, 180}, 356.f, 220.f, 0);
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
        float rowHeight = 40.f;
        float listWidth = 356.f;
        float listHeight = 220.f;

        float totalHeight = entries.size() * rowHeight;
        float contentHeight = std::max(totalHeight, listHeight);

        auto contentLayer = CCLayer::create();
        contentLayer->setContentSize({listWidth, contentHeight});

        int rank = 1;
        for (auto& entry : entries) {
            auto name = entry.get<std::string>("name").unwrapOr("Unknown");
            auto points = entry.get<int>("points").unwrapOr(0);
            auto id = entry.get<int>("id").unwrapOr(0);

            float y = contentHeight - rank * rowHeight + rowHeight / 2.f;

            // row background
            auto rowBackground = CCLayerColor::create(
                rank % 2 == 0 ? ccColor4B{ 0, 0, 0, 60 } : ccColor4B{ 255, 255, 255, 15 },
                listWidth, rowHeight
            );
            rowBackground->setPosition({ 0, y - rowHeight / 2.f });
            contentLayer->addChild(rowBackground);

            // rank label
            auto rankLeaderboardLayer = CCLabelBMFont::create(
                ("#" + std::to_string(rank)).c_str(), "bigFont.fnt"
            );
            rankLeaderboardLayer->setScale(0.35f);
            rankLeaderboardLayer->setAnchorPoint({ 0.f, 0.5f });
            rankLeaderboardLayer->setPosition({ 8.f, y });
            contentLayer->addChild(rankLeaderboardLayer);

            // name label
            auto nameLeaderboardLayer = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
            nameLeaderboardLayer->setScale(0.4f);
            nameLeaderboardLayer->setAnchorPoint({0.f, 0.5f});
            nameLeaderboardLayer->setPosition({45.f, y});
            contentLayer->addChild(nameLeaderboardLayer);

            // points label
            auto pointsLeaderboardLayer = CCLabelBMFont::create(
                (std::to_string(points) + " pts").c_str(), "bigFont.fnt"
            );
            pointsLeaderboardLayer->setScale(0.35f);
            pointsLeaderboardLayer->setAnchorPoint({1.f, 0.5f});
            pointsLeaderboardLayer->setPosition({listWidth - 8.f, y});
            contentLayer->addChild(pointsLeaderboardLayer);

            rank++;
        }

        auto scrollLayer = ScrollLayer::create({listWidth, listHeight});
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

        scrollLayer->setPosition(m_listLayer->getPosition());
        this->addChild(scrollLayer);
    }


    void onBack(CCObject*) {
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