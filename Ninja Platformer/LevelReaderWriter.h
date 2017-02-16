#pragma once

#include <string>

#include "Player.h"
#include "Light.h"
#include "Box.h"

class LevelReaderWriter
{
public:
    static bool saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
    static bool loadFromText(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
private:
    static bool saveAsTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
    static bool loadAsTextV0(std::ifstream& file, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
};