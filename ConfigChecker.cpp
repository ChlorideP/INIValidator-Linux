#include "ConfigChecker.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <set>

ConfigChecker::ConfigChecker(const IniFile& configFile) {
    loadConfig(configFile);
}

// ���������ļ�
void ConfigChecker::loadConfig(const IniFile& configFile) {
    // ���� Limits
    if (configFile.sections.count("Limits")) {
        for (const auto& [limitKey, _] : configFile.sections.at("Limits")) {
            if (configFile.sections.count(limitKey)) {
                LimitRule rule;
                rule.loadFromConfig(configFile.sections.at(limitKey));
                limits[limitKey] = rule;
            }
        }
    }

    // ���� Sections
    if (configFile.sections.count("Sections")) {
        for (const auto& [sectionName, _] : configFile.sections.at("Sections")) {
            if (configFile.sections.count(sectionName)) {
                sections[sectionName] = configFile.sections.at(sectionName);
                handleInheritance(sections[sectionName]);
            }
        }
    }
}

// ����̳й�ϵ (֧�� [A]:[B] �﷨)
void ConfigChecker::handleInheritance(std::unordered_map<std::string, std::string>& section) {
    for (const auto& [key, value] : section) {
        if (key.back() == ':') {
            std::string parentSection = key.substr(0, key.size() - 1);
            if (sections.count(parentSection)) {
                section.insert(sections[parentSection].begin(), sections[parentSection].end());
            }
        }
    }
}

// ����Ŀ�� INI ��ע���
void ConfigChecker::parseRegistry(const std::string& registrySection, const IniFile& targetIni, std::unordered_map<int, std::string>& registry) {
    std::set<int> usedKeys;
    if (targetIni.sections.count(registrySection)) {
        for (const auto& [keyStr, value] : targetIni.sections.at(registrySection)) {
            int key;
            if (keyStr == "+") {
                key = 1;
                while (usedKeys.count(key)) ++key; // �ҵ�δʹ�õ���С����
            }
            else {
                try {
                    key = std::stoi(keyStr);
                }
                catch (...) {
                    std::cerr << "Warning: Non-integer key \"" << keyStr << "\" in " << registrySection << " ignored." << std::endl;
                    continue;
                }
                if (registry.count(key)) {
                    std::cerr << "Error: Duplicate key \"" << key << "\" in " << registrySection << ", value \"" << registry[key] << "\" overwritten by \"" << value << "\"." << std::endl;
                }
            }
            registry[key] = value;
            usedKeys.insert(key);
        }
    }
}

// ��֤ÿ��ע��ڵ�����
void ConfigChecker::checkFile(const IniFile& targetIni) {
    for (const auto& [registryName, _] : sections) {
        std::unordered_map<int, std::string> registry;
        parseRegistry(registryName, targetIni, registry);

        for (const auto& [_, sectionName] : registry) {
            if (targetIni.sections.count(sectionName)) {
                validateSection(sectionName, targetIni.sections.at(sectionName));
            }
            else {
                std::cerr << "Error: Section \"" << sectionName << "\" referenced in " << registryName << " not found." << std::endl;
            }
        }
    }
}

// ��֤ĳ����
void ConfigChecker::validateSection(const std::string& sectionName, const std::unordered_map<std::string, std::string>& keys) {
    if (!sections.count(sectionName)) {
        std::cerr << "Warning: No configuration found for section \"" << sectionName << "\"." << std::endl;
        return;
    }

    const auto& rules = sections[sectionName];
    for (const auto& [key, value] : keys) {
        std::string type = rules.count(key) ? rules.at(key) : "";
        if (!validate(key, value, type)) {
            std::cerr << "Error: Invalid value for " << sectionName << "." << key << ": " << value << std::endl;
        }
    }
}

// ��֤��ֵ��
bool ConfigChecker::validate(const std::string& key, const std::string& value, const std::string& type) {
    if (type == "int") return isNumber(value);
    if (type == "float" || type == "double") return isFloat(value);
    if (type == "string") return value.size() < 256;

    // ����Ƿ�Ϊ��������
    if (limits.count(type)) {
        return limits.at(type).validate(value);
    }
    return true;
}

bool ConfigChecker::isNumber(const std::string& str) { return std::regex_match(str, std::regex("^-?\\d+$")); }
bool ConfigChecker::isFloat(const std::string& str) { return std::regex_match(str, std::regex("^-?\\d+(\\.\\d+)?$")); }