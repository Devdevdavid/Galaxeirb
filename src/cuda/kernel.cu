#include "kernel.cuh"

uint64_t lastInteractionUpdate; 		/**< Date of the last interraction update */
struct vector3_t * cu_position;			// Float array of entities positions [x0,y0,z0][x1,y1,z1]...
struct vector3_t * cu_currVelocity;		// Float array of current entities velocities [vx0,vy0,vz0][vx1,vy1,vz1]...
struct vector3_t * cu_newVelocity;		// Float array of new entities velocities [vx0,vy0,vz0][vx1,vy1,vz1]...
float * cu_mass;						// Float array 

/**
 * Each thread of each block has recieve an entity 
 * and compute its new velocity by comparing it to all others entities
 * @param cu_position: vector of all the positions of the entity of the universe
 * @param cu_newVelocity: new velocity computed by each thread
 * @param cu_mass: vector of all masses of the entities of the universe
 * @param entityCount: number of entity in the universe (and size of the above vectors)
 */
__global__ void kernel_compute_new_velocity(struct vector3_t * cu_position, struct vector3_t * cu_newVelocity, float * cu_mass, uint32_t entityCount)
{
	uint32_t curEntity = fmaf(blockIdx.x, blockDim.x, threadIdx.x);	
	uint32_t entityIndex;
	struct vector3_t delta;		/** Vector from the current entity to the neighbor */
	float distance;				/** Distance between the current entity to the neighbor */
	float coeff;				/** Coefficient applied for velocity calculation */
	float curX, curY, curZ;	

	if (curEntity >= entityCount) {
		return;
	}

	/** Reset the velocity */
	(cu_newVelocity[curEntity]).x = 0;
	(cu_newVelocity[curEntity]).y = 0;
	(cu_newVelocity[curEntity]).z = 0;

	curX = (cu_position[curEntity]).x;
	curY = (cu_position[curEntity]).y;
	curZ = (cu_position[curEntity]).z;

	for (entityIndex = 0; entityIndex < entityCount; entityIndex++) {

		/** Ignore the current entity */
		if (entityIndex == curEntity) {
			continue;
		}

		/** Compute the slope vector delta */
		delta.x = (cu_position[entityIndex]).x - curX;
		delta.y = (cu_position[entityIndex]).y - curY;
		delta.z = (cu_position[entityIndex]).z - curZ;

		/** Compute the distance by computing the abs of delta */
		distance = sqrtf(POW_2(delta.x) + POW_2(delta.y) + POW_2(delta.z));

		if (distance == 0) {
			// TODO Entity collisions
			continue;		
		}

		/** Pre-Computed coeff to avoid repetition */
		coeff = cu_mass[entityIndex] / (POW_3(distance));

		/** Add the neighboor contribution to the new velocity of the current entity */
		(cu_newVelocity[curEntity]).x = fmaf(delta.x, coeff, (cu_newVelocity[curEntity]).x);
		(cu_newVelocity[curEntity]).y = fmaf(delta.y, coeff, (cu_newVelocity[curEntity]).y);
		(cu_newVelocity[curEntity]).z = fmaf(delta.z, coeff, (cu_newVelocity[curEntity]).z);
	}
}

__global__ void kernel_apply_new_velocity(struct vector3_t * cu_position, 
		struct vector3_t * cu_currVelocity, struct vector3_t * cu_newVelocity, 
		float constFactor, float si_timeFactor, int entityCount) 
{
	uint32_t curEntity = fmaf(blockIdx.x, blockDim.x, threadIdx.x);
	float curVelX, curVelY, curVelZ;

	// Avoid memory overrun
	if (curEntity >= entityCount) {
		return;
	}

	// Save values in local memory
	curVelX = (cu_currVelocity[curEntity]).x;
	curVelY = (cu_currVelocity[curEntity]).y;
	curVelZ = (cu_currVelocity[curEntity]).z;

	// Update current velocity
	cu_currVelocity[curEntity].x = fmaf(cu_newVelocity[curEntity].x, constFactor, curVelX);
	cu_currVelocity[curEntity].y = fmaf(cu_newVelocity[curEntity].y, constFactor, curVelY);
	cu_currVelocity[curEntity].z = fmaf(cu_newVelocity[curEntity].z, constFactor, curVelZ);

	// Update position
	cu_position[curEntity].x = fmaf(si_timeFactor, curVelX, cu_position[curEntity].x);
	cu_position[curEntity].y = fmaf(si_timeFactor, curVelY, cu_position[curEntity].y);
	cu_position[curEntity].z = fmaf(si_timeFactor, curVelZ, cu_position[curEntity].z);

}

/**
 *	Allocate memory on GPU and copy init value
 */
uint32_t cuda_allocate(struct universe_t *universe) 
{	
	cuda_lib_malloc((void**) &cu_position, universe->entityCount * sizeof(struct vector3_t));
	cuda_lib_malloc((void**) &cu_currVelocity, universe->entityCount * sizeof(struct vector3_t));
	cuda_lib_malloc((void**) &cu_newVelocity, universe->entityCount * sizeof(struct vector3_t));
	cuda_lib_malloc((void**) &cu_mass, universe->entityCount * sizeof(float));

	cuda_lib_memcpy(cu_position, universe->position, universe->entityCount * sizeof(struct vector3_t), cudaMemcpyHostToDevice);
	cuda_lib_memcpy(cu_currVelocity ,universe->currVelocity, universe->entityCount * sizeof(struct vector3_t), cudaMemcpyHostToDevice);
	cuda_lib_memcpy(cu_mass, universe->mass, universe->entityCount * sizeof(float), cudaMemcpyHostToDevice);

	return 0;
}

/**
 * Init the lastInteractionUpdate variable to avoid jump on start 
 */
void cuda_start_make_a_move(void)
{
	lastInteractionUpdate = getTimeOfDayInUs();
}

/**
 * Compute the next move of all entities
 */
void cuda_make_a_move(struct universe_t *universe, float constFactor, float si_timeFactor) 
{
	int nbBlock = 1;
	int nbThreadPerBlock = 1023;
	float deltaInSec;

	/** Compute new velocities on all threads of all blocks */
	kernel_compute_new_velocity<<<nbBlock, nbThreadPerBlock>>>(cu_position, cu_newVelocity, cu_mass, universe->entityCount);

	/** Compute the time between two apply and update the constant factor */
	deltaInSec = (getTimeOfDayInUs() - lastInteractionUpdate) / 1000000.0f;
	lastInteractionUpdate = getTimeOfDayInUs();
	constFactor *= deltaInSec;
	si_timeFactor *= deltaInSec;

	/** Apply new velocity */
	kernel_apply_new_velocity<<<nbBlock, nbThreadPerBlock>>>(cu_position, cu_currVelocity, cu_newVelocity, constFactor, si_timeFactor, universe->entityCount);

	/** Copy results from device to host */ 
	cuda_lib_memcpy(universe->position, cu_position, universe->entityCount * sizeof(struct vector3_t), cudaMemcpyDeviceToHost);
}

/**
 * Free all memory previously allocated
 */
void cuda_free(void) 
{
	cuda_lib_free((void *)cu_position);
	cuda_lib_free((void *)cu_currVelocity);
	cuda_lib_free((void *)cu_newVelocity);
	cuda_lib_free((void *)cu_mass);
}
