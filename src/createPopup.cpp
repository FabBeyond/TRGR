#include <Geode/Geode.hpp>
#include "createPopup.hpp"
#include "scriptPopup.hpp"
#include <cctype>
#include <filesystem>
#include <matjson.hpp>
#include <fstream>


using namespace geode::prelude;
using namespace std;

void CreatePopup::createScript(CCObject* sender) {
    if (nameInput->getString() == "") {
        return;
    }

    matjson::Value script;
    script["name"] = nameInput->getString();
    script["description"] = descriptionInput->getString();

    std::string code;

    std::ifstream file(path);
    std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    script["code"] = content.c_str();

    std::string filename = nameInput->getString();
    std::replace(filename.begin(), filename.end(), ' ', '_');
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    filename += ".json";

    std::ofstream jsonFile(Mod::get()->getSaveDir() / filename);
    jsonFile << script.dump();
    jsonFile.close();

    this->onClose(nullptr);
    ScriptPopup::instance->close();
    ScriptPopup::create()->show();
}


void CreatePopup::pickFile(CCObject* sender) {
    file::FilePickOptions::Filter filter = {
        .description = "Lua Files",
        .files = {"*.lua"}
    };
    file::FilePickOptions options =  {
        std::nullopt,
        {filter}
    };

    async::spawn(
        file::pick(file::PickMode::OpenFile, options),
        [this](Result<optional<filesystem::path>> result) {
            onFilePicked(std::move(result));
        }
    );
}

void CreatePopup::onFilePicked(Result<std::optional<std::filesystem::path>> result) {
    if (!result.isOk()) {
        return;
    }
    auto unwrapped = result.unwrap();
    if (!unwrapped.has_value()) {
        return;
    }

    path = unwrapped.value();
}

bool CreatePopup::init() {
    if (!Popup::init(250, 175)) {
        return false;
    }

    auto createButtonMenu = CCMenu::create();
    createButtonMenu->setPosition({0, 0});

    auto createButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Create", 0.7f),
        this,
        menu_selector(CreatePopup::createScript)
    );
    createButton->setPosition({125, 0});

    createButtonMenu->addChild(createButton);
    this->m_mainLayer->addChild(createButtonMenu);

    nameInput = TextInput::create(150, "Name");
    nameInput->setPosition({125, 140});
    descriptionInput = TextInput::create(200, "Description (optional)");
    descriptionInput->setPosition({125, 95});

    auto filePickerMenu = CCMenu::create();
    filePickerMenu->setPosition({0, 0});

    auto filePickerButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Pick File", 0.7f),
        this,
        menu_selector(CreatePopup::pickFile)
    );
    filePickerButton->setPosition({125, 50});

    filePickerMenu->addChild(filePickerButton);

    this->m_mainLayer->addChild(nameInput);
    this->m_mainLayer->addChild(descriptionInput);
    this->m_mainLayer->addChild(filePickerMenu);

    return true;
}

CreatePopup* CreatePopup::create() {
    auto ret = new CreatePopup;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}