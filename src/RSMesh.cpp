#include "RenderScene.h"
#include <iostream>

void AIMesh::Draw()
{
	static bool firstDraw = true;
	if(firstDraw)
	{
		std::cerr << "Drawing Mesh  " << m_vertexObjects[0].vertexBufferObject << " / " << m_vertexObjects[0].elementBufferObject << "\n";
		//std::cerr << "\tIndex Count  " <<  m_vertexObjects[0].elementCount << "\n";
		firstDraw = false;
	}

	for(unsigned int i = 0; i < m_vertexObjects.size(); i++)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexObjects[i].elementBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER		, m_vertexObjects[i].vertexBufferObject);

		glEnableVertexAttribArray(0);  // Vertex position.
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)0);

		glEnableVertexAttribArray(1);  // Vertex Normal.
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)16);

		glEnableVertexAttribArray(2);  // Vertex UV.
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)32);

		glEnableVertexAttribArray(2);  // Vertex UV.
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)40);

		// Draw the triangles !
		glDrawElements	( GL_TRIANGLES,      			// mode
						  m_vertexObjects[i].elementCount, // count
						  GL_UNSIGNED_INT,   			// type
						  (void*)0           			// element array buffer offset
						);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void AIMesh::Initialise(std::string& _path)
{
	(void)_path;

	std::cerr << " Loading model: " << _path << "\n";

	const aiScene* scene = (aiImportFile( _path.c_str(),		aiProcessPreset_TargetRealtime_MaxQuality
															| 	aiProcess_JoinIdenticalVertices 
															| 	aiProcess_Triangulate 
															| 	aiProcess_GenNormals
															| 	aiProcess_CalcTangentSpace 
										)
							);
	if(scene)
	{
		//std::cerr << "Model Load Success\n";
	}
	else
	{
		std::cerr << "Model Load FAILED\n";
		std::cerr << aiGetErrorString() << "\n";
	}

	if(scene->HasMeshes())
	{
		//std::cerr << scene->mNumMeshes << " mesh(es) loaded.\n";
	}
	else
	{
		std::cerr << "No Mesh loaded.\n";
	}

	//For each mesh
	for(unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* currMesh = scene->mMeshes[i];
		aiVector3D* vertexPos = currMesh->mVertices;
		aiVector3D* vertexNor = currMesh->mNormals;
		aiVector3D* vertexUV = 0;

		if(currMesh->HasTextureCoords(0))
		{
			//std::cerr << "mesh has UV coordinates\n";
			vertexUV = currMesh->mTextureCoords[0];
		}
		else
		{
			//std::cerr << "mesh does not have UV coordinates\n";
		}

		aiFace* faces = currMesh->mFaces;

		std::vector<GenericVertex> vecVertex;
		std::vector<unsigned int>  vecIndices;

		//std::cerr << "Number of faces: " <<  currMesh->mNumFaces << "\n";
		//std::cerr << "Vert Count: " << currMesh->mNumVertices 	<< "\n";
		//std::cerr << "Idx  Count: " << faces->mNumIndices * currMesh->mNumFaces	<< "\n";

		//Process Vertices
		for(unsigned int vi = 0; vi < currMesh->mNumVertices; vi++)
		{
			Position vP(vertexPos[vi][0], vertexPos[vi][1], vertexPos[vi][2]);
			Position vN(vertexNor[vi][0], vertexNor[vi][1], vertexNor[vi][2]);

			//TODO: Add UV Support.

			texCoord vT;
			if(currMesh->HasTextureCoords(0))
				vT = texCoord(vertexUV [vi][0], vertexUV[vi][1]);

			GenericVertex gv;
			gv.m_pos = vP;
			gv.m_nor = vN;
			gv.m_uv = texCoord(vT.u, vT.v);//vT;
			vecVertex.push_back(gv);
		}

		//Process Indices
		for(unsigned int ii = 0; ii < currMesh->mNumFaces; ii++)
		{
			unsigned int i0 = faces[ii].mIndices[0];
			unsigned int i1 = faces[ii].mIndices[1];
			unsigned int i2 = faces[ii].mIndices[2];

			vecIndices.push_back(i0);
			vecIndices.push_back(i1);
			vecIndices.push_back(i2);
		}

		/*
		for(unsigned int ii = 0; ii < vecIndices.size(); ii++)
		{
			std::cerr << "\tIndex: " << vecIndices[ii] << "\n";
		}

		for(unsigned int ii = 0; ii < vecVertex.size(); ii++)
		{
			std::cerr << "Pos: " << vecVertex[ii].m_pos.pos[0] 
								 << "," << vecVertex[ii].m_pos.pos[1] 
								 << "," << vecVertex[ii].m_pos.pos[2] << "\n";			
		}

		std::cerr << "Index Count: " << vecIndices.size() << "\n";
		std::cerr << "Vertex Count: " << vecVertex.size() << "\n";
		*/
		
		//Build VBOIBO
		GLuint elementBufferObject = -1;
		glGenBuffers(1, &elementBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(unsigned int), &vecIndices[0], GL_STATIC_DRAW);
		//std::cerr << "\tCreated index buffer object at id: " << elementBufferObject << "\n";

		//Vertex Buffer Objects
		GLuint vertexBufferObject = -1;
		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GenericVertex)*vecVertex.size(), &vecVertex[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);  // Vertex position.
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)0);

		glEnableVertexAttribArray(1);  // Vertex Normal.
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)16);

		glEnableVertexAttribArray(2);  // Vertex UV.
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)32);

		glEnableVertexAttribArray(2);  // Vertex UV.
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GenericVertex), (void*)40);

		//std::cerr << "\tCreated vertex buffer object at id: " << vertexBufferObject << "\n";

		//Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_vertexObjects.push_back( VBOIBO(  vertexBufferObject, elementBufferObject, vecIndices.size() ) );
	}
	
	if(scene)
		aiReleaseImport(scene);

}

void QuadMesh::Draw()
{
	static bool firstDraw = true;
	if(firstDraw)
	{
		std::cerr << "Drawing QUAD\n";
		firstDraw = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	
	glEnableVertexAttribArray(0);  // Vertex position.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)0);

	glEnableVertexAttribArray(1);  // Vertex color.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)16);

	glEnableVertexAttribArray(2);  // Vertex size.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)32);

	glEnableVertexAttribArray(3);  // Vertex uv.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)40);

	glDrawArrays( GL_TRIANGLE_FAN, 0, 4);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadMesh::Initialise()
{
	//std::cerr << "Initialising QuadMesh\n";

	//std::cerr << "\tInitialising vertex data...\n";

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

	//std::cerr << "\tCreated vertex buffer object at id: " << vertexBufferObject << "\n";

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

