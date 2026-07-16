#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <thread>
#include <map>
#include "KDLBackground.hpp"

using namespace geode::prelude;

#define KDL_CREDITS_URL "https://raw.githubusercontent.com/Therealkeanan00/Keanan-Demon-List-Updated-Geometry-Dash/main/Credits.json"

struct KDLCreditEntry {
    int accountId;
    std::string name;
    std::string tab;
};

class KDLCreditsPopup : public geode::Popup {
protected:
    static constexpr float LIST_WIDTH = 320.0f;
    static constexpr float LIST_HEIGHT = 175.0f;
    static constexpr float HEADER_HEIGHT = 30.0f;
    static constexpr float PLAYER_HEIGHT = 50.0f;

    ScrollLayer* m_scroll = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    int m_requestId = 0;
    // SimplePlayer nodes per account id, updated once icon data arrives
    std::map<int, std::vector<Ref<SimplePlayer>>> m_playerNodes;

    bool init() {
        if (!Popup::init(380.0f, 250.0f)) return false;
        setTitle("K.D.L. Credits");

        auto center = m_mainLayer->getContentSize() / 2.0f;

        auto listBg = CCScale9Sprite::create("square02b_001.png");
        listBg->setContentSize({LIST_WIDTH + 10.0f, LIST_HEIGHT + 10.0f});
        listBg->setColor({0, 0, 0});
        listBg->setOpacity(120);
        listBg->setPosition({center.width, center.height - 12.0f});
        m_mainLayer->addChild(listBg);

        m_scroll = ScrollLayer::create({LIST_WIDTH, LIST_HEIGHT});
        m_scroll->setPosition({center.width - LIST_WIDTH / 2.0f, center.height - 12.0f - LIST_HEIGHT / 2.0f});
        m_mainLayer->addChild(m_scroll);

        m_loadingCircle = LoadingCircle::create();
        m_loadingCircle->setParentLayer(m_mainLayer);
        m_loadingCircle->setPosition(center - m_mainLayer->getPosition());
        m_loadingCircle->show();

        loadCredits();
        return true;
    }

    void loadCredits() {
        int requestId = ++m_requestId;
        Ref<KDLCreditsPopup> self = this;

        std::thread([self, requestId] {
            auto res = web::WebRequest().getSync(KDL_CREDITS_URL);

            std::vector<KDLCreditEntry> entries;
            bool ok = false;

            if (res.ok()) {
                if (auto jsonRes = res.json(); jsonRes.isOk()) {
                    if (auto arr = jsonRes.unwrap().asArray(); arr.isOk()) {
                        ok = true;
                        for (auto& entry : arr.unwrap()) {
                            entries.push_back({
                                entry.get<int>("id").unwrapOr(0),
                                entry.get<std::string>("name").unwrapOr("Unknown"),
                                entry.get<std::string>("tab").unwrapOr("Credits")
                            });
                        }
                    }
                }
            }

            Loader::get()->queueInMainThread([self, requestId, ok, entries = std::move(entries)] {
                if (requestId != self->m_requestId) return;
                self->m_loadingCircle->setVisible(false);
                if (!ok) {
                    Notification::create("Failed to load credits! Please try again later.. Please Message Keanan about this issue if it persists.", NotificationIcon::Error)->show();
                    return;
                }
                self->populate(entries);
            });
        }).detach();
    }

