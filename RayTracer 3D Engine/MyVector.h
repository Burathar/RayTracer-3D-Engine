#ifndef _VECT_H
#define _VECT_H

#include "math.h"

class my_vector {
	double x_, y_, z_;

public:

	my_vector();

	my_vector(double, double, double);

	double get_vect_x() { return x_; }
	double get_vect_y() { return y_; }
	double get_vect_z() { return z_; }

	double magnitude() {
		return sqrt((x_ * x_) + (y_ * y_) + (z_ * z_));
	}

	my_vector normalize() {
		const double magnitude = sqrt((x_ * x_) + (y_ * y_) + (z_ * z_)); //deze lijn mag weg?
		return my_vector(x_ / magnitude, y_ / magnitude, z_ / magnitude);
	}

	my_vector negative() {
		return my_vector(-x_, -y_, -z_);
	}

	double dot_product(my_vector v) {
		return x_ * v.get_vect_x() + y_ * v.get_vect_y() + z_ * v.get_vect_z();
	}

	my_vector cross_product(my_vector v) {
		return my_vector(y_ * v.get_vect_z() - z_ * v.get_vect_y(),
			z_ * v.get_vect_x() - x_ * v.get_vect_z(),
			x_ * v.get_vect_y() - y_ * v.get_vect_x());
	}

	my_vector vect_add(my_vector v) {
		return my_vector(x_ + v.get_vect_x(), y_ + v.get_vect_y(), z_ + v.get_vect_z());
	}

	my_vector vect_mult(double scalar) {
		return my_vector(x_ * scalar, y_ * scalar, z_ * scalar);
	}
};

inline my_vector::my_vector() {
	x_ = 0;
	y_ = 0;
	z_ = 0;
}

inline my_vector::my_vector(double i, double j, double k) {
	x_ = i;
	y_ = j;
	z_ = k;
}

#endif
