// RayTracer 3D Engine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Filestream.h"

#include "Vect.h"
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

int winningObjectIndex(vector<double> object_intersections) {
	// return the index of the winning intersection
	int index_of_minimum_value;

	//prevent unnecessary caculations
	if (object_intersections.size() == 0) {
		return -1;
	}
	else if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
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
		else {
			return -1;
		}
	}
}

Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight) {
	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

	if (winning_object_color.getColorSpecial() == 2) {
		//checkerboard
		int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());

		if ((square % 2) == 0) {
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorBlue(1);
		}
	}

	Color final_color = winning_object_color.colorScalar(ambientlight);

	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		//reflection from objects with spectral intensity
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();

		Ray reflection_ray(intersection_position, reflection_direction);

		//determine what the ray intersects with first
		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}

		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

		if (index_of_winning_object_with_reflection != -1) {
			//reflection_ray missed everyting else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				//determine the position and direction at the point of intersection with the reflection ray
				// the ray only affect the color if it reflects off something

				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;

				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}

	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();

		float cosine_angle = winning_object_normal.dotProduct(light_direction);

		if (cosine_angle > 0) {
			//test fopr shadows
			bool shadowed = false;

			Vect disance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = disance_to_light.magnitude(); //zinloos

			Ray shadow_ray(intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());

			vector<double> secondary_intersections;

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
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
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));

				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular * winning_object_color.getColorSpecial()));
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

	clock_t t1, t2;
	t1 = clock();

	int dpi = 72;
	int width = 640;
	int height = 480;
	int n = width*height;
	RGBType *pixels = new RGBType[n];

	const int aadepth = 2; // hoeveel anti-aliasing (0 = 1 pixel, 1 = 4 pixels, )
	double aathreshold = 0.1;
	double aspectratio = (double)width / (double)height;
	double ambientlight = 0.2;
	double accuracy = 0.000001;

	Vect O(0, 0, 0);
	Vect X(1, 0, 0);
	Vect Y(0, 1, 0);
	Vect Z(0, 0, 1);

	Vect new_sphere_location(1.7, 0, -0.7);

	Vect campos(3, 1.5, -4);
	Vect look_at(0, 0, 0);
	Vect diff_btw(campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());

	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam(campos, camdir, camright, camdown);

	Color white_light(1.0, 1.0, 1.0, 0);
	Color pretty_green(0.5, 1.0, 0.5, 0.5);
	Color maroon(0.5, 0.25, 0.25, .3);
	Color orange(0.94, 0.75, 0.31, 0);
	Color tile_floor(1, 1, 1, 2);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0.0, 0.0, 0.0, 0);

	Vect light_position(-7, 10, -10);
	Light scene_light(light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light)); //voeg alle ligthsources toe

																  // scene objects
	Sphere scene_sphere(O, 1, pretty_green);
	Sphere scene_sphere2(new_sphere_location, .5, maroon);
	Plane scene_plane(Y, -1, tile_floor);
	Triangle scene_triangle(Vect(3, 0, 0), Vect(0, 3, 0), Vect(0, 0, 3), orange);

	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));//klopt hier iets van?
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_triangle));

	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;

	for (int x = 0; x < width; x++) {
		cout << ((double)x / (double)width * 100) << '\n';
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;

			// start with blank pixel
			double tempRed[aadepth * aadepth];
			double tempGreen[aadepth * aadepth];
			double tempBlue[aadepth * aadepth];

			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
					aa_index = aay * aadepth + aax;

					srand(time(0));

					//create the ray from the camera to this pxiel

					if (aadepth == 1) {
						//no anti-aliasing
						if (width > height) { //Dit if statement snap ik nog niet helemaal
							xamnt = ((x + 0.5) / width) * aspectratio - (((width - height) / (double)height) / 2);
							yamnt = ((height - y) + 0.5) / height;
						}
						else if (height > width) {
							xamnt = (x + 0.5) / width;
							yamnt = (((height - y) + 0.5) / height) / aspectratio - (((height - width) / (double)width) / 2);
						}
						else {
							xamnt = (x + 0.5) / width;
							yamnt = ((height - y) + 0.5) / height;
						}
					}
					else {
						//anti-aliasing
						if (width > height) { //Dit if statement snap ik nog niet helemaal
							xamnt = ((x + (double)aax / ((double)aadepth - 1)) / width) * aspectratio - (((width - height) / (double)height) / 2);
							yamnt = ((height - y) + ((double)aax / (double)aadepth - 1)) / height;
						}
						else if (height > width) {
							xamnt = (x + (double)aax / ((double)aadepth - 1)) / width;
							yamnt = (((height - y) + (double)aax / ((double)aadepth - 1)) / height) / aspectratio - (((height - width) / (double)width) / 2);
						}
						else {
							xamnt = (x + (double)aax / ((double)aadepth - 1)) / width;
							yamnt = ((height - y) + (double)aax / ((double)aadepth - 1)) / height;
						}
					}

					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();

					Ray cam_ray(cam_ray_origin, cam_ray_direction);

					vector<double> intersections;

					for (int i = 0; i < scene_objects.size(); i++) {
						intersections.push_back(scene_objects.at(i)->findIntersection(cam_ray));
					}

					int index_of_winning_object = winningObjectIndex(intersections);

					if (index_of_winning_object == -1) {
						//background
						tempRed[aa_index] = 0;
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else {
						if (intersections.at(index_of_winning_object) > accuracy) {
							Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
							Vect intersecting_ray_direction = cam_ray_direction;

							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);

							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();
						}
					}
				}
			}
			//average pixel Color
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;

			for (int iRed = 0; iRed < aadepth * aadepth; iRed++) {//dit kan korter
				totalRed = totalRed + tempRed[iRed];
			}
			for (int iGreen = 0; iGreen < aadepth * aadepth; iGreen++) {
				totalGreen = totalGreen + tempGreen[iGreen];
			}
			for (int iBlue = 0; iBlue < aadepth * aadepth; iBlue++) {
				totalBlue = totalBlue + tempBlue[iBlue];
			}

			double avgRed = totalRed / (aadepth * aadepth);
			double avgGreen = totalGreen / (aadepth * aadepth);
			double avgBlue = totalBlue / (aadepth * aadepth);

			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
		}
	}

	Filestream::savembp("scene_AntiAliasedBig.bmp", width, height, dpi, pixels);

	delete pixels, tempRed, tempGreen, tempBlue;

	t2 = clock();
	float diff = ((float)t2 - (float)t1) / 1000;

	cout << diff << " seconds" << endl;

	return 0;
}