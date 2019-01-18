/*
 * tools.h
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#ifndef SRC_TOOLS_HPP_
#define SRC_TOOLS_HPP_

#include <math.h>
#include "global.hpp"

#define POW_2(x)		(x * x)
#define POW_3(x)		(x * x * x)
#define POW_4(x)		(POW_2(x) * POW_2(x))

uint32_t file_count_lines(const char *pathToFile);
void getMinMaxFromArray(float * min, float * max, float array[], uint32_t size);
uint64_t getTimeOfDayInUs(void);
void print_vector3(struct vector3_t vector3);
void print_matrix4x4(float m4x4[16]);
float to_degrees(float radians);
float to_radians(float degrees);
float vector3_multiply(struct vector3_t a, struct vector3_t b);
void matrix4x4_multiply(float result4x4[16], float a4x4[16], float b4x4[16]);
void matrix4x4_multiply_by(float a4x4[16], float b4x4[16]);
void translate(float matrix4x4[16], float x, float y, float z);
void rotate(float matrix4x4[16], float t_deg, float x, float y, float z);
void print_matrix(const float matrix4x4[16]);
void set_matrix_identity(float matrix4x4[16]);
void set_matrix_null(float matrix4x4[16]);
void set_vector3(struct vector3_t * vector3, float x, float y, float z);
float normalize_vector3(struct vector3_t * vector3);
void get_perpendicular_vector3(struct vector3_t * out, struct vector3_t * in);
void cross_product_vector3(struct vector3_t *, struct vector3_t *, struct vector3_t *);
void rotate_landmark(struct vector3_t * outVect, const struct vector3_t inVect, const struct vector3_t teta);
void rotate_landmark_y(struct vector3_t * outVect, const struct vector3_t inVect, float tetaY);
bool get_matrix_inverse(float invOut[16], const float m[16]);
float get_matrix_det(const float m[16]);
void get_fps_modelview(float outModelview[16], struct vector3_t eye, float pitch, float yaw);

#endif /* SRC_TOOLS_HPP_ */
