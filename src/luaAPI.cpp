#include "luaAPI.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>

using namespace geode::prelude;
using namespace std;

sol::state lua;

ZLayer intToZLayer(int value) {
    switch (value) {
        case -5: return ZLayer::B5;
        case -4: return ZLayer::B4;
        case -3: return ZLayer::B3;
        case -2: return ZLayer::B2;
        case -1: return ZLayer::B1;
        case  0: return ZLayer::Default;
        case  1: return ZLayer::T1;
        case  2: return ZLayer::T2;
        case  3: return ZLayer::T3;
        case  4: return ZLayer::T4;
        default: return ZLayer::Default;
    }
}

void addFunctions(sol::table gd) {
    gd.set_function("getById", [](int id) {
        vector<GameObject*> objects;
        for (auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->getAllObjects())) {
            if (obj->m_objectID == id) {
                objects.push_back(obj);
            }
        }
        return objects;
    });
    gd.set_function("getAll", []() {
        return LevelEditorLayer::get()->getAllObjects();
    });
    gd.set_function("getSelected", []() {
        vector<GameObject*> objects;
        for (auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->getAllObjects())) {
            if (obj->m_isSelected) {
                objects.push_back(obj);
            }
        }
        return objects;
    });
    gd.set_function("getByGroup", [](int group) {
        vector<GameObject*> objects;
        for (auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->getAllObjects())) {
            for (int i = 0; i < obj->m_groupCount; i++) {
                if (obj->m_groups->at(i) == group) {
                    objects.push_back(obj);
                    break;
                }
            }
        }
        return objects;
    });
    gd.set_function("log", [](std::string message) {
        log::info("{}", message);
    });
}

void setupGameObjectMethods(sol::usertype<GameObject>& type) {
    type["move"] = [](GameObject* self, float x, float y) {
        self->setPosition(self->getPosition() + CCPoint{x, y});
    };
    type["moveTo"] = [](GameObject* self, float x, float y) {
        self->setPosition({x, y});
    };
    type["rotate"] = [](GameObject* self, float rotation) {
        self->addRotation(rotation);
    };
    type["scale"] = [](GameObject* self, float x, float y) {
        self->updateCustomScaleX(self->getScaleX() + x);
        self->updateCustomScaleY(self->getScaleY() + y);
    };
    type["delete"] = [](GameObject* self) {
        LevelEditorLayer::get()->removeObject(self, false);
        EditorUI::get()->deselectAll();
    };
}

void setupGameObjectProperties(sol::usertype<GameObject>& type) {
    type["id"] = sol::property([](GameObject* self) { return self->m_objectID; });
    type["x"] = sol::property(
        [](GameObject* self) { return self->getPositionX(); },
        [](GameObject* self, float x) { self->setPositionX(x); }
    );
    type["y"] = sol::property(
        [](GameObject* self) { return self->getPositionY(); },
        [](GameObject* self, float y) { self->setPositionY(y); }
    );
    type["scaleX"] = sol::property(
        [](GameObject* self) { return self->getScaleX(); },
        [](GameObject* self, float x) { self->updateCustomScaleX(x); }
    );
    type["scaleY"] = sol::property(
        [](GameObject* self) { return self->getScaleY(); },
        [](GameObject* self, float y) { self->updateCustomScaleY(y); }
    );
    type["rotation"] = sol::property(
        [](GameObject* self) { return self->getRotation(); },
        [](GameObject* self, float r) {
            if (self->canRotateFree()) {
                self->addRotation(-self->getRotation());
                self->addRotation(r);
            }
        }
    );
    type["zOrder"] = sol::property(
        [](GameObject* self) { return self->m_zOrder; },
        [](GameObject* self, int z) { self->m_zOrder = z; }
    );
    type["zLayer"] = sol::property(
        [](GameObject* self) { return self->m_zLayer; },
        [](GameObject* self, int l) { self->m_zLayer = intToZLayer(l); }
    );
    type["flipX"] = sol::property(
        [](GameObject* self) { return self->isFlipX(); },
        [](GameObject* self, bool f) { self->setFlipX(f); }
    );
    type["flipY"] = sol::property(
        [](GameObject* self) { return self->isFlipY(); },
        [](GameObject* self, bool f) { self->setFlipY(f); }
    );
}

void setupLuaAPI() {
    lua.open_libraries(sol::lib::base, sol::lib::table, sol::lib::math);

    sol::table gd = lua.create_named_table("level");
    addFunctions(gd);

    auto type = lua.new_usertype<GameObject>("GameObject");
    setupGameObjectMethods(type);
    setupGameObjectProperties(type);
}

void runScript(const std::string& code) {
    auto result = lua.safe_script(code, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;

        log::info("Script error: {}", err.what());
        FLAlertLayer::create("Script Error", err.what(), "OK")->show();
    }
}