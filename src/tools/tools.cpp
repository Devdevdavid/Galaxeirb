/*
 * tools.c
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#include "tools.hpp"

/**
 * Count the number of '\n' in a file
 * @param pathToFile
 * @return -1: Error, > 0: number of lines in the file
 */
uint32_t file_count_lines(const char *pathToFile)
{
	FILE *fp = NULL;
	int ch = 0;
	int lineCount = 0;

	if ((fp = fopen(pathToFile, "r")) == NULL) {
		return -1;
	}

	lineCount++;
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n') {
			lineCount++;
		}
	}
	fclose(fp);

	return lineCount;
}

/**
 * Get the max and min from an array of float
 */
void getMinMaxFromArray(float * min, float * max, float array[], uint32_t size)
{
	uint32_t index;

	if (size <= 0) {
		return;
	}

	(*min) = array[0];
	(*max) = array[0];

	for (index = 0; index < size; ++index) {
		if (array[index] > (*max)) {
			(*max) = array[index];
		}
		if (array[index] < (*min)) {
			(*min) = array[index];
		}
	}
}

/**
 * Get the current time in us
 * @return the current time in us
 */
uint64_t getTimeOfDayInUs(void)
{
	struct timeval curTime;

	gettimeofday(&curTime, NULL);

	return curTime.tv_sec * 1000000 + curTime.tv_usec;
}

/**
 * Print the 3 parameter of a vector 3 with a Debug level output
 * @param vector3
 */
void print_vector3(struct vector3_t vector3)
{
	log_debug("Vector3: x = %f, y = %f, z = %f", vector3.x, vector3.y, vector3.z);
}

/**
 * Print the 3 parameter of a vector 3 with a Debug level output
 * @param vector3
 */
void print_matrix4x4(float m4x4[16])
{
	log_info("m4x4: %5.1f, %5.1f, %5.1f, %5.1f", m4x4[0], m4x4[4], m4x4[8], m4x4[12]);
	log_info("      %5.1f, %5.1f, %5.1f, %5.1f", m4x4[1], m4x4[5], m4x4[9], m4x4[13]);
	log_info("      %5.1f, %5.1f, %5.1f, %5.1f", m4x4[2], m4x4[6], m4x4[10], m4x4[14]);
	log_info("      %5.1f, %5.1f, %5.1f, %5.1f", m4x4[3], m4x4[7], m4x4[11], m4x4[15]);
}

/**
 * Convert radian to degree
 * @param radians
 * @return
 */
float to_degrees(float radians)
{
	return radians * (180.0 / M_PI);
}
/**
 * Convert radian to degree
 * @param degrees
 * @return
 */
float to_radians(float degrees)
{
	return degrees * (M_PI / 180.0);
}

/**
 * Multiply two matrix a and b and save result
 * @param a
 * @param b
 * @return the dot product of a by b
 */
float vector3_multiply(struct vector3_t a, struct vector3_t b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * Multiply two matrix a and b and save result
 * @param result4x4
 * @param a4x4
 * @param b4x4
 */
void matrix4x4_multiply(float result4x4[16], float a4x4[16], float b4x4[16])
{
	uint8_t i, j, k;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 4; k++) {
				result4x4[4 * i + j] += a4x4[4 * i + k] * b4x4[4 * k + j];
			}
		}
	}
}

/**
 * Multiply a by b and save result in a
 * @param a4x4
 * @param b4x4
 */
void matrix4x4_multiply_by(float a4x4[16], float b4x4[16])
{
	float result4x4[16] = {0.0f};
	matrix4x4_multiply(result4x4, a4x4, b4x4);
	memcpy(a4x4, result4x4, sizeof(result4x4));
}

void translate(float matrix4x4[16], float x, float y, float z)
{
	float translate[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1
	};

	matrix4x4_multiply_by(matrix4x4, translate);
}

void rotate(float matrix4x4[16], float t_deg, float x, float y, float z)
{
	float teta = to_radians(t_deg); // teta is in radians

	float cos_teta = cos(teta);
	float sin_teta = sin(teta);

	float rotate[16] = {
			x*x*(1-cos_teta) + cos_teta,	x*y*(1-cos_teta) - z*sin_teta,	x*z*(1-cos_teta) + y*sin_teta,	0,
			x*y*(1-cos_teta) + z*sin_teta,	y*y*(1-cos_teta) + cos_teta,	y*z*(1-cos_teta) - x*sin_teta,	0,
			x*z*(1-cos_teta) - y*sin_teta,	y*z*(1-cos_teta) + x*sin_teta,	z*z*(1-cos_teta) + cos_teta,	0,
			0, 0, 0, 1
	};

	matrix4x4_multiply_by(matrix4x4, rotate);
}

void print_matrix(const float matrix4x4[16])
{
	uint8_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%.2f ", matrix4x4[4 * i + j]);
		}
		printf("\n");
	}
}

void set_matrix_identity(float matrix4x4[16])
{
	uint8_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (i == j) {
				matrix4x4[4 * i + j] = 1;
			} else {
				matrix4x4[4 * i + j] = 0;
			}
		}
	}
}
void set_matrix_null(float matrix4x4[16])
{
	uint8_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			matrix4x4[4 * i + j] = 0;
		}
	}
}

void set_vector3(struct vector3_t * vector3, float x, float y, float z)
{
	vector3->x = x;
	vector3->y = y;
	vector3->z = z;
}

