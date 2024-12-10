#pragma once
#include <map>
#include <string>
#include <matjson.hpp>
#include <fstream>
#include <filesystem>

static const auto folderPath = geode::Mod::get()->getSaveDir();
static const auto folderMacroPath = folderPath / "Macros";
static const auto fileDataPath = folderPath / "config2.json";
static const auto labelsDataPath = folderPath / "labels.txt";
static const auto colorsDataPath = folderPath / "colors.txt";

class Config {
public:
    static Config& get() {
        static Config instance;
        return instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void load(const std::filesystem::path& filepath) {
        std::ifstream inFile(filepath);
        if (inFile.is_open()) {
            std::string file_contents((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

            auto result = matjson::parse(file_contents);
            if (!result.isErr()) {
                jsonData = result.unwrap();
            }

            inFile.close();
        }
    }

    void save(const std::filesystem::path& filepath) const {
        std::ofstream outFile(filepath);
        if (outFile.is_open()) {
            outFile << jsonData.dump(4);
            outFile.close();
        }
    }

    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        if (jsonData.contains(key)) {
            return jsonData[key].as<T>().unwrap();
        }
        return defaultValue;
    }

    template<typename T>
    void set(const std::string& key, const T& value) {
        jsonData[key] = value; 
    }

private:
    Config() = default;
    matjson::Value jsonData;
};