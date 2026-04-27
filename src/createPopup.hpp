#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace std;

class CreatePopup : public Popup {
protected:
    TextInput* nameInput;
    TextInput* descriptionInput;
    std::filesystem::path path;

    void onFilePicked(Result<std::optional<std::filesystem::path>> result);
    void createScript(CCObject* sender);
    void pickFile(CCObject* sender);
    bool init();
public:
    static CreatePopup* create();
};