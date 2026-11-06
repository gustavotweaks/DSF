#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <algorithm>

using namespace geode::prelude;

namespace {
    class TimingDisc final : public CCNode {
    public:
        static TimingDisc* create() {
            auto ret = new TimingDisc();
            if (ret && ret->init()) {
                ret->autorelease();
                return ret;
            }
            CC_SAFE_DELETE(ret);
            return nullptr;
        }

        bool init() {
            if (!CCNode::init()) return false;
            m_draw = CCDrawNode::create();
            if (!m_draw) return false;
            addChild(m_draw);
            setContentSize({28.f, 54.f});
            setAnchorPoint({0.5f, 0.5f});
            setVisible(false);
            return true;
        }

        void show(float progress, bool hold) {
            m_progress = std::clamp(progress, 0.f, 1.f);
            m_hold = hold;
            setVisible(true);
            redraw();
        }

        void hide() {
            setVisible(false);
        }

    private:
        void redraw() {
            if (!m_draw) return;
            m_draw->clear();

            // Exactly one filled disc; no rings, outlines or extra circles.
            constexpr float radius = 7.f;
            constexpr float top = 46.f;
            constexpr float hit = 8.f;
            const float y = top + (hit - top) * m_progress;
            const ccColor4F white{1.f, 1.f, 1.f, 1.f};
            m_draw->drawDot({14.f, y}, radius, white);
        }

        CCDrawNode* m_draw = nullptr;
        float m_progress = 0.f;
        bool m_hold = false;
    };
}

class $modify(GustavoRhythmHUDPlayLayer, PlayLayer) {
    struct Fields {
        TimingDisc* indicator = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto indicator = TimingDisc::create();
        if (indicator) {
            indicator->setPosition({25.f, 43.f});
            this->addChild(indicator, 10000);
            m_fields->indicator = indicator;
        }
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Safe prototype behavior: never display invented/incorrect timing.
        // Replace this with the physics + object look-ahead predictor.
        if (m_fields->indicator) {
            m_fields->indicator->hide();
        }
    }
};
