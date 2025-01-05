#pragma once
#include <map>
#include <string>
#include "json.hpp"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <type_traits>

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

    nlohmann::json constructJson() const {
        nlohmann::json json;
        for (const auto& [key, value] : m_values) {
            if (std::holds_alternative<bool>(value)) {
                json[key] = std::get<bool>(value);
            } else if (std::holds_alternative<float>(value)) {
                json[key] = std::get<float>(value);
            } else if (std::holds_alternative<int>(value)) {
                json[key] = std::get<int>(value);
            } else if (std::holds_alternative<std::filesystem::path>(value)) {
                json[key] = std::get<std::filesystem::path>(value).string();
            }
        }
        return json;
    }

    void loadFromJson(const nlohmann::json& json) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();

            if (value.is_boolean()) {
                m_values[key] = value.get<bool>();
            } else if (value.is_number_float()) {
                m_values[key] = value.get<float>();
            } else if (value.is_number_integer()) {
                m_values[key] = value.get<int>();
            } else if (value.is_string()) {
                m_values[key] = std::filesystem::path(value.get<std::string>());
            }
        }
    }

    void load(const std::filesystem::path& filepath) {
        std::ifstream inFile(filepath);
        if (inFile.is_open()) {
            std::string file_contents((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

            nlohmann::json json = nlohmann::json::parse(file_contents, nullptr, false);
            if (json.is_discarded()) {
                return;
            }

            loadFromJson(json);
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
    void set(const std::string& key, T value) {
        m_values[key] = value;
    }

    template <typename T, typename R = optional_ref<T>> requires is_valid_type<T>
    R get(const std::string& key, const T& defaultValue) const {
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
    std::unordered_map<std::string, Value> m_values {};
};
