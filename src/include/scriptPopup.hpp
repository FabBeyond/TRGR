#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ScriptPopup : public Popup {
protected:
    void createScript(CCObject* sender);
    void installScript(CCObject* sender);
    void favourite(CCObject*);
    NineSlice* createScriptUI(std::string id, char const* name, char const* description);
    void runCode(CCObject*);
    bool init();
public:
    void close();
    static ScriptPopup* instance;
    static ScriptPopup* create();
};