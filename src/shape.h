#ifndef _shape_h
#define _shape_h

#include "main.h"

class Shape {
private:
	int lane;
	int speed;
	int height;

	int type;

public:
    Shape();                /* creates player shape */
	Shape(int speed);       /* creates falling shape */
    
	~Shape() {}

	void move();
	void render(RenderData *data);

	bool isFalling();

    void setType(int newType);
    int getType() const;

    void setLane(int lane);
    int getLane() const;

    bool operator==(const Shape& other) const;
};


#endif /* _shape_h */