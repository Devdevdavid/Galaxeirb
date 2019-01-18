/*
 * controler.c
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#define SRC_CONTROLLER_C_

#include "controller.hpp"

/**
 * ===============================
 * 		OBJECTS MANAGEMENT
 * ===============================
 */

/**
 * Init the universe by adding the entity of the file
 * @param universe
 * @return 0: OK, -1: Error
 */
int32_t init_universe(struct universe_t * universe)
{
	uint32_t choosedEntityTrace;
	uint8_t index;

#ifndef STAR_WARS_ENABLE
	/** Parse file for first time */
	if (parse_file_entity_data(universe, "ressource/dubinski2.tab") != 0) {
		log_error("Unable to parse file correctly");
		return -1;
	}
#else
	if (tie_fighter_entity_data(universe) != 0) {
		log_error("Unable to create tie hunter correctly");
		return -1;
	}
#endif

	if (ENTITY_TRACE_COUNT > 0) {
		universe->entityTrace = (struct entityTrace_t *) malloc(ENTITY_TRACE_COUNT * sizeof(struct entityTrace_t));
		if (universe->entityTrace == NULL) {
			log_error("Unable to allocate entity trace");
			return -1;
		}

		for (index = 0; index < ENTITY_TRACE_COUNT; ++index) {
			// Choose a random entity to trace
			choosedEntityTrace = rand() % universe->entityCount;
			init_entityTrace(&universe->entityTrace[index], &universe->entities[choosedEntityTrace]);
		}
	}

	// Init entity blaster system
	init_entityBlaster(&universe->blaster);

	return 0;
}

/**
 * Allocate entity to universe
 * @param universe:
 * @param entityCountToAdd: the number of entity to add
 * @return 0: OK, -1: Error
 */
int32_t alloc_entities(struct universe_t *universe, uint32_t entityCount)
{
	uint32_t index;

	universe->entities = (struct entity_t *) malloc(entityCount * sizeof(struct entity_t));
	if (universe->entities == NULL) {
		log_error("Entities allocation failed");
		return -1;
	}

	universe->mass = (float *) malloc(entityCount * sizeof(float));
	if (universe->mass == NULL) {
		log_error("Mass allocation failed");
		return -1;
	}

	universe->position = (struct vector3_t *) malloc(entityCount * sizeof(struct vector3_t));
	if (universe->position == NULL) {
		log_error("Position allocation failed");
		return -1;
	}

	universe->currVelocity = (struct vector3_t *) malloc(entityCount * sizeof(struct vector3_t));
	if (universe->currVelocity == NULL) {
		log_error("Current velocity allocation failed");
		return -1;
	}

	universe->newVelocity = (struct vector3_t *) malloc(entityCount * sizeof(struct vector3_t));
	if (universe->newVelocity == NULL) {
		log_error("New velocity allocation failed");
		return -1;
	}

	universe->color = (struct colorRGB_t *) malloc(entityCount * sizeof(struct colorRGB_t));
	if (universe->color == NULL) {
		log_error("Colors allocation failed");
		return -1;
	}

	universe->entityCount = entityCount;

	// For all new entities, update their pointers and colors
	for (index = 0; index < universe->entityCount; index++) {
		universe->entities[index].mass = &universe->mass[index];
		universe->entities[index].pos = &universe->position[index];
		universe->entities[index].currVelocity = &universe->currVelocity[index];
		universe->entities[index].newVelocity = &universe->newVelocity[index];

		// Default color is white
		universe->color[index].r = 1.0f;
		universe->color[index].g = 1.0f;
		universe->color[index].b = 1.0f;
	}
	universe->isColorNeedLoad = true;

	log_info("Universe has now %d entities", universe->entityCount);

	return 0;
}

/**
 * Init the entity trace system
 * @param entityTrace : struct to init
 * @param entity : entity to follow, NULL if nothing to follow
 */
void init_entityTrace(struct entityTrace_t * entityTrace, struct entity_t * entity)
{
	uint32_t index;
	float colorIndic;
	struct colorRGB_t colorRand;

	// Update pointer
	entityTrace->entity = entity;

	if (entityTrace->entity == NULL) {
		memset(entityTrace->position, 0, ENTITY_TRACE_LENGTH * sizeof(struct vector3_t));
	} else {
		// At the beginning all points are at the start position of entity
		for (index = 0; index < ENTITY_TRACE_LENGTH; ++index) {
			entityTrace->position[index].x = entity->pos->x;
			entityTrace->position[index].y = entity->pos->y;
			entityTrace->position[index].z = entity->pos->z;
		}
	}

	// Choose a random color to apply
	colorRand.r = ((float) rand()) / ((float) RAND_MAX);
	colorRand.g = ((float) rand()) / ((float) RAND_MAX);
	colorRand.b = ((float) rand()) / ((float) RAND_MAX);

	for (index = 0; index < ENTITY_TRACE_LENGTH; index++) {
		// Make a nice color disapearance with a cos()
		colorIndic = 0.5 * (1 + cos(M_PI * ((float) index / ENTITY_TRACE_LENGTH)));
		entityTrace->color[index].r = colorIndic * colorRand.r;
		entityTrace->color[index].g = colorIndic * colorRand.g;
		entityTrace->color[index].b = colorIndic * colorRand.b;
	}
}

