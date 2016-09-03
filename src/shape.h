#ifndef _shape_h
#define _shape_h

#include "main.h"

class Shape {
private:
	int lane;
	double speed;
	double height;

	int type;

public:
    Shape();                /* creates player shape */
	Shape(double speed, int range);       /* creates falling shape */
    
	~Shape() {}

	void move(double var=1);
	void render(RenderData *data);

	bool isFalling();
	double getHeight();

    void setType(int newType);
    int getType() const;

    void setLane(int lane);
    int getLane() const;

    bool operator==(const Shape& other) const;
};


#endif /* _shape_h */