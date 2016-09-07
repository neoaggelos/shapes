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
	height = 20;

    type = random(0, range-1);
}

void
Shape::move(double var)
{
	height += speed * var;
}

void
Shape::render()
{
	RenderData *data = gSuper->getRenderData();
	SDL_Rect srcRect, dstRect;

	srcRect.x = 80 * type;
	srcRect.y = 0;
	srcRect.w = 80;
	srcRect.h = 80;

	dstRect.x = 60 + 20 + (lane - 1) * 120;
	dstRect.y = static_cast<int>(height);

	dstRect.w = 80;
	dstRect.h = 80;

	SDL_RenderCopy(data->getRenderer(), data->getTexture(), &srcRect, &dstRect);
}

bool
Shape::isFalling()
{
	return height <= 460.0;
}

void
Shape::setType(int newType)
{
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

double
Shape::getHeight()
{
	return height;
}

