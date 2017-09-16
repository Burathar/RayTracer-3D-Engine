// RayTracer 3D Engine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <time.h>

#include "filestream.h"

#include "MyVector.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"

using namespace std;

int winning_object_index(vector<double> object_intersections) {
	// return the index of the winning intersection
	int index_of_minimum_value = 0;

	//prevent unnecessary caculations
	if (object_intersections.size() == 0) {
		return -1;
	}
	if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			return 0;
		}
		return -1;
	}
	//more than one intersections
	double max = 0;
	for (int i = 0; i < object_intersections.size(); i++) {
		if (max < object_intersections.at(i)) {
			max = object_intersections.at(i);
		}
	}
	if (max > 0) {
		for (int i = 0; i < object_intersections.size(); i++) {
			if (object_intersections.at(i) > 0 && object_intersections.at(i) <= max) {
				max = object_intersections.at(i);
				index_of_minimum_value = i;
			}
		}
		return index_of_minimum_value;
	}
	return -1;
}

color get_color_at(my_vector intersection_position, my_vector intersecting_ray_direction, vector<object*> scene_objects, int index_of_winning_object, vector<source*> light_sources, double accuracy, double ambientlight) {
	color winning_object_color = scene_objects.at(index_of_winning_object)->get_color();
	my_vector winning_object_normal = scene_objects.at(index_of_winning_object)->get_normal_at(intersection_position);

	if (winning_object_color.get_color_special() == 2) {
		//checkerboard
		const int square = int(floor(intersection_position.get_vect_x())) + int(floor(intersection_position.get_vect_z()));

		if ((square % 2) == 0) {
			winning_object_color.set_color_red(0);
			winning_object_color.set_color_green(0);
			winning_object_color.set_color_blue(0);
		}
		else {
			winning_object_color.set_color_red(1);
			winning_object_color.set_color_green(1);
			winning_object_color.set_color_blue(1);
		}
	}

	color final_color = winning_object_color.color_scalar(ambientlight);

	if (winning_object_color.get_color_special() > 0 && winning_object_color.get_color_special() <= 1) {
		//reflection from objects with spectral intensity
		const double dot1 = winning_object_normal.dot_product(intersecting_ray_direction.negative());
		my_vector scalar1 = winning_object_normal.vect_mult(dot1);
		my_vector add1 = scalar1.vect_add(intersecting_ray_direction);
		const my_vector scalar2 = add1.vect_mult(2);
		my_vector add2 = intersecting_ray_direction.negative().vect_add(scalar2);
		my_vector reflection_direction = add2.normalize();

		const ray reflection_ray(intersection_position, reflection_direction);

		//determine what the ray intersects with first
		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->find_intersection(reflection_ray));
		}

		const int index_of_winning_object_with_reflection = winning_object_index(reflection_intersections);

		if (index_of_winning_object_with_reflection != -1) {
			//reflection_ray missed everyting else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				//determine the position and direction at the point of intersection with the reflection ray
				// the ray only affect the color if it reflects off something

				const my_vector reflection_intersection_position = intersection_position.vect_add(reflection_direction.vect_mult(reflection_intersections.at(index_of_winning_object_with_reflection)));

				color reflection_intersection_color = get_color_at(reflection_intersection_position, reflection_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				final_color = final_color.color_add(reflection_intersection_color.color_scalar(winning_object_color.get_color_special()));
			}
		}
	}

	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		const my_vector light_direction = light_sources.at(light_index)->get_light_position().vect_add(intersection_position.negative()).normalize();

		const float cosine_angle = winning_object_normal.dot_product(light_direction);

		if (cosine_angle > 0) {
			//test fopr shadows
			bool shadowed = false;

			my_vector disance_to_light = light_sources.at(light_index)->get_light_position().vect_add(intersection_position.negative()).normalize();
			const float distance_to_light_magnitude = disance_to_light.magnitude(); //zinloos

			const ray shadow_ray(intersection_position, light_sources.at(light_index)->get_light_position().vect_add(intersection_position.negative()).normalize());

			vector<double> secondary_intersections;

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->find_intersection(shadow_ray));
			}

			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
					break;
				}
			}

			if (shadowed == false) {
				final_color = final_color.color_add(winning_object_color.color_multiply(light_sources.at(light_index)->get_light_color()).color_scalar(cosine_angle));

				if (winning_object_color.get_color_special() > 0 && winning_object_color.get_color_special() <= 1) {
					const double dot1 = winning_object_normal.dot_product(intersecting_ray_direction.negative());
					my_vector scalar1 = winning_object_normal.vect_mult(dot1);
					my_vector add1 = scalar1.vect_add(intersecting_ray_direction);
					const my_vector scalar2 = add1.vect_mult(2);
					my_vector add2 = intersecting_ray_direction.negative().vect_add(scalar2);
					my_vector reflection_direction = add2.normalize();

					double specular = reflection_direction.dot_product(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.color_add(light_sources.at(light_index)->get_light_color().color_scalar(specular * winning_object_color.get_color_special()));
					}
				}
			}
		}
	}

	return final_color.clip();
}

