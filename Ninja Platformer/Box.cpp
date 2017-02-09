#include "Box.h"

Box::Box()
{
    // Empty
}


Box::~Box()
{
    // Empty
}


void Box::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, Bengine::ColorRGBA8 color)
{
    m_color = color;
    m_dimensions = dimensions;

    // Make the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    m_body = world->CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(dimensions.x / 2, dimensions.y / 2);

    // Make the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    m_fixture = m_body->CreateFixture(&fixtureDef);
}