float normalize_vector3(struct vector3_t * vector3)
{
	float delta = sqrt(POW_2(vector3->x) + POW_2(vector3->y) + POW_2(vector3->z));

	vector3->x /= delta;
	vector3->y /= delta;
	vector3->z /= delta;

	return delta;
}

void get_perpendicular_vector3(struct vector3_t * out, struct vector3_t * in)
{
	out->x = 0; out->y = 0; out->z = 0;

	if (in->x == 0) 		{ out->x = 1; }
	else if (in->y == 0) 	{ out->y = 1; }
	else if (in->z == 0) 	{ out->z = 1; }
	else {
		out->x = -in->y;
		out->y = in->x;
	}
}

void cross_product_vector3(struct vector3_t * out, struct vector3_t * a, struct vector3_t * b)
{
	out->x = 0; out->y = 0; out->z = 0;

	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

void rotate_landmark(struct vector3_t * outVect, const struct vector3_t inVect, const struct vector3_t teta)
{
	outVect->x = inVect.x * cos(teta.y) + inVect.z * sin(teta.y) * cos(teta.x);
	outVect->y = - inVect.z * sin(teta.x);
	outVect->z = - inVect.x * sin(teta.y) + inVect.z * cos(teta.y) * cos(teta.x);
}

void rotate_landmark_y(struct vector3_t * outVect, const struct vector3_t inVect, float tetaY)
{
	outVect->x = inVect.x * cos(tetaY) + inVect.z * sin(tetaY);
	outVect->y = inVect.y;
	outVect->z = - inVect.x * sin(tetaY) + inVect.z * cos(tetaY);
}

/**
 * Compute the inverse of the matrix m and store the result in invOut
 * @param invOut: Output matrix
 * @param m: input matrix
 * @return true: OK, false: null determinant
 */
bool get_matrix_inverse(float invOut[16], const float m[16])
{
	float inv[16], det;
	uint8_t i;

	inv[0] = m[5]  * m[10] * m[15] -
			m[5]  * m[11] * m[14] -
			m[9]  * m[6]  * m[15] +
			m[9]  * m[7]  * m[14] +
			m[13] * m[6]  * m[11] -
			m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
			m[4]  * m[11] * m[14] +
			m[8]  * m[6]  * m[15] -
			m[8]  * m[7]  * m[14] -
			m[12] * m[6]  * m[11] +
			m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] -
			m[4]  * m[11] * m[13] -
			m[8]  * m[5] * m[15] +
			m[8]  * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] +
			m[4]  * m[10] * m[13] +
			m[8]  * m[5] * m[14] -
			m[8]  * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
			m[1]  * m[11] * m[14] +
			m[9]  * m[2] * m[15] -
			m[9]  * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
			m[0]  * m[11] * m[14] -
			m[8]  * m[2] * m[15] +
			m[8]  * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] +
			m[0]  * m[11] * m[13] +
			m[8]  * m[1] * m[15] -
			m[8]  * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] -
			m[0]  * m[10] * m[13] -
			m[8]  * m[1] * m[14] +
			m[8]  * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
			m[1]  * m[7] * m[14] -
			m[5]  * m[2] * m[15] +
			m[5]  * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
			m[0]  * m[7] * m[14] +
			m[4]  * m[2] * m[15] -
			m[4]  * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
			m[0]  * m[7] * m[13] -
			m[4]  * m[1] * m[15] +
			m[4]  * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
			m[0]  * m[6] * m[13] +
			m[4]  * m[1] * m[14] -
			m[4]  * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0) {
		return false;
	}

	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		invOut[i] = inv[i] * det;
	}

	return true;
}

float get_matrix_det(const float m[16])
{
	float inv[16], det;

	inv[0] = m[5]  * m[10] * m[15] -
			m[5]  * m[11] * m[14] -
			m[9]  * m[6]  * m[15] +
			m[9]  * m[7]  * m[14] +
			m[13] * m[6]  * m[11] -
			m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
			m[4]  * m[11] * m[14] +
			m[8]  * m[6]  * m[15] -
			m[8]  * m[7]  * m[14] -
			m[12] * m[6]  * m[11] +
			m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] -
			m[4]  * m[11] * m[13] -
			m[8]  * m[5] * m[15] +
			m[8]  * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] +
			m[4]  * m[10] * m[13] +
			m[8]  * m[5] * m[14] -
			m[8]  * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	return det;
}

/**
 * Compute the modelview of the a camera with the specified eye, pitch and yaw
 * @param outModelview
 * @param eye
 * @param pitch
 * @param yaw
 */
void get_fps_modelview(float outModelview[16], struct vector3_t eye, float pitch, float yaw)
{
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	struct vector3_t xaxis = { cosYaw, 0, -sinYaw };
	struct vector3_t yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
	struct vector3_t zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

	outModelview[0] = xaxis.x;
	outModelview[1] = yaxis.x;
	outModelview[2] = zaxis.x;
	outModelview[3] = 0;

	outModelview[4] = xaxis.y;
	outModelview[5] = yaxis.y;
	outModelview[6] = zaxis.y;
	outModelview[7] = 0;

	outModelview[8] = xaxis.z;
	outModelview[9] = yaxis.z;
	outModelview[10] = zaxis.z;
	outModelview[11] = 0;

	outModelview[12] = -vector3_multiply(xaxis, eye);
	outModelview[13] = -vector3_multiply(yaxis, eye);
	outModelview[14] = -vector3_multiply(zaxis, eye);
	outModelview[15] = 1;
}

