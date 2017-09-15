#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "MyVector.h"
#include "Color.h"

class object {
public:
	virtual ~object() = default;

	object();

	virtual color get_color() { return color(0.0, 0.0, 0.0, 0); }

	virtual my_vector get_normal_at(my_vector intersection_position) {
		return my_vector(0, 0, 0);
	}

	virtual double find_intersection(ray ray) {
		return 0;
	}
};

inline object::object() {}

#endif
