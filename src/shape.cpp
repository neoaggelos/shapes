#include "shape.h"

Shape::Shape()
{
    /* No arguments constructor creates a player */
    lane = 2;
    height = 550.0;
    speed = 0;

    type = 0;
}

Shape::Shape(double spd, int range)
{
	lane = random(1, 3);
    speed = spd;
	height = 40;

    type = random(0, range-1);
}

void
Shape::move()
{
	height += speed;
}

void
Shape::render(RenderData *data)
{
	SDL_Rect pos;

	pos.x = 60 + 20 + (lane - 1) * 120;
	pos.y = static_cast<int>(height);

	pos.w = 80;
	pos.h = 80;

	SDL_RenderCopy(data->getRenderer(), data->getTexture(type), NULL, &pos);
}

bool
Shape::isFalling()
{
	return height <= 460.0;
}

void
Shape::setType(int newType)
{
    /* avoid funny stuff */
    if (type >= 0 && type < NUM_SHAPES)
        type = newType;
}

int
Shape::getType() const
{
    return type;
}

void
Shape::setLane(int newLane)
{
    lane = newLane;
    
    /* avoid funny stuff */
    if (lane >= 3) lane = 3;
    if (lane <= 1) lane = 1;
}

int
Shape::getLane() const
{
    return lane;
}

bool
Shape::operator==(const Shape & other) const
{
    return (lane == other.getLane()) && (type == other.getType());
}