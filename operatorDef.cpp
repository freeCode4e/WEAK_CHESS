#include "operatorDef.h"


sf::Vector2f operator*(sf::Vector2f vec1, sf::Vector2f vec2)
{
	return { vec1.x * vec2.x, vec1.y * vec2.y };
}

sf::Vector2f operator*(sf::Vector2f vec1, float f)
{
	return { vec1.x * f, vec1.y * f };
}

sf::Vector2f operator*(float f, sf::Vector2f vec1)
{
	return vec1 * f;
}
