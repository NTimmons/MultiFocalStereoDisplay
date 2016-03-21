#include "RenderScene.h"
#include <iostream>


void QuadMesh::Draw()
{
	TESTGL;
	static bool firstDraw = true;
	if(firstDraw)
	{
		std::cerr << "Drawing QUAD\n";
		firstDraw = true;
	}

TESTGL;

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);



TESTGL;

	// Draw the triangles !
	/*glDrawElements	( GL_TRIANGLES,      // mode
					  6,    // count
					  GL_UNSIGNED_INT,   // type
					  (void*)0           // element array buffer offset
					);
*/
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4);
TESTGL;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
TESTGL;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
TESTGL;
}

void QuadMesh::Initialise()
{
	std::cerr << "Initialising QuadMesh\n";

	std::cerr << "\tInitialising vertex data...\n";

	vertices[0].m_pos = Position(0.f, 0.f, 0.f);
	vertices[1].m_pos = Position(0.f, 1.f, 0.f);
	vertices[2].m_pos = Position(1.f, 1.f, 0.f);
	vertices[3].m_pos = Position(1.f, 0.f, 0.f);
	
	vertices[0].m_col = colour(0.f, 0.f, 0.f, 1.f);
	vertices[1].m_col = colour(0.f, 1.f, 0.f, 1.f);
	vertices[2].m_col = colour(1.f, 1.f, 0.f, 1.f);
	vertices[3].m_col = colour(1.f, 0.f, 0.f, 1.f);

	vertices[0].m_size = size(0.f, 0.f);
	vertices[1].m_size = size(0.f, 1.f);
	vertices[2].m_size = size(1.f, 0.f);
	vertices[3].m_size = size(1.f, 1.f);

	vertices[0].m_uv = texCoord(0.f, 0.f);
	vertices[1].m_uv = texCoord(0.f, 1.f);
	vertices[2].m_uv = texCoord(1.f, 0.f);
	vertices[3].m_uv = texCoord(1.f, 1.f);
	

/*
	std::cerr << "\tInitialising index data...\n";
	indicies[0] = 0;
	indicies[1] = 1;
	indicies[2] = 3;
	indicies[3] = 0;
	indicies[4] = 3;
	indicies[5] = 1;

	std::cerr << "\tCreating index buffer object...\n";
	//Index Buffer Objects
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);
	std::cerr << "\tCreated index buffer object at id: " << elementBufferObject << "\n";*/

	//Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex)*4, &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // Vertex position.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)0);

	glEnableVertexAttribArray(1);  // Vertex color.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)16);

	glEnableVertexAttribArray(2);  // Vertex size.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)32);

	glEnableVertexAttribArray(3);  // Vertex uv.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)40);

	std::cerr << "\tCreated vertex buffer object at id: " << vertexBufferObject << "\n";

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

