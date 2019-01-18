/*
 * render.c
 *
 *  Created on: Oct 8, 2018
 *      Author: David
 */

#include "render.hpp"

struct vao_t vao;
uint32_t gridElementCount = 0;

void init_render(struct universe_t * universe)
{
	uint16_t index;

	// Create all VAO and associated VBO Buffers
	glGenVertexArrays(VAO_NUMBER, vao.vaoIdTable);
	for (index = 0; index < VAO_NUMBER; ++index) {
		glGenBuffers(2, &vao.vboIdTable[2 * index]);
	}

	// Load all non dynamic VAO
	load_axes();
	load_grid(10, 10);
	load_universe(universe);
	load_entityTrace(universe);
}

void free_render(void)
{
	uint16_t index;

	for (index = 0; index < VAO_NUMBER; ++index) {
		glDeleteBuffers(2, &vao.vboIdTable[2 * index]);
	}
	glDeleteVertexArrays(VAO_NUMBER, vao.vaoIdTable);
}

void load_axes(void)
{
	float positions[] = {
			0.0, 0.0, 0.0,  1.0, 0.0, 0.0,
			0.0, 0.0, 0.0,  0.0, 1.0, 0.0,
			0.0, 0.0, 0.0,  0.0, 0.0, 1.0
	};
	float colors[] = {
			1.0, 0.0, 0.0,  1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,  0.0, 0.0, 1.0
	};

	// Bind VAO (set current)
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_AXES]);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_AXES + VBO_INDEX_POSITION]);  // Positions
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_AXES + VBO_INDEX_COLOR]);  // Color
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_COLOR);

}

void show_axes(void)
{
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_AXES]);
	glDrawArrays(GL_LINES, 0, 6); // Fixed size
}

void load_grid(uint8_t sqareCount, float width)
{
	uint32_t bufferSize;

	gridElementCount = 4 * (sqareCount + 1);
	bufferSize = gridElementCount * sizeof(struct vector3_t); // Same size for colorRGB_t

	struct vector3_t * positions = (struct vector3_t *) malloc(bufferSize);
	struct colorRGB_t * colors   = (struct colorRGB_t *) malloc(bufferSize);

	for (uint8_t index = 0; index < gridElementCount; index++) {
		colors[index].r = 0.5f;
		colors[index].g = 0.5f;
		colors[index].b = 0.5f;
	}

	for (uint8_t index = 0; index < gridElementCount / 4; index++) {
		positions[4 * index + 0].x = - width / 2;
		positions[4 * index + 0].y = 0;
		positions[4 * index + 0].z = - width / 2 + index * width / sqareCount;

		positions[4 * index + 1].x = + width / 2;
		positions[4 * index + 1].y = 0;
		positions[4 * index + 1].z = - width / 2 + index * width / sqareCount;

		positions[4 * index + 2].x = - width / 2 + index * width / sqareCount;
		positions[4 * index + 2].y = 0;
		positions[4 * index + 2].z = - width / 2;

		positions[4 * index + 3].x = - width / 2 + index * width / sqareCount;
		positions[4 * index + 3].y = 0;
		positions[4 * index + 3].z = + width / 2;
	}

	// Bind VAO (set current)
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_GRID]);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_GRID + VBO_INDEX_POSITION]);  // Positions
	glBufferData(GL_ARRAY_BUFFER, bufferSize, positions, GL_STATIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_GRID + VBO_INDEX_COLOR]);  // color
	glBufferData(GL_ARRAY_BUFFER, bufferSize, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_COLOR);

	free(positions);
	free(colors);
}

void show_grid(void)
{
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_GRID]);
	glDrawArrays(GL_LINES, 0, gridElementCount);
}

void load_universe(struct universe_t * universe)
{
	uint32_t bufferSize = universe->entityCount * sizeof(struct vector3_t);

	// Bind VAO (set current)
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_UNIVERSE]);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_UNIVERSE + VBO_INDEX_POSITION]);  // Positions
	glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_POSITION);

	if (universe->isColorNeedLoad == true) {
		glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_UNIVERSE + VBO_INDEX_COLOR]);  // color
		glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->color, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(VBO_INDEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VBO_INDEX_COLOR);
		universe->isColorNeedLoad = false;
	}
}

void show_universe(struct universe_t * universe)
{
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_UNIVERSE]);
	glDrawArrays(GL_POINTS, 0, universe->entityCount);
}

void load_entityTrace(struct universe_t * universe)
{
	uint8_t index;
	uint32_t bufferSize = ENTITY_TRACE_LENGTH * sizeof(struct vector3_t);

	for (index = 0; index < ENTITY_TRACE_COUNT; index++) {
		// Bind VAO (set current)
		glBindVertexArray(vao.vaoIdTable[VAO_INDEX_ENTITIES_TRACE + index]);

		glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * (VAO_INDEX_ENTITIES_TRACE + index) + VBO_INDEX_POSITION]);  // Positions
		glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->entityTrace[index].position, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(VBO_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VBO_INDEX_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * (VAO_INDEX_ENTITIES_TRACE + index) + VBO_INDEX_COLOR]);  // color
		glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->entityTrace[index].color, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(VBO_INDEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VBO_INDEX_COLOR);
	}
}

void show_entityTrace(void)
{
	uint8_t index;

	for (index = 0; index < ENTITY_TRACE_COUNT; index++) {
		glBindVertexArray(vao.vaoIdTable[VAO_INDEX_ENTITIES_TRACE + index]);
		glDrawArrays(GL_LINE_STRIP, 0, ENTITY_TRACE_LENGTH);
	}
}

void load_blasters(struct universe_t * universe)
{
	uint32_t bufferSize = BLASTER_LENGTH * sizeof(struct vector3_t);

	// Bind VAO (set current)
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_BLASTERS]);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_BLASTERS + VBO_INDEX_POSITION]);  // Positions
	glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->blaster.position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, vao.vboIdTable[2 * VAO_INDEX_BLASTERS + VBO_INDEX_COLOR]);  // color
	glBufferData(GL_ARRAY_BUFFER, bufferSize, universe->blaster.color, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VBO_INDEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VBO_INDEX_COLOR);
}

void show_blasters(struct universe_t * universe)
{
	glBindVertexArray(vao.vaoIdTable[VAO_INDEX_BLASTERS]);
	glDrawArrays(GL_LINES, 0, BLASTER_LENGTH);
}
