#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>

using namespace geode::prelude;

class $modify(ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* a2) {
        ProfilePage::loadPageFromUserInfo(a2);
        auto layer = m_mainLayer;

        CCMenu* username_menu = static_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        if (!username_menu) return;

        auto sprite = CCSprite::create((Mod::get()->getResourcesDir() / "owner.png").string().c_str());
        auto yourBadge = CCMenuItemExt::createSpriteExtra(sprite, [](CCMenuItemSpriteExtra*) {
            FLAlertLayer::create(nullptr, "K.D.L Owner", "Badge description here.", "OK", nullptr, 300.f)->show();
        });
        yourBadge->setID("kdl-owner-badge"_spr);
        username_menu->addChild(yourBadge);
        username_menu->updateLayout();
    }
};

class $modify(CommentCell) {
    void loadFromComment(GJComment* p0) {
        CommentCell::loadFromComment(p0);
        auto layer = m_mainLayer;

        CCMenu* username_menu = static_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        if (!username_menu) return;

        auto sprite = CCSprite::create((Mod::get()->getResourcesDir() / "owner.png").string().c_str());
        auto yourBadge = CCMenuItemExt::createSpriteExtra(sprite, [](CCMenuItemSpriteExtra*) {
            FLAlertLayer::create(nullptr, "K.D.L Owner", "Badge description here.", "OK", nullptr, 300.f)->show();
        });
        yourBadge->setID("kdl-owner-badge"_spr);
        username_menu->addChild(yourBadge);
        username_menu->updateLayout();
    }
};