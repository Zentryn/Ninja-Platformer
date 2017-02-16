#include "LevelReaderWriter.h"

#include <Bengine/ResourceManager.h>
#include <fstream>

// When a new version is made, add it here
const unsigned int TEXT_VERSION_0 = 100;

// Change this according to what version is being used
const unsigned int TEXT_VERSION = TEXT_VERSION_0;

bool LevelReaderWriter::saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights)
{
    if (TEXT_VERSION == TEXT_VERSION_0) {
        return saveAsTextV0(filePath, player, boxes, lights);
    }
    else {
        puts("Unknown text version!");
        return false;
    }
}

bool LevelReaderWriter::loadFromText(const std::string& filePath, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights)
{
    std::ifstream file(filePath);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    // Get version
    unsigned int version;
    file >> version;

    switch (version) {
    case TEXT_VERSION_0:
        return loadAsTextV0(file, world, player, boxes, lights);
        break;
    default:
        puts("Unknown version number in level file. File may be corrupted...");
        return false;
    }
}

bool LevelReaderWriter::saveAsTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights)
{
    std::ofstream file(filePath);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    // Text version
    file << TEXT_VERSION << '\n';

    // Player information
    file << player.getPosition().x << ' ' << player.getPosition().y << ' '
         << player.getDrawDims().x << ' ' << player.getDrawDims().y << ' '
         << player.getCollisionDims().x << ' ' << player.getCollisionDims().y << ' '
         << player.getColor().r << ' ' << player.getColor().g << ' '
         << player.getColor().b << ' ' << player.getColor().a << '\n';

    // Number of boxes
    file << boxes.size() << '\n';

    // Boxes' information
    for (auto& b : boxes) {
        file << b.getPosition().x << ' ' << b.getPosition().y << ' '
             << b.getDimensions().x << ' ' << b.getDimensions().y << ' '
             << b.getColor().r << ' ' << b.getColor().g << ' '
             << b.getColor().b << ' ' << b.getColor().a << ' '
             << b.getUvRect().x << ' ' << b.getUvRect().y << ' '
             << b.getUvRect().z << ' ' << b.getUvRect().w << ' '
             << b.getAngle() << ' ' << b.getTexture().filePath << ' '
             << b.getIsDynamic() << ' ' << b.getFixedRotation() << '\n';
    }

    // Number of lights
    file << lights.size() << '\n';

    // Lights' information
    for (auto& l : lights) {
        file << l.position.x << ' ' << l.position.y << ' '
             << l.size << ' '
             << l.color.r << ' ' << l.color.g << ' '
             << l.color.b << ' ' << l.color.a << '\n';
    }

    return true;
}

bool LevelReaderWriter::loadAsTextV0(std::ifstream& file, b2World* world, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights)
{
    { // Read player
        glm::vec2 pos;
        glm::vec2 ddims;
        glm::vec2 cdims;
        Bengine::ColorRGBA8 color;
        file >> pos.x >> pos.y >> ddims.x >> ddims.y >> cdims.x >> cdims.y >> color.r >> color.g >> color.b >> color.a;
        player.init(world, pos, ddims, cdims, color);
    }

    { // Read boxes
        glm::vec2 pos;
        glm::vec2 dims;
        glm::vec4 uvRect;
        Bengine::ColorRGBA8 color;
        std::string texturePath;
        Bengine::GLTexture texture;
        bool fixedRotation;
        float angle;
        bool dynamic;
        size_t num_boxes;

        file >> num_boxes;
        for (size_t i = 0; i < num_boxes; i++) {
            file >> pos.x >> pos.y >> dims.x >> dims.y
                 >> color.r >> color.g >> color.b >> color.a
                 >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
                 >> angle >> texturePath >> dynamic >> fixedRotation;

            texture = Bengine::ResourceManager::getTexture(texturePath);

            boxes.emplace_back();
            boxes.back().init(world, pos, dims, texture, color, dynamic, angle, fixedRotation, uvRect);
        }
    }

    { // Read lights
        glm::vec2 pos;
        float size;
        Bengine::ColorRGBA8 color;
        size_t num_lights;

        file >> num_lights;
        for (size_t i = 0; i < num_lights; i++) {
            file >> pos.x >> pos.y >> size >> color.r >> color.g >> color.b >> color.a;

            lights.emplace_back();
            lights.back().color = color;
            lights.back().size = size;
            lights.back().position = pos;
        }
    }

    return true;
}
