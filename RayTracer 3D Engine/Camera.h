#ifndef _CAMERA_H
#define _CAMERA_H

#include "MyVector.h"

class camera {
	my_vector campos_, camdir_, camright_, camdown_;

public:

	camera();

	camera(my_vector, my_vector, my_vector, my_vector);

	my_vector get_camera_position() { return campos_; }
	my_vector get_camera_direction() { return camdir_; }
	my_vector get_camera_right() { return camright_; }
	my_vector get_camera_down() { return camdown_; }
};

inline camera::camera() {
	campos_ = my_vector(0, 0, 0);
	camdown_ = my_vector(0, 0, 1);
	camright_ = my_vector(0, 0, 0);
	camdown_ = my_vector(0, 0, 0);
}

inline camera::camera(my_vector pos, my_vector dir, my_vector right, my_vector down) {
	campos_ = pos;
	camdir_ = dir;
	camright_ = right;
	camdown_ = down;
}

#endif
