#include "scriptPopup.hpp"
#include <filesystem>
#include <matjson.hpp>
#include "luaAPI.hpp"
#include "createPopup.hpp"

using namespace geode::prelude;
using namespace filesystem;
using namespace std;

ScriptPopup* ScriptPopup::instance = nullptr;

void ScriptPopup::close() {
    this->onClose(nullptr);
}

void ScriptPopup::runCode(CCObject* sender) {
    auto* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto* menu = static_cast<CCMenu*>(button->getParent());
    auto* nineSlice = static_cast<NineSlice*>(menu->getParent());
    
    auto* obj = static_cast<CCString*>(nineSlice->getUserObject("filename"_spr));

    auto path = Mod::get()->getSaveDir() / obj->getCString();
    ifstream file(path);
    std::string content(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    auto parseResult = matjson::parse(content);
    if (!parseResult.isOk()) {
        log::error("Invalid json file: {}", path.filename());
        return;
    }

    auto json = parseResult.unwrap();

    auto script = json["code"].asString().unwrapOr("");
    runScript(script);
}
NineSlice* ScriptPopup::createScriptUI(std::string id, char const* name, char const* description) {
    auto nineSlice = NineSlice::create(
        "GJ_square05.png",
        {0, 0, 80, 80},
        {12, 12, 12, 12}
    );
    nineSlice->setContentSize({200, 75});
    nineSlice->setUserObject("filename"_spr, CCString::create(id));

    auto nameLabel = CCLabelBMFont::create(name, "bigFont.fnt");
    nameLabel->setWidth(150);
    nameLabel->setScale(0.5f);
    nameLabel->setAnchorPoint({0, 1});
    nameLabel->setPosition({5, 72});

    auto descriptionLabel = CCLabelBMFont::create(description, "bigFont.fnt");
    descriptionLabel->setWidth(150);
    descriptionLabel->setScale(0.275f);
    descriptionLabel->setAnchorPoint({0, 0});
    descriptionLabel->setPosition({5, 5});

    nineSlice->addChild(nameLabel);
    nineSlice->addChild(descriptionLabel);

    auto buttons = CCMenu::create();
    buttons->setLayout(ColumnLayout::create());
    buttons->setPosition({195, 5});
    buttons->setContentSize({0, 65});
    buttons->setAnchorPoint({1, 0});

    auto runButton = CCMenuItemSpriteExtra::create(
        CCSprite::create("runSprite.png"_spr),
        nineSlice,
        menu_selector(ScriptPopup::runCode)
    );

    buttons->addChild(runButton);
    buttons->updateLayout();

    nineSlice->addChild(buttons);

    return nineSlice;
}

void ScriptPopup::createScript(CCObject* sender) {
    CreatePopup::create()->show();
}

void ScriptPopup::installScript(CCObject* sender) {
    file::FilePickOptions::Filter filter = {
        .description = "JSON Files",
        .files = {"*.json"}
    };
    file::FilePickOptions options =  {
        std::nullopt,
        {filter}
    };

    async::spawn(
        file::pick(file::PickMode::OpenFile, options),
        [this](Result<optional<filesystem::path>> result) {
            if (!result.isOk()) {
                return;
            }
            auto unwrapped = result.unwrap();
            if (!unwrapped.has_value()) {
                return;
            }

            std::filesystem::copy_file(unwrapped.value(), Mod::get()->getSaveDir() / unwrapped.value().filename(),
                              std::filesystem::copy_options::overwrite_existing);

            this->onClose(nullptr);
            ScriptPopup::create()->show();
        }
    );
}

bool ScriptPopup::init() {
    if (!Popup::init(425, 250)) {
        return false;
    }

    this->setTitle("Scripts");

    auto scrollLayer = ScrollLayer::create({425, 35});
    scrollLayer->setPosition({0, 20});
    scrollLayer->setContentSize({425, 180});

    auto layout = AxisLayout::create(Axis::Row);
    layout->setGrowCrossAxis(true);
    layout->setCrossAxisOverflow(true);

    scrollLayer->m_contentLayer->setLayout(layout);

    auto saveDir = Mod::get()->getSaveDir();

    for (auto& entry : directory_iterator(saveDir))
    {
        if (entry.path().extension() != ".json") {
            continue;
        }
        if (entry.path().filename() == "settings.json" || entry.path().filename() == "saved.json") {
            continue;
        }

        ifstream file(entry.path());
        std::string content(
            (istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>()
        );

        auto parseResult = matjson::parse(content);
        if (!parseResult.isOk()) {
            log::error("Invalid json file: {}", entry.path().filename());
            continue;
        }

        auto json = parseResult.unwrap();

        NineSlice *script = createScriptUI(entry.path().filename().string(),
                                           json["name"].asString().unwrapOr("Unnamed").c_str(),
                                           json["description"].asString().unwrapOr("No description").c_str());


        scrollLayer->m_contentLayer->addChild(script);
    }

    scrollLayer->m_contentLayer->updateLayout();

    scrollLayer->moveToTop();
    this->getChildByIndex(0)->addChild(scrollLayer);

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});

    auto createButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Create", 0.4f),
        this,
        menu_selector(ScriptPopup::createScript)
    );
    createButton->setID("create-script-button"_spr);
    createButton->setAnchorPoint({1, 1});
    createButton->setPosition({410, 240});

    auto installButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Install", 0.4f),
        this,
        menu_selector(ScriptPopup::installScript)
    );
    installButton->setID("create-install-button"_spr);
    installButton->setAnchorPoint({1, 1});
    installButton->setPosition({335, 240});

    menu->addChild(createButton);
    menu->addChild(installButton);
    this->m_mainLayer->addChild(menu);

    return true;
}

ScriptPopup* ScriptPopup::create() {
    auto ret = new ScriptPopup;
    if (ret->init()) {
        instance = ret;
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}