/**
 * Init the entity blaster system
 * @param entityBlaster : struct to init
 */
void init_entityBlaster(struct blaster_t * entityBlaster)
{
	uint32_t index;

	memset(entityBlaster->position, 0, BLASTER_LENGTH * sizeof(struct vector3_t));
	memset(entityBlaster->currVelocity, 0, BLASTER_LENGTH * sizeof(struct vector3_t));

	for (index = 0; index < BLASTER_LENGTH; index++) {
		entityBlaster->color[index].r = 255;
		entityBlaster->color[index].g = 0;
		entityBlaster->color[index].b = 0;
	}
}

/**
 * Free all previous allocations for entities and galaxies
 * @param universe: master object universe
 */
void free_universe(struct universe_t *universe)
{
	free(universe->entities);
	free(universe->position);
	free(universe->currVelocity);
	free(universe->newVelocity);
	free(universe->color);
	free(universe->entityTrace);
	universe->entityCount = 0;
}

/**
 * ===============================
 * 		3D Creation Tools
 * ===============================
 */

/**
 * Create a rectangle in 3D [Not optimised]
 * @param countMode : if true, the function do noting on curEntity, just return the number of entity needed
 * @param curEntity : First entity to use
 * @param startPos : start from this corner positions
 * @param endPos : end to this corner positions
 * @return the number of entity used to create the rectangle
 */
uint32_t create_rectangle(bool countMode, struct entity_t * curEntity, struct vector3_t startPos, struct vector3_t endPos)
{
	uint16_t nbEntities = 0;
	struct vector3_t delta, cur;

	/** Compute the slope vector delta */
	delta.x = endPos.x - startPos.x;
	delta.y = endPos.y - startPos.y;
	delta.z = endPos.z - startPos.z;

	for (cur.z = 0; cur.z <= delta.z; ++cur.z) {
		for (cur.y = 0; cur.y <= delta.y; ++cur.y) {
			for (cur.x = 0; cur.x <= delta.x; ++cur.x) {
				if (!countMode) {
					curEntity->pos->x = startPos.x + cur.x;
					curEntity->pos->y = startPos.y + cur.y;
					curEntity->pos->z = startPos.z + cur.z;
					curEntity++;
				}

				nbEntities++;
			}
		}
	}

	return nbEntities;
}

/**
 * Create a spere in 3D [Not optimised]
 * @param countMode : if true, the function do noting on curEntity, just return the number of entity needed
 * @param curEntity : First entity to use
 * @param middlePos : Position of the center of the spere
 * @param radius : radius of the spere
 * @return the number of entity used to create the spere
 */
uint32_t create_sphere(bool countMode, struct entity_t * curEntity, struct vector3_t middlePos, float radius)
{
	uint32_t nbPointOnDisc = 32;
	uint16_t tetaX, tetaY;
	float s, t;

	if (!countMode) {
		for (tetaY = 0; tetaY < nbPointOnDisc; ++tetaY) {
			s = tetaY * 2.0f * 3.14f / nbPointOnDisc;

			for (tetaX = 0; tetaX < nbPointOnDisc; ++tetaX) {
				t = tetaX * 2.0f * 3.14f / nbPointOnDisc;


				curEntity->pos->x = middlePos.x + radius * cos(s) * sin(t);
				curEntity->pos->y = middlePos.y + radius * sin(s) * sin(t);
				curEntity->pos->z = middlePos.z + radius * cos(t);

				curEntity++;
			}
		}
	}

	return POW_2(nbPointOnDisc);
}

/**
 * Create a cylinder in 3D [Not optimised]
 * @param countMode : if true, the function do noting on curEntity, just return the number of entity needed
 * @param curEntity : First entity to use
 * @param startPos : Start position of the cyclinder
 * @param endPos : End position of the cylinder
 * @param radius : radius of the cylinder
 * @return the number of entity used to create the cylinder
 */
