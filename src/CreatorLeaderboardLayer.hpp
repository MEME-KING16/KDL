#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CreatorLeaderboardLayer : public CCLayer {
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
        auto titleLabel = CCLabelBMFont::create("K.D.L Creator Leaderboard", "goldFont.fnt");
        titleLabel->setScale(0.7f);
        titleLabel->setPosition({winSize.width / 2.f, winSize.height - 25.f});
        this->addChild(titleLabel);

        //back btn
        auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backButton = CCMenuItemSpriteExtra::create(
            backSprite, this, menu_selector(CreatorLeaderboardLayer::onBack)
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
        
        loadCreatorLeaderboard();

        return true;
    }

    void loadCreatorLeaderboard() {
        auto res = web::WebRequest().getSync("https://raw.githubusercontent.com/Therealkeanan00/Keanan-Demon-List-Updated-Geometry-Dash/main/CreatorLeaderboard.json");
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
                rank % 2 == 0 ? ccColor4B{0, 0, 0, 60} : ccColor4B{255, 255, 255, 15},
                listWidth, rowHeight
            );
            rowBackground->setPosition({0, y - rowHeight / 2.f});
            contentLayer->addChild(rowBackground);

            // rank label
            auto rankCreatorLeaderboardLayer = CCLabelBMFont::create(
                ("#" + std::to_string(rank)).c_str(), "bigFont.fnt"
            );
            rankCreatorLeaderboardLayer->setScale(0.35f);
            rankCreatorLeaderboardLayer->setAnchorPoint({0.f, 0.5f});
            rankCreatorLeaderboardLayer->setPosition({8.f, y});
            contentLayer->addChild(rankCreatorLeaderboardLayer);

            // name label
            auto nameCreatorLeaderboardLayer = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
            nameCreatorLeaderboardLayer->setScale(0.4f);
            nameCreatorLeaderboardLayer->setAnchorPoint({0.f, 0.5f});
            nameCreatorLeaderboardLayer->setPosition({45.f, y});
            contentLayer->addChild(nameCreatorLeaderboardLayer);

            // points label
            auto pointsCreatorLeaderboardLayer = CCLabelBMFont::create(
                (std::to_string(points) + " pts").c_str(), "bigFont.fnt"
            );
            pointsCreatorLeaderboardLayer->setScale(0.35f);
            pointsCreatorLeaderboardLayer->setAnchorPoint({1.f, 0.5f});
            pointsCreatorLeaderboardLayer->setPosition({listWidth - 8.f, y});
            contentLayer->addChild(pointsCreatorLeaderboardLayer);

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
    static CreatorLeaderboardLayer* create() {
        auto ret = new CreatorLeaderboardLayer();

        if (ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    static CCScene* scene() {
        auto s = CCScene::create();
        s->addChild(CreatorLeaderboardLayer::create());
        return s;
    }
};