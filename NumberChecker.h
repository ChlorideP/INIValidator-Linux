﻿#pragma once
#include "IniFile.h"
#include <string>
class NumberChecker {
public:
	NumberChecker() = default;
	NumberChecker(const Section& config);

	// 检查数字是否在指定范围内
	std::string validate(const std::string& value) const;

private:
	std::string type; // 类型，如 uint, short, ushort
	int minRange = 0; // 最小值
	int maxRange = 0; // 最大值

	bool checkRange(int value) const;
};