uint32_t create_cylinder(bool countMode, struct entity_t * curEntity, struct vector3_t startPos, struct vector3_t endPos, float radius)
{
	uint32_t nbPointOnDisc = 16;
	uint32_t nbPointOnLength = 16;
	uint16_t teta, nbEntities = 0;
	float t;
	struct vector3_t delta, perpDelta1, perpDelta2;
	float length, step, cursor;

	/** Compute the slope vector delta */
	delta.x = endPos.x - startPos.x;
	delta.y = endPos.y - startPos.y;
	delta.z = endPos.z - startPos.z;

	/** Normalize delta and get distance */
	length = normalize_vector3(&delta);

	/** Get first perpendicular vector */
	get_perpendicular_vector3(&perpDelta1, &delta);

	/** Get second perpendicular vector */
	cross_product_vector3(&perpDelta2, &delta, &perpDelta1);

	/** Compute the step between two circles */
	step = length / nbPointOnLength;

	for (cursor = 0; cursor <= length; cursor += step) {
		for (teta = 0; teta < nbPointOnDisc; ++teta) {
			t = teta * 2.0f * 3.14f / nbPointOnDisc;

			if (!countMode) {
				curEntity->pos->x = startPos.x + radius * (perpDelta1.x * cos(t) + perpDelta2.x * sin(t));
				curEntity->pos->y = startPos.y + radius * (perpDelta1.y * cos(t) + perpDelta2.y * sin(t));
				curEntity->pos->z = startPos.z + radius * (perpDelta1.z * cos(t) + perpDelta2.z * sin(t));
				curEntity++;
			}

			nbEntities++;
		}

		/** Jump to the next circle middle */
		startPos.x += step * delta.x;
		startPos.y += step * delta.y;
		startPos.z += step * delta.z;
	}

	return nbEntities;
}

/**
 * Build a TIE FIGHTER from Star Wars
 * @param countMode : if true, the function do noting on curEntity, just return the number of entity needed
 * @param curEntity : First entity to use
 * @return the number of entity used to create the tie fighter
 */
uint32_t build_tie_fighter(bool countMode, struct entity_t * curEntity)
{
	uint32_t nbEntities = 0;
	struct vector2_t wingsSize = {9.0f, 6.0f};
	struct vector3_t pos1 = {0.0f, 0.0f, 0.0f};
	struct vector3_t pos2 = {5.0f, 0.0f, 5.0f};

	set_vector3(&pos1, 0.0f, 0.0f, 0.0f);
	nbEntities += create_sphere(countMode, curEntity + nbEntities, pos1, 5);

	set_vector3(&pos1, 5.0f, 0.0f, 0.0f);
	set_vector3(&pos2, 10.0f, 0.0f, 0.0f);
	nbEntities += create_cylinder(countMode, curEntity + nbEntities, pos1, pos2, 1);

	set_vector3(&pos1, 10.0f, -wingsSize.x, -wingsSize.y);
	set_vector3(&pos2, 11.0f, wingsSize.x, wingsSize.y);
	nbEntities += create_rectangle(countMode, curEntity + nbEntities, pos1, pos2);

	set_vector3(&pos1, -5.0f, 0.0f, 0.0f);
	set_vector3(&pos2, -10.0f, 0.0f, 0.0f);
	nbEntities += create_cylinder(countMode, curEntity + nbEntities, pos1, pos2, 1);

	set_vector3(&pos1, -11.0f, -wingsSize.x, -wingsSize.y);
	set_vector3(&pos2, -10.0f, wingsSize.x, wingsSize.y);
	nbEntities += create_rectangle(countMode, curEntity + nbEntities, pos1, pos2);

	set_vector3(&pos1, 70.0f, 10.0f, -70.0f);
	//nbEntities += create_sphere(countMode, curEntity + nbEntities, pos1, 30);

	return nbEntities;
}

/**
 * This function create a universe from 3D generation function instead of reading entity information from a file
 * @param universe
 * @return -1: Error, 0: OK
 */
int32_t tie_fighter_entity_data(struct universe_t * universe)
{
	int32_t retValue = 0;
	uint32_t nbEntities = 0, index;

	/** Count the number of entities needed */
	nbEntities = build_tie_fighter(true, NULL);

	/** Create all needed entities */
	log_info("%d entities detected", nbEntities);
	if (alloc_entities(universe, nbEntities) != 0) {
		free_universe(universe);
		return -1;
	}

	/** Build it for real */
	build_tie_fighter(false, universe->entities);

	for (index = 0; index < nbEntities; ++index) {
		*universe->entities[index].mass = 0.0000498914f;
	}

	return retValue;
}

/**
 * ===============================
 * 		PARSE ENTITY FILE
 * ===============================
 */

/**
 * Parse a line of the entities data file
 * @param fp
 * @param entity
 * @return 0: OK, -1: Error
 */
