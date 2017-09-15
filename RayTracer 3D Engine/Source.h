#ifndef _SOURCE_H
#define _SOURCE_H

class source {
public:
	virtual ~source() = default;

	source();

	virtual my_vector get_light_position() { return my_vector(0, 0, 0); }
	virtual color get_light_color() { return color(1, 1, 1, 0); }
};

inline source::source() {}

#endif
