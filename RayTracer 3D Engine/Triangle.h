#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Object.h"
#include "MyVector.h"
#include "Color.h"

class triangle : public object {
	my_vector a_, b_, c_;
	my_vector normal_;
	double distance_;
	color color_;

public:

	triangle();

	triangle(my_vector, my_vector, my_vector, color);

	my_vector get_normal() {
		my_vector ca(
			c_.get_vect_x() - a_.get_vect_x(),
			c_.get_vect_y() - a_.get_vect_y(),
			c_.get_vect_z() - a_.get_vect_z());
		const my_vector ba(
			b_.get_vect_x() - a_.get_vect_x(),
			b_.get_vect_y() - a_.get_vect_y(),
			b_.get_vect_z() - a_.get_vect_z());
		normal_ = ca.cross_product(ba).normalize();
		return normal_;
	}

	color get_color() override { return color_; }

	my_vector get_normal_at(my_vector point) override
	{
		normal_ = get_normal();
		return normal_;
	}

	double find_intersection(ray ray) override
	{
		normal_ = get_normal();
		distance_ = get_triangle_distance();

		const double a = ray.get_ray_direction().dot_product(normal_);

		if (a == 0) {
			//ray is parallel to  the Triangle
			return -1;
		}

		//Is the intersection in the triangle?
		return intersection_in_triangle(ray, a);
	}

private:

	double get_triangle_distance() {
		normal_ = get_normal();
		distance_ = normal_.dot_product(a_);
		return distance_;
	}

	double intersection_in_triangle(ray ray, const double a) const
	{
		const double b = normal_.dot_product(ray.get_ray_origin().vect_add(normal_.vect_mult(distance_).negative()));

		const my_vector q = calculate_q(a, b, ray);

		if (test(q, c_, a_) >= 0 && test(q, b_, c_) >= 0 && test(q, a_, b_) >= 0)
		{
			//inside the triangle
			return -1 * b / a;
		}
		return -1;
	}

	my_vector calculate_q(double a, double b, ray ray) const
	{
		const double distance_to_plane = -1 * b / a;

		return my_vector(
			ray.get_ray_direction().vect_mult(distance_to_plane).get_vect_x() + ray.get_ray_origin().get_vect_x(),
			ray.get_ray_direction().vect_mult(distance_to_plane).get_vect_y() + ray.get_ray_origin().get_vect_y(),
			ray.get_ray_direction().vect_mult(distance_to_plane).get_vect_z() + ray.get_ray_origin().get_vect_z()
		);
	}

	double test(my_vector q, my_vector a, my_vector b) const
	{
		const my_vector ab(
			a.get_vect_x() - b.get_vect_x(),
			a.get_vect_y() - b.get_vect_y(),
			a.get_vect_z() - b.get_vect_z()
		);
		const my_vector qb(
			q.get_vect_x() - b.get_vect_x(),
			q.get_vect_y() - b.get_vect_y(),
			q.get_vect_z() - b.get_vect_z()
		);
		return ab.cross_product(qb).dot_product(normal_);
	}
};

inline triangle::triangle() {
	a_ = my_vector(1, 0, 0);
	b_ = my_vector(0, 1, 0);
	c_ = my_vector(0, 0, 1);
	normal_ = my_vector(1, 0, 0);
	distance_ = 0.0;
	color_ = color(.5, .5, .5, 0);
}

inline triangle::triangle(my_vector point_a, my_vector point_b, my_vector point_c, color color_value) {
	a_ = point_a;
	b_ = point_b;
	c_ = point_c;
	color_ = color_value;
	distance_ = 0.0;
}

#endif
