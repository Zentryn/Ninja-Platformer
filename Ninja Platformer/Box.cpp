#include "Box.h"

Box::Box()
{
    // Empty
}


Box::~Box()
{
    // Empty
}


void Box::init(b2World* world,
               const glm::vec2& position,
               const glm::vec2& dimensions,
               Bengine::GLTexture texture,
               Bengine::ColorRGBA8 color,
               bool isDynamic,
               float angle /*= 0.0f*/,
               bool fixedRotation /*= false*/,
               glm::vec4 uvRect /*= glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)*/)
{
    m_color = color;
    m_dimensions = dimensions;
    m_texture = texture;
    m_uvRect = uvRect;
    m_isDynamic = isDynamic;

    // Make the body
    b2BodyDef bodyDef;
    if (isDynamic) {
        bodyDef.type = b2_dynamicBody;
    }
    else {
        bodyDef.type = b2_staticBody;
    }

    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.fixedRotation = fixedRotation;
    bodyDef.angle = angle;
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

void Box::destroy(b2World* world)
{
    world->DestroyBody(m_body);
}

void Box::draw(Bengine::SpriteBatch& spriteBatch)
{
    glm::vec4 destRect(
        m_body->GetPosition().x - m_dimensions.x / 2.0f,
        m_body->GetPosition().y - m_dimensions.y / 2.0f,
        m_dimensions
    );

    spriteBatch.draw(
        destRect,
        m_uvRect,
        m_texture.id,
        0.0f,
        m_color,
        m_body->GetAngle()
    );
}
