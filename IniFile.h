﻿#pragma once
#include <string>
#include <unordered_map>

#include <iostream>

class Value {
public:
	operator std::string() const { return value; }
	std::string getFileName() const;

	friend Value operator+(const Value& lhs, const std::string& rhs) {
		Value result = lhs;
		result.value += rhs;
		return result;
	}

	friend Value operator+(const std::string& lhs, const Value& rhs) {
		Value result = rhs;
		result.value = lhs + result.value;
		return result;
	}

	friend Value operator+(const Value& lhs, const Value& rhs) {
		Value result = lhs;
		result.value += rhs.value;
		return result;
	}

	Value& operator+=(const std::string& rhs) {
		value += rhs;
		return *this;
	}

	Value& operator+=(const Value& rhs) {
		value += rhs.value;
		return *this;
	}

	std::string value { };
	int line { -1 };
	char fileIndex { -1 };
	bool isInheritance { false };
};

class Section {
public:
	auto begin() { return section.begin(); }
	auto begin() const { return section.begin(); }
	auto end() { return section.end(); }
	auto end() const { return section.end(); }
	void insert(const Section& other) { return section.insert(other.begin(), other.end()); }
	size_t count(const std::string& key) const { return section.count(key); }
	Value at(const std::string& key) const { return section.at(key); }
	Value& at(const std::string& key) { return section.at(key); }
	Value& operator[](const std::string& key) { return section[key]; }

	bool isScanned { false };
	int inheritanceLevel { 0 };
	std::string name { };
	std::unordered_map<std::string, Value> section;
};
using Sections = std::unordered_map<std::string, Section>;

class IniFile {
public:
	static std::string argv0;
    IniFile(const std::string& filepath, bool isConfig);

    void load(const std::string& filepath);
    void readSection(std::string& line, int& lineNumber, std::string& currentSection);
    void readKeyValue(std::string& currentSection, std::string& line, int lineNumber);

	bool isConfig { false };
    Sections sections;
private:
    void processIncludes(const std::string& basePath);
	void processInheritance(std::string& line, size_t endPos, int& lineNumber, std::string& currentSection);
    std::string removeInlineComment(const std::string& str);
    std::string trim(const std::string& str);
};