    void populate(std::vector<KDLCreditEntry> const& entries) {
        auto content = m_scroll->m_contentLayer;
        content->removeAllChildren();

        float totalHeight = 0.0f;
        std::string lastTab;
        for (auto& entry : entries) {
            if (entry.tab != lastTab) {
                totalHeight += HEADER_HEIGHT;
                lastTab = entry.tab;
            }
            totalHeight += PLAYER_HEIGHT;
        }
        float contentHeight = std::max(totalHeight, LIST_HEIGHT);
        content->setContentSize({LIST_WIDTH, contentHeight});

        float y = contentHeight;
        lastTab.clear();
        int rowIndex = 0;
        m_playerNodes.clear();
        for (auto& entry : entries) {
            if (entry.tab != lastTab) {
                lastTab = entry.tab;
                y -= HEADER_HEIGHT;

                auto headerRow = CCNode::create();
                headerRow->setContentSize({LIST_WIDTH, HEADER_HEIGHT});
                headerRow->setAnchorPoint({0.0f, 0.0f});
                headerRow->setPosition({0.0f, y});
                content->addChild(headerRow);

                auto headerBg = CCLayerColor::create(ccc4(0, 0, 0, 110), LIST_WIDTH, HEADER_HEIGHT);
                headerBg->setPosition({0.0f, 0.0f});
                headerRow->addChild(headerBg);

                auto header = CCLabelBMFont::create(entry.tab.c_str(), "bigFont.fnt");
                header->limitLabelWidth(LIST_WIDTH - 60.0f, 0.5f, 0.1f);
                header->setPosition({LIST_WIDTH / 2.0f, HEADER_HEIGHT / 2.0f});
                headerRow->addChild(header);
            }

            y -= PLAYER_HEIGHT;

            auto row = CCNode::create();
            row->setContentSize({LIST_WIDTH, PLAYER_HEIGHT});
            row->setAnchorPoint({0.0f, 0.0f});
            row->setPosition({0.0f, y});
            content->addChild(row);

            auto rowBg = CCLayerColor::create(
                ccc4(0, 0, 0, rowIndex++ % 2 == 0 ? 60 : 30), LIST_WIDTH, PLAYER_HEIGHT
            );
            rowBg->setPosition({0.0f, 0.0f});
            row->addChild(rowBg);

            auto player = SimplePlayer::create(1);
            player->updatePlayerFrame(1, IconType::Cube);
            player->setPosition({35.0f, PLAYER_HEIGHT / 2.0f});
            row->addChild(player);
            if (entry.accountId > 0) m_playerNodes[entry.accountId].push_back(player);

            auto nameLabel = CCLabelBMFont::create(entry.name.c_str(), "goldFont.fnt");
            nameLabel->setScale(0.8f);
            nameLabel->limitLabelWidth(LIST_WIDTH - 100.0f, 0.8f, 0.1f);
            nameLabel->setAnchorPoint({0.0f, 0.5f});

            auto menu = CCMenu::create();
            menu->setContentSize({LIST_WIDTH, PLAYER_HEIGHT});
            menu->setAnchorPoint({0.0f, 0.0f});
            menu->setPosition({0.0f, 0.0f});
            auto nameBtn = CCMenuItemSpriteExtra::create(
                nameLabel, this, menu_selector(KDLCreditsPopup::onProfile)
            );
            nameBtn->setTag(entry.accountId);
            nameBtn->setAnchorPoint({0.0f, 0.5f});
            nameBtn->setPosition({65.0f, PLAYER_HEIGHT / 2.0f});
            menu->addChild(nameBtn);
            row->addChild(menu);
        }

        m_scroll->moveToTop();
        fetchPlayerIcons();
    }

    static std::map<int, std::string> parseRobtopResponse(std::string const& body) {
        std::map<int, std::string> out;
        std::vector<std::string> parts;
        size_t start = 0;
        while (start <= body.size()) {
            auto end = body.find(':', start);
            if (end == std::string::npos) { parts.push_back(body.substr(start)); break; }
            parts.push_back(body.substr(start, end - start));
            start = end + 1;
        }
        for (size_t i = 0; i + 1 < parts.size(); i += 2) {
            out[utils::numFromString<int>(parts[i]).unwrapOr(-1)] = parts[i + 1];
        }
        return out;
    }

    void fetchPlayerIcons() {
        int requestId = m_requestId;
        Ref<KDLCreditsPopup> self = this;

        std::vector<int> ids;
        for (auto& [accountId, nodes] : m_playerNodes) ids.push_back(accountId);

        std::thread([self, requestId, ids = std::move(ids)] {
            for (int accountId : ids) {
                auto res = web::WebRequest()
                    .bodyString(fmt::format("targetAccountID={}&secret=Wmfd2893gb7", accountId))
                    .header("Content-Type", "application/x-www-form-urlencoded")
                    .postSync("https://www.boomlings.com/database/getGJUserInfo20.php");

                if (!res.ok()) continue;
                auto body = res.string().unwrapOr("");
                if (body.empty() || body == "-1") continue;

                auto data = parseRobtopResponse(body);
                int cube = utils::numFromString<int>(data[21]).unwrapOr(1);
                int color1 = utils::numFromString<int>(data[10]).unwrapOr(0);
                int color2 = utils::numFromString<int>(data[11]).unwrapOr(0);
                bool glow = utils::numFromString<int>(data[28]).unwrapOr(0) > 0;
                int glowColor = utils::numFromString<int>(data[51]).unwrapOr(-1);
                if (glowColor < 0) glowColor = color2;

                Loader::get()->queueInMainThread([self, requestId, accountId, cube, color1, color2, glow, glowColor] {
                    if (requestId != self->m_requestId) return;
                    auto it = self->m_playerNodes.find(accountId);
                    if (it == self->m_playerNodes.end()) return;

                    auto gm = GameManager::sharedState();
                    for (auto& player : it->second) {
                        player->updatePlayerFrame(cube, IconType::Cube);
                        player->setColors(gm->colorForIdx(color1), gm->colorForIdx(color2));
                        if (glow) player->setGlowOutline(gm->colorForIdx(glowColor));
                    }
                });
            }
        }).detach();
    }

// btw keanan REALLY likes femboys ( expecially Glownick :3 )

    void onProfile(CCObject* sender) {
        int accountId = static_cast<CCNode*>(sender)->getTag();
        if (accountId > 0) ProfilePage::create(accountId, false)->show();
    }

public:
    static KDLCreditsPopup* create() {
        auto ret = new KDLCreditsPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
