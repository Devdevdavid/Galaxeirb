/*
 * spaceInterraction.c
 *
 *  Created on: Oct 3, 2018
 *      Author: David
 */

#define SRC_SPACE_INTERRACTION_C_
#include "space_interraction.hpp"

uint64_t lastInteractionUpdate1; 	/**< Date of the last interraction update */
uint32_t entityPerThread;				/**< Number of entity per thread (Equals to universe->entityCount if Threads disables)*/
#ifdef THREAD_ENABLE
thread_t * thread[NB_THREAD];			/**< Thread ID */
#endif

/**
 * Init global variable for space interraction
 */
void space_interraction_init(void)
{
	si_timeFactor = DEFAULT_TIME_FACTOR;

#ifdef THREAD_ENABLE
	entityPerThread = universe.entityCount / NB_THREAD;
	log_info("%d threads with %d entities each", NB_THREAD, entityPerThread);

	uint8_t index;
	for (index = 0; index < NB_THREAD; ++index) {
		thread[index] = NewThread();

		StartWorkerThread(thread[index], "compute", compute_new_velocity,
				(void*) &universe.entities[index * entityPerThread],
				CORE_ANY, THREAD_NORMAL, DEFAULT_THREAD_STACK_SIZE);
	}
#else
	entityPerThread = universe.entityCount;
#endif
}

/**
 * Free the worker threads
 */
void space_interraction_free(void)
{
#ifdef THREAD_ENABLE
	uint8_t index;

	for (index = 0; index < NB_THREAD; ++index) {
		DeleteThread(thread[index]);
	}
#endif
}

/**
 * Conflict a given entity with all others of the galaxy to find the resulting velocity
 * @param universe
 * @param cur_entity
 */
int compute_new_velocity(void * arg)
{
	register float curEntityPosX, curEntityPosY, curEntityPosZ;
	struct entity_t *entity;
	uint32_t entityIndex;
	struct vector3_t delta;		/** Vector from the current entity to the neighbor */
	float distance;				/** Distance between the current entity to the neighbor */
	float coeff;				/** Coefficient applied for velocity calculation */
	struct entity_t * curEntity = (struct entity_t *) arg;
	uint32_t index;

	for (index = 0; index < entityPerThread; ++index) {
		/** Load into CPU registers */
		curEntityPosX = curEntity->pos->x;
		curEntityPosY = curEntity->pos->y;
		curEntityPosZ = curEntity->pos->z;

		/** Reset new velocity */
		(*curEntity->newVelocity).x = 0;
		(*curEntity->newVelocity).y = 0;
		(*curEntity->newVelocity).z = 0;

		for (entityIndex = 0; entityIndex < universe.entityCount; ++entityIndex) {
			entity = &universe.entities[entityIndex];

			/** Ignore the current entity */
			if (entity == curEntity) {
				continue;
			}

			/** Compute the slope vector delta */
			delta.x = (*entity->pos).x - curEntityPosX;
			delta.y = (*entity->pos).y - curEntityPosY;
			delta.z = (*entity->pos).z - curEntityPosZ;

			/** Compute the distance by computing the abs of delta */
			distance = sqrtf(POW_2(delta.x) + POW_2(delta.y) + POW_2(delta.z));

			// Earn speed by commenting this
			if (distance == 0.0f) {
				// TODO Entity collisions
				//log_warn("Entity collision");
				continue;
			}

			/** Pre-Computed coeff to avoid repetition */
			coeff = (*entity->mass) / (POW_3(distance));

			/** Add the neighbor contribution to the new velocity of the current entity */
			(*curEntity->newVelocity).x += delta.x * coeff;
			(*curEntity->newVelocity).y += delta.y * coeff;
			(*curEntity->newVelocity).z += delta.z * coeff;
		}
		curEntity++; // Go to the next entity
	}
	return 0;
}

void start_make_a_move(void)
{
#ifdef CUDA_ENABLE
	cuda_start_make_a_move();
#else
	lastInteractionUpdate1 = curTime;
#endif
}

/**
 * Move all entities of the universe by connflicting their interractions
 * @param universe: pointer on the universe
 */