int32_t parse_line_entity_data(FILE *fp, struct entity_t *entity)
{
	bool isFinished = false;
	uint8_t argIndex = 0;
	char valueStr[20];
	uint8_t valueStrIndex = 0;
	float value;

	while (!isFinished) {
		// Read character
		valueStr[valueStrIndex] = fgetc(fp);

		// Test the character
		switch (valueStr[valueStrIndex]) {
		case ' ':
			valueStr[valueStrIndex] = '\0';
			value = atof(valueStr);
			valueStrIndex = 0;

			switch (argIndex) {
			case 0: (*entity->mass) = value; break;
			case 1: (*entity->pos).x = value; break;
			case 2: (*entity->pos).y = value; break;
			case 3: (*entity->pos).z = value; break;
			case 4: (*entity->currVelocity).x = value; break;
			case 5: (*entity->currVelocity).y = value; break;
			case 6: (*entity->currVelocity).z = value; break;
			default:
				log_error("Wrong file format (more than 6 arguments)");
				isFinished = true;
				break;
			}
			argIndex++;

			break;

			//case -1:
			case '\n':
				isFinished = true;
				break;

			default:
				valueStrIndex++;
				break;
		}
	}

	// Return OK only if we have 7 arguments
	return (argIndex == 7) ? 0 : -1;
}

/**
 * Parse the input file and create the entities needed
 * @param galaxy
 * @param pathToFile
 * @return 0: OK, -1: Error
 */
int32_t parse_file_entity_data(struct universe_t * universe, const char pathToFile[])
{
	int32_t retValue = 0;
	FILE *fp = NULL;
	int32_t numberOfLines;
	uint32_t index;

	/** Count the number of line (number of entity to allocate) */
	numberOfLines = file_count_lines(pathToFile);
	if (numberOfLines <= 0) {
		log_error("Wrong input file: \"%s\"", pathToFile);
		return -1;
	}
	log_info("%d lines detected", numberOfLines);

	/** Create all needed entities (The last line is not an entity) */
	if (alloc_entities(universe, numberOfLines - 1) != 0) {
		free_universe(universe);
		return -1;
	}

	/** Read the file and parse */
	if ((fp = fopen(pathToFile, "r")) == NULL) {
		return -1;
	}

	/** Read data to fill entities value */
	for (index = 0; index < universe->entityCount; ++index) {
		if (parse_line_entity_data(fp, &universe->entities[index]) != 0) {
			retValue = -1;
			break;
		}
	}

	/** CLose file */
	fclose(fp);

	return retValue;
}

/**
 * ===============================
 * 		ACTION ON ENTITIES
 * ===============================
 */

/**
 * Update the color of each entities according to the specified color pattern
 * @param universe
 * @param color_mode
 * @return
 */
int32_t color_entities(struct universe_t *universe, enum color_mode_e color_mode)
{
	uint32_t index;
	float colorIndic;

	/** Color computation according to the mass */
	switch (color_mode) {
	case MASS:
		float min, max;
		getMinMaxFromArray(&min, &max, universe->mass, universe->entityCount);
		for (index = 0; index < universe->entityCount; index++) {
			colorIndic = ((*universe->entities[index].mass) - min) / (max - min);
			universe->color[index].r = 1.0f;
			universe->color[index].g = 1.0f - colorIndic;
			universe->color[index].b = 0.0f;
		}
		break;

	case RANDOM:
		for (index = 0; index < universe->entityCount; index++) {
			universe->color[index].r = ((float) rand()) / ((float) RAND_MAX);
			universe->color[index].g = ((float) rand()) / ((float) RAND_MAX);
			universe->color[index].b = ((float) rand()) / ((float) RAND_MAX);
		}
		break;

	case WHITE:
	default:
		for (index = 0; index < universe->entityCount; index++) {
			universe->color[index].r = 1.0f;
			universe->color[index].g = 1.0f;
			universe->color[index].b = 1.0f;
		}
		break;
	}
	universe->isColorNeedLoad = true;

	return 0;
}

/**
 * ===============================
 * 		DEBUG FUNCTIONS
 * ===============================
 */

void print_entity_data(struct entity_t *entity)
{
	log_debug("> Entity: mass = %f", entity->mass);
	log_debug("         x=%f,  y=%f,  z=%f",
			(*entity->pos).x, (*entity->pos).y, (*entity->pos).z);
	log_debug("        vx=%f, vy=%f, vz=%f",
			(*entity->currVelocity).x, (*entity->currVelocity).y, (*entity->currVelocity).z);
}

void print_universe_data(struct universe_t *universe)
{
	log_debug("Print universe: %d entities", universe->entityCount);

	for (uint32_t index = 0; index < universe->entityCount; ++index) {
		print_entity_data(&universe->entities[index]);
	}
}
