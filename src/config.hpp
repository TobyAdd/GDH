#pragma once
#include <map>
#include <string>
#include <matjson.hpp>
#include <matjson/std.hpp>
#include <fstream>
#include <filesystem>

static const auto folderPath = geode::Mod::get()->getSaveDir();
static const auto folderMacroPath = folderPath / "Macros";
static const auto fileDataPath = folderPath / "config2.json";
static const auto labelsDataPath = folderPath / "labels.txt";
static const auto colorsDataPath = folderPath / "colors.txt";

template <typename T>
struct requires_ref {
    static constexpr auto size = sizeof(T);
    static constexpr auto ptr_size = sizeof(T*);
    static constexpr bool value = size > ptr_size;
};

template <typename T>
constexpr bool requires_ref_v = requires_ref<T>::value;

template <typename T>
using optional_ref = std::conditional_t<requires_ref_v<T>, T const&, T>;

template <typename T, typename U = std::remove_cvref_t<std::decay_t<T>>>
constexpr bool is_valid_type = std::is_same_v<U, bool>
                            || std::is_same_v<U, float>
                            || std::is_same_v<U, int>
                            || std::is_same_v<U, std::filesystem::path>;

class Config {
public:
    static Config& get() {
        static Config instance;
        return instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    matjson::Value constructJson() const {
        matjson::Value json;
        for (const auto& [key, value] : m_values) {
    #define ADD_VALUE(type) if (auto* ptr = std::get_if<type>(&value)) json[key] = *ptr;
            ADD_VALUE(bool)
            else ADD_VALUE(float)
            else ADD_VALUE(std::filesystem::path)
    #undef ADD_VALUE
        }
        return json;
    }

    void loadFromJson(matjson::Value json) {
        // for (const auto& [key, value] : json) {
        //     if (value.isNumber()) {
        //         if (value.isFloat()) {
        //             m_values[key] = value.as<float>().unwrapOrDefault();
        //         } else {
        //             m_values[key] = value.as<int>().unwrapOrDefault();
        //         }
        //     } else if (value.isBool()) {
        //         m_values[key] = value.as<bool>().unwrapOrDefault();
        //     } else {
        //         auto path = value.as<std::filesystem::path>().unwrapOrDefault();
        //         m_values[key] = path;
        //     }
        // }
    }

    void load(const std::filesystem::path& filepath) {
        std::ifstream inFile(filepath);
        if (inFile.is_open()) {
            std::string file_contents((std::istreambuf_iterator(inFile)), std::istreambuf_iterator<char>());

            if (auto result = matjson::parse(file_contents)) {
                loadFromJson(result.unwrap());
            }

            inFile.close();
        }
    }

    void save(const std::filesystem::path& filepath) const {
        std::ofstream outFile(filepath);
        if (outFile.is_open()) {
            outFile << constructJson().dump(4);
            outFile.close();
        }
    }

    template <typename T> requires is_valid_type<T>
    void set(std::string const& key, T value) {
        m_values[key] = value;
    }

    template <typename T, typename R = optional_ref<T>> requires is_valid_type<T>
    R get(std::string const& key, T const& defaultValue) const {
        if (auto it = m_values.find(key); it != m_values.end()) {
            if (auto* ptr = std::get_if<T>(&it->second)) {
                return *ptr;
            }
        }
        return defaultValue;
    }

private:
    using Value = std::variant<bool, int, float, std::filesystem::path>;

    Config() = default;
    // matjson::Value jsonData;
    std::unordered_map<std::string, Value> m_values {};
};