void make_a_move(struct universe_t *universe, float deltaInSec)
{
#ifdef CUDA_ENABLE
	/** For all entities of the universe, compute their new velocity */
	cuda_make_a_move(universe, CONST_FACTOR, si_timeFactor);
#else
#ifdef THREAD_ENABLE
	uint8_t index;

	/** Start the job on the threads */
	for (index = 0; index < NB_THREAD; ++index) {
		SignalWork(thread[index]);
	}

	/** Wait all threads to finish */
	for (index = 0; index < NB_THREAD; ++index) {
		while (ThreadIsFuncRunning(thread[index])) {}
	}


#else
	compute_new_velocity(&universe->entities[0]);
#endif
	uint32_t entityIndex;

	/** Update entities positions from their new velocity */
	for (entityIndex = 0; entityIndex < universe->entityCount; entityIndex++) {
		universe->currVelocity[entityIndex].x += universe->newVelocity[entityIndex].x * CONST_FACTOR * deltaInSec;
		universe->currVelocity[entityIndex].y += universe->newVelocity[entityIndex].y * CONST_FACTOR * deltaInSec;
		universe->currVelocity[entityIndex].z += universe->newVelocity[entityIndex].z * CONST_FACTOR * deltaInSec;

		universe->position[entityIndex].x += universe->currVelocity[entityIndex].x * deltaInSec;
		universe->position[entityIndex].y += universe->currVelocity[entityIndex].y * deltaInSec;
		universe->position[entityIndex].z += universe->currVelocity[entityIndex].z * deltaInSec;
	}
#endif
}

void update_blaster_pos(struct blaster_t * blaster, float deltaInSec)
{
	uint32_t entityIndex;

	/** Update entities positions from their new velocity */
	for (entityIndex = 0; entityIndex < BLASTER_LENGTH; entityIndex++) {
		blaster->position[entityIndex].x += blaster->currVelocity[entityIndex].x * deltaInSec;
		blaster->position[entityIndex].y += blaster->currVelocity[entityIndex].y * deltaInSec;
		blaster->position[entityIndex].z += blaster->currVelocity[entityIndex].z * deltaInSec;
	}
}

/**
 * Save the position of entity into the shift register of entityTrace
 * @param entityTrace
 */
void save_entity_trace(struct entityTrace_t * entityTrace)
{
	uint32_t index;

	for (index = ENTITY_TRACE_LENGTH - 1; index > 0; index--) {
		entityTrace->position[index] = entityTrace->position[index - 1];
	}
	entityTrace->position[0] = (*entityTrace->entity->pos);
}

/**
 * Move the last blaster entity to the camera position and give it a velocity
 * @param blaster
 * @param camPos
 * @param camRot
 */
void fire_a_blaster(struct blaster_t * blaster, const struct vector3_t camPos, const struct vector3_t camRot)
{
	struct vector3_t direction;

	if (((curTime - blaster->lastFire) / 1000000.0f) < BLASTER_MIN_PERIOD_IN_SEC) {
		return;
	}
	blaster->lastFire = curTime;

	blaster->curEntity += 2;

	if (blaster->curEntity >= BLASTER_LENGTH) {
		blaster->curEntity = 0;
	}

	blaster->position[blaster->curEntity] = camPos;
	blaster->position[blaster->curEntity + 1] = camPos;
	blaster->position[blaster->curEntity].y -= 0.5f;
	blaster->position[blaster->curEntity + 1].y -= 0.5f;

	direction.x = 0;
	direction.y = 0;
	direction.z = - NOMINAL_BLASTER_SPEED;

	rotate_landmark(&blaster->currVelocity[blaster->curEntity], direction, camRot);
	rotate_landmark(&blaster->currVelocity[blaster->curEntity + 1], direction, camRot);

	blaster->position[blaster->curEntity].x += blaster->currVelocity[blaster->curEntity].x * 0.5;
	blaster->position[blaster->curEntity].y += blaster->currVelocity[blaster->curEntity].y * 0.5;
	blaster->position[blaster->curEntity].z += blaster->currVelocity[blaster->curEntity].z * 0.5;
}

/**
 * Move entities and save the entityTrace
 */
void space_interraction_main_loop(void)
{
	uint8_t index;
	float deltaInSec;

	/** Compute the number of usec since the last update */
	deltaInSec = (curTime - lastInteractionUpdate1) / 1000000.0f;
	lastInteractionUpdate1 = curTime;

	/** Update positions according to the delta time */
	if (graphic.isUniverseMoving) {
		make_a_move(&universe, deltaInSec);
		for (index = 0; index < ENTITY_TRACE_COUNT; ++index) {
			save_entity_trace(&universe.entityTrace[index]);
		}
	}

	if (graphic.isStarWarsModeEnabled) {
		update_blaster_pos(&universe.blaster, deltaInSec);
	}

}