int thisone;

int main(int argc, char *argv[]) {
	cout << "rendering ..." << endl;

	const clock_t t1 = clock();

	const int dpi = 72;
	int width = 640;
	int height = 480;
	const int n = width*height;
	rgb_type *pixels = new rgb_type[n];

	const int aadepth = 2; // hoeveel anti-aliasing (0 = 1 pixel, 1 = 4 pixels, )
	double aspectratio = static_cast<double>(width) / static_cast<double>(height);
	const double ambientlight = 0.2;
	const double accuracy = 0.000001;

	const my_vector o(0, 0, 0);
	my_vector x(1, 0, 0);
	my_vector y(0, 1, 0);
	my_vector z(0, 0, 1);

	const my_vector new_sphere_location(1.7, 0, -0.7);

	my_vector campos(3, 1.5, -4);
	my_vector look_at(0, 0, 0);
	my_vector diff_btw(campos.get_vect_x() - look_at.get_vect_x(), campos.get_vect_y() - look_at.get_vect_y(), campos.get_vect_z() - look_at.get_vect_z());

	my_vector camdir = diff_btw.negative().normalize();
	my_vector camright = y.cross_product(camdir).normalize();
	my_vector camdown = camright.cross_product(camdir);
	camera scene_cam(campos, camdir, camright, camdown);

	const color white_light(1.0, 1.0, 1.0, 0);
	const color pretty_green(0.5, 1.0, 0.5, 0.5);
	const color maroon(0.5, 0.25, 0.25, .3);
	const color orange(0.94, 0.75, 0.31, 0);
	const color tile_floor(1, 1, 1, 2);

	const my_vector light_position(-7, 10, -10);
	light scene_light(light_position, white_light);
	vector<source*> light_sources;
	light_sources.push_back(dynamic_cast<source*>(&scene_light)); //voeg alle ligthsources toe

																  // scene objects
	sphere scene_sphere(o, 1, pretty_green);
	sphere scene_sphere2(new_sphere_location, .5, maroon);
	plane scene_plane(y, -1, tile_floor);
	triangle scene_triangle(my_vector(3, 0, 0), my_vector(0, 3, 0), my_vector(0, 0, 3), orange);

	vector<object*> scene_objects;
	scene_objects.push_back(dynamic_cast<object*>(&scene_sphere));//klopt hier iets van?
	scene_objects.push_back(dynamic_cast<object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<object*>(&scene_plane));
	scene_objects.push_back(dynamic_cast<object*>(&scene_triangle));

	double xamnt, yamnt;

	for (int x = 0; x < width; x++) {
		cout << (static_cast<double>(x) / static_cast<double>(width) * 100) << '\n';
		for (int y = 0; y < height; y++) {
			const int thisone = y*width + x;

			// start with blank pixel
			double temp_red[aadepth * aadepth];
			double temp_green[aadepth * aadepth];
			double temp_blue[aadepth * aadepth];

			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
					const int aa_index = aay * aadepth + aax;

					srand(time(0));

					//create the ray from the camera to this pxiel

					if (aadepth == 1) {
						//no anti-aliasing
						if (width > height) { //Dit if statement snap ik nog niet helemaal
							xamnt = ((x + 0.5) / width) * aspectratio - (((width - height) / static_cast<double>(height)) / 2);
							yamnt = ((height - y) + 0.5) / height;
						}
						else if (height > width) {
							xamnt = (x + 0.5) / width;
							yamnt = (((height - y) + 0.5) / height) / aspectratio - (((height - width) / static_cast<double>(width)) / 2);
						}
						else {
							xamnt = (x + 0.5) / width;
							yamnt = ((height - y) + 0.5) / height;
						}
					}
					else {
						//anti-aliasing
						if (width > height) { //Dit if statement snap ik nog niet helemaal
							xamnt = ((x + static_cast<double>(aax) / (static_cast<double>(aadepth) - 1)) / width) * aspectratio - (((width - height) / (double)height) / 2);
							yamnt = ((height - y) + (static_cast<double>(aax) / static_cast<double>(aadepth) - 1)) / height;
						}
						else if (height > width) {
							xamnt = (x + static_cast<double>(aax) / (static_cast<double>(aadepth) - 1)) / width;
							yamnt = (((height - y) + static_cast<double>(aax) / (static_cast<double>(aadepth) - 1)) / height) / aspectratio - (((height - width) / (double)width) / 2);
						}
						else {
							xamnt = (x + static_cast<double>(aax) / (static_cast<double>(aadepth) - 1)) / width;
							yamnt = ((height - y) + static_cast<double>(aax) / (static_cast<double>(aadepth) - 1)) / height;
						}
					}

					const ray cam_ray(
						scene_cam.get_camera_position(),
						camdir.vect_add(camright.vect_mult(xamnt - 0.5).vect_add(camdown.vect_mult(yamnt - 0.5))).normalize()
					);

					vector<double> intersections;

					for (int i = 0; i < scene_objects.size(); i++) {
						intersections.push_back(scene_objects.at(i)->find_intersection(cam_ray));
					}

					const int index_of_winning_object = winning_object_index(intersections);

					if (index_of_winning_object == -1) {
						//background
						temp_red[aa_index] = 0;
						temp_green[aa_index] = 0;
						temp_blue[aa_index] = 0;
					}
					else {
						if (intersections.at(index_of_winning_object) > accuracy) {
							const my_vector intersection_position = cam_ray.get_ray_origin().vect_add(cam_ray.get_ray_direction().vect_mult(intersections.at(index_of_winning_object)));
							const my_vector intersecting_ray_direction = cam_ray.get_ray_direction();

							color intersection_color = get_color_at(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);

							temp_red[aa_index] = intersection_color.get_color_red();
							temp_green[aa_index] = intersection_color.get_color_green();
							temp_blue[aa_index] = intersection_color.get_color_blue();
						}
					}
				}
			}
			//average pixel Color
			double total_red = 0;
			double total_green = 0;
			double total_blue = 0;

			for (int i_red = 0; i_red < aadepth * aadepth; i_red++) {//dit kan korter
				total_red = total_red + temp_red[i_red];
			}
			for (int i_green = 0; i_green < aadepth * aadepth; i_green++) {
				total_green = total_green + temp_green[i_green];
			}
			for (int i_blue = 0; i_blue < aadepth * aadepth; i_blue++) {
				total_blue = total_blue + temp_blue[i_blue];
			}

			const double avg_red = total_red / (aadepth * aadepth);
			const double avg_green = total_green / (aadepth * aadepth);
			const double avg_blue = total_blue / (aadepth * aadepth);

			pixels[thisone].r = avg_red;
			pixels[thisone].g = avg_green;
			pixels[thisone].b = avg_blue;
		}
	}

	filestream fs;
	fs.savembp("scene_AntiAliasedBig.bmp", width, height, dpi, pixels);
	//delete fs?

	const clock_t t2 = clock();
	const float diff = (static_cast<float>(t2) - static_cast<float>(t1)) / 1000;

	cout << diff << " seconds" << endl;

	return 0;
}