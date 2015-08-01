#include "system_core.h"
#include "performance.h"
#include "platform.h"
#include "VideoInterface3D.h"
#include "PhysicsManager.h"
#include "GameObjectManager.h"
#include "performance.h"

#include "Interfaces.h"

#include "Chunk.h"

#include <memory.h>

#define CHUNK_OPTIMIZATIONS 1

namespace {
	#if 1
	const int g_BoxWidth = 8;
	const int g_BoxDepth = 8;
	const int g_BoxHeight = 8;
	#else
	const int g_BoxWidth = 1;
	const int g_BoxDepth = 1;
	const int g_BoxHeight = 1;
	#endif
	const int g_BoxDims = g_BoxWidth * g_BoxDepth * g_BoxHeight;
}

static void SetupVert(Vert3dBaked_t* vert, float x, float y, float z, float s, float t, int color = 0) {
		vert->pos[0] = x;
		vert->pos[1] = y;
		vert->pos[2] = z;

		vert->tex[0] = s;
		vert->tex[1] = t;

		switch( color ) {
			case 1: {
				vert->color[0] = 255;
				vert->color[1] = 0;
				vert->color[2] = 0;
				vert->color[3] = 255;
			} break;
			default:{
				vert->color[0] = 255;
				vert->color[1] = 255;
				vert->color[2] = 255;
				vert->color[3] = 255;
			}break;
		}
}

static void SetupFace(Vert3dBaked_t* vert, unsigned short* idxs, ChunkFace& face) {
	face.verts[0].Set(vert[0].pos[0], vert[0].pos[1], vert[0].pos[2]);
	face.verts[1].Set(vert[1].pos[0], vert[1].pos[1], vert[1].pos[2]);
	face.verts[2].Set(vert[2].pos[0], vert[2].pos[1], vert[2].pos[2]);
	face.verts[3].Set(vert[3].pos[0], vert[3].pos[1], vert[3].pos[2]);

	memcpy(face.vertsBaked, vert, sizeof(Vert3dBaked_t) * 4);
	memcpy(face.idxsBaked, idxs, sizeof(unsigned short) * 6);

}

//	Prim_t m_Prim;
ChunkPrim::ChunkPrim() : m_nBoxes(0){
}
ChunkPrim::~ChunkPrim() {
	//m_
}
void ChunkPrim::Init() {
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(!pVideoInterface3D) {
		return;
	}

	HTEXTURE hTex = pVideoInterface3D->CreateTexture( "test" );

	int nFaces = 6;

	m_Prim.hTex = hTex;

	m_Prim.numVertsBaked = 0;//4 * nFaces;
	m_Prim.vertsBaked = new Vert3dBaked_t[ (4*nFaces) * g_BoxDims];

	m_Prim.numIdxBaked = 0;//6 * nFaces;
	m_Prim.idxBaked = new unsigned short[ (6*nFaces) * g_BoxDims];
}

void ChunkPrim::OnRender( float elapsed ) {
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(!pVideoInterface3D) {
		return;
	}

	if (pVideoInterface3D->RenderPrim3DVBO( &m_Prim )  != RESULT_OK ) {
		printf("Failed to render chunk\n");
	}
}

void ChunkPrim::PrintFaces() {
	FaceVector::iterator iter = m_Faces.begin();

	for( int i = 0; iter != m_Faces.end(); ++i ){
		//are these equal? if so, remove them both and restart the iterator..
		//or do we need to? maybe just exit.
		/*
		if( *iter == face ) {
			m_Faces.erase(iter);
			printf("found!\n");
			return;
		}
		*/
		
		printf("[%02d] ", i);
		(*iter).verts[0].Print();
		(*iter).verts[1].Print();
		(*iter).verts[2].Print();
		(*iter).verts[3].Print();
		printf("\n");

		++iter;
	}
}

int ChunkPrim::GetNumFaces() {
	return (int)m_Faces.size();
}

void ChunkPrim::AddBox(Vector3d &pos) {
	int nFaces = 6;
	float scale = 4.0f;
	m_Prim.numVertsBaked += 4 * nFaces;
	m_Prim.numIdxBaked += 6 * nFaces;
	int vert_offset = m_nBoxes * 24;
	int index_offset = m_nBoxes * 36;

	SetupVert(&m_Prim.vertsBaked[vert_offset + 0], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 1], scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 2], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 3], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset] );

	m_Prim.idxBaked[index_offset + 0] = vert_offset + 3;
	m_Prim.idxBaked[index_offset + 1] = vert_offset + 1;
	m_Prim.idxBaked[index_offset + 2] = vert_offset + 0;
	m_Prim.idxBaked[index_offset + 3] = vert_offset + 3;
	m_Prim.idxBaked[index_offset + 4] = vert_offset + 2;
	m_Prim.idxBaked[index_offset + 5] = vert_offset + 1;

	//left
	SetupVert(&m_Prim.vertsBaked[vert_offset + 4], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 5], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 6], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 7], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 1.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset + 4] );

	m_Prim.idxBaked[index_offset + 6] = vert_offset + 4;
	m_Prim.idxBaked[index_offset + 7] = vert_offset + 5;
	m_Prim.idxBaked[index_offset + 8] = vert_offset + 7;
	m_Prim.idxBaked[index_offset + 9] = vert_offset + 5;
	m_Prim.idxBaked[index_offset + 10] = vert_offset + 6;
	m_Prim.idxBaked[index_offset + 11] = vert_offset + 7;

	//back
	SetupVert(&m_Prim.vertsBaked[vert_offset + 8], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 9], scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 10], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 11], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset + 8] );

	m_Prim.idxBaked[index_offset + 12] = vert_offset + 8;
	m_Prim.idxBaked[index_offset + 13] = vert_offset + 9;
	m_Prim.idxBaked[index_offset + 14] = vert_offset + 11;
	m_Prim.idxBaked[index_offset + 15] = vert_offset + 9;
	m_Prim.idxBaked[index_offset + 16] = vert_offset + 10;
	m_Prim.idxBaked[index_offset + 17] = vert_offset + 11;

	//right
	SetupVert(&m_Prim.vertsBaked[vert_offset + 12], scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 13], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 14], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 15], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset + 12] );

	m_Prim.idxBaked[index_offset + 18] = vert_offset + 12;
	m_Prim.idxBaked[index_offset + 19] = vert_offset + 13;
	m_Prim.idxBaked[index_offset + 20] = vert_offset + 15;
	m_Prim.idxBaked[index_offset + 21] = vert_offset + 13;
	m_Prim.idxBaked[index_offset + 22] = vert_offset + 14;
	m_Prim.idxBaked[index_offset + 23] = vert_offset + 15;

	//front
	SetupVert(&m_Prim.vertsBaked[vert_offset + 16], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 17], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 18], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 19], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset + 16] );

	m_Prim.idxBaked[index_offset + 24] = vert_offset + 16;
	m_Prim.idxBaked[index_offset + 25] = vert_offset + 17;
	m_Prim.idxBaked[index_offset + 26] = vert_offset + 19;
	m_Prim.idxBaked[index_offset + 27] = vert_offset + 17;
	m_Prim.idxBaked[index_offset + 28] = vert_offset + 18;
	m_Prim.idxBaked[index_offset + 29] = vert_offset + 19;

	SetupVert(&m_Prim.vertsBaked[vert_offset + 20], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 21], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 22], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);
	SetupVert(&m_Prim.vertsBaked[vert_offset + 23], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);

	//AddFace( &m_Prim.vertsBaked[vert_offset + 20] );

	m_Prim.idxBaked[index_offset + 30] = vert_offset + 20;
	m_Prim.idxBaked[index_offset + 31] = vert_offset + 23;
	m_Prim.idxBaked[index_offset + 32] = vert_offset + 21;
	m_Prim.idxBaked[index_offset + 33] = vert_offset + 21;
	m_Prim.idxBaked[index_offset + 34] = vert_offset + 23;
	m_Prim.idxBaked[index_offset + 35] = vert_offset + 22;

	++m_nBoxes;
}

void ChunkPrim::AddBoxOpt(Vector3d &pos) {
	int nFaces = 6;
	float scale = 4.0f;
	//m_Prim.numVertsBaked += 4 * nFaces;
	//m_Prim.numIdxBaked += 6 * nFaces;
	//int vert_offset = m_nBoxes * 24;
	//int index_offset = m_nBoxes * 36;
	
	Vert3dBaked_t vertsBaked[4];
	unsigned short idxBaked[6];

	SetupVert(&vertsBaked[0], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&vertsBaked[1], scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 0.0f);
	SetupVert(&vertsBaked[2], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[3], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);

	idxBaked[0] = 3;
	idxBaked[1] = 1;
	idxBaked[2] = 0;
	idxBaked[3] = 3;
	idxBaked[4] = 2;
	idxBaked[5] = 1;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	//left
	SetupVert(&vertsBaked[0], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&vertsBaked[1], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 0.0f);
	SetupVert(&vertsBaked[2], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[3], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 1.0f);

	idxBaked[0] = 0;
	idxBaked[1] = 1;
	idxBaked[2] = 3;
	idxBaked[3] = 1;
	idxBaked[4] = 2;
	idxBaked[5] = 3;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	//back
	SetupVert(&vertsBaked[0], -scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[1], scale + pos.x, 0.0f + pos.y, scale + pos.z, 0.0f, 1.0f);
	SetupVert(&vertsBaked[2], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);
	SetupVert(&vertsBaked[3], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);

	idxBaked[0] = 0;
	idxBaked[1] = 1;
	idxBaked[2] = 3;
	idxBaked[3] = 1;
	idxBaked[4] = 2;
	idxBaked[5] = 3;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	//right
	SetupVert(&vertsBaked[0], scale + pos.x, 0.0f + pos.y, scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[1], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);
	SetupVert(&vertsBaked[2], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 0.0f);
	SetupVert(&vertsBaked[3], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);

	idxBaked[0] = 0;
	idxBaked[1] = 1;
	idxBaked[2] = 3;
	idxBaked[3] = 1;
	idxBaked[4] = 2;
	idxBaked[5] = 3;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	//front
	SetupVert(&vertsBaked[0], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 0.0f);
	SetupVert(&vertsBaked[1], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 0.0f);
	SetupVert(&vertsBaked[2], scale + pos.x, 0.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[3], -scale + pos.x, 0.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);

	idxBaked[0] = 0;
	idxBaked[1] = 1;
	idxBaked[2] = 3;
	idxBaked[3] = 1;
	idxBaked[4] = 2;
	idxBaked[5] = 3;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	SetupVert(&vertsBaked[0], -scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 0.0f, 1.0f);
	SetupVert(&vertsBaked[1], scale + pos.x, scale * 2.0f + pos.y, -scale + pos.z, 1.0f, 1.0f);
	SetupVert(&vertsBaked[2], scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 1.0f, 0.0f);
	SetupVert(&vertsBaked[3], -scale + pos.x, scale * 2.0f + pos.y, scale + pos.z, 0.0f, 0.0f);

	idxBaked[0] = 0;
	idxBaked[1] = 3;
	idxBaked[2] = 1;
	idxBaked[3] = 1;
	idxBaked[4] = 3;
	idxBaked[5] = 2;

	CacheFace( (Vert3dBaked_t*)&vertsBaked, (unsigned short *)idxBaked );

	++m_nBoxes;
}

void ChunkPrim::CacheFace(Vert3dBaked_t *verts, unsigned short *idxs) {
	ChunkFace face;
	SetupFace(verts, idxs, face);

	FaceVector::iterator iter = m_Faces.begin();

	while( iter != m_Faces.end() ){
		//are these equal? if so, remove them both and restart the iterator..
		//or do we need to? maybe just exit.
		if( *iter == face ) {
			m_Faces.erase(iter);
			//printf("found!\n");
			return;
		}

		++iter;
	}
	
	m_Faces.push_back( face );
	//printf("faces %d\n", (int)m_Faces.size());

}

void ChunkPrim::AddFaces() {
	//m_Prim.numVertsBaked
	//m_Prim.numIdxBaked
	
	//m_Prim.vertsBaked
	//m_Prim.idxBaked
	int nVerts = 0;
	int nIdxs = 0;
	
	//printf("Adding %d faces.\n", m_Faces.size());
	
	FaceVector::iterator iter = m_Faces.begin();

	for ( int i = 0;  iter != m_Faces.end() ; ++i ){
		//(*iter)
		//printf("face %d\n", i);
		
		memcpy(&m_Prim.vertsBaked[nVerts], (*iter).vertsBaked, sizeof(Vert3dBaked_t) * 4);
		//m_Prim.numVertsBaked += 4;
		
		//memcpy(&m_Prim.idxBaked[nIdxs], (*iter).idxsBaked, sizeof(unsigned short) * 6);
		
		m_Prim.idxBaked[nIdxs + 0] = nVerts + (*iter).idxsBaked[0];
		m_Prim.idxBaked[nIdxs + 1] = nVerts + (*iter).idxsBaked[1];
		m_Prim.idxBaked[nIdxs + 2] = nVerts + (*iter).idxsBaked[2];
		m_Prim.idxBaked[nIdxs + 3] = nVerts + (*iter).idxsBaked[3];
		m_Prim.idxBaked[nIdxs + 4] = nVerts + (*iter).idxsBaked[4];
		m_Prim.idxBaked[nIdxs + 5] = nVerts + (*iter).idxsBaked[5];
		
		//m_Prim.numIdxBaked += 6;
		nVerts += 4;
		nIdxs += 6;
		
		++iter;
	}
	
	m_Prim.numVertsBaked = nVerts;
	m_Prim.numIdxBaked = nIdxs;
	
}

void ChunkPrim::Bake() {
	m_Prim.FastBake();
}



Chunk::Chunk() : m_GameObjects(NULL), m_pBoxGrid(NULL) {
}

Chunk::~Chunk() {
	for (int i = 0; i < (g_BoxDepth * g_BoxWidth * g_BoxHeight); ++i){
		//delete m_pBoxGrid[ i ];
		delete m_GameObjects[ i ].phys;
	}

	delete [] m_pBoxGrid;
	delete [] m_GameObjects;
}

void Chunk::Init() {
	int width = g_BoxWidth;
	int depth = g_BoxDepth;
	int height = g_BoxHeight;

	int nCount = 0;

	float xLimit = 0;
	float yLimit = 0;
	float zLimit = 0;
	
	CPerformanceCounter counter;
	counter.Start();

	m_ChunkPrim.Init();

	m_pPhysContainer = new PhysContainer;
	m_GameObjects = new GameObject[g_BoxWidth * g_BoxDepth * g_BoxHeight];
	m_pBoxGrid = NULL;//new Prim_t*[g_BoxWidth * g_BoxDepth * g_BoxHeight];

	//m_pPhysContainer->halfwidths.Set(width * 4.0f + 0.01f, height * 4.0f + 0.01f, depth * 4.0f + 0.01f);
	//m_pPhysContainer->center.Set( );
	
	Vector3d transPos;
	transPos.Set( m_Pos );//-128.0f, -10.0f, -128.0f);

	for ( int h = 0; h < height; ++h ){
		for ( int i = 0; i < depth; ++i ){
			for ( int j = 0; j < width; ++j ){
				int idx = (h*(depth*width)) + (i*width) + j;
				//printf("idx: %d\n", idx);
				
				int r = rand() % 100;
				
				if( r > 65 ) {
					float x = transPos.x + (j*8);
					float y = transPos.y + (h*8);
					float z = transPos.z + (i*8);
					
					float xFabs = fabs(x);
					float yFabs = fabs(y);
					float zFabs = fabs(z);
					
					if( xFabs > xLimit) xLimit = xFabs;
					if( yFabs > yLimit) yLimit = yFabs;
					if( zFabs > zLimit) zLimit = zFabs;

					/*
					if( r > 75) {
						m_pBoxGrid[ idx ] = CreateBox( "dirt.raw", 4 );
					} else {
						m_pBoxGrid[ idx ] = CreateBox( "test.raw", 4 );
					}
					*/
					//m_pBoxGrid[ idx ] = CreateBox( "test.raw", 4 );
					
					//m_pBoxGrid[ idx ]->vPos.Set( x, y, z );
					m_GameObjects[ idx ].renderable = NULL;
					
					Vector3d boxPos;
					boxPos.Set( transPos.x * -1.0f, transPos.y * -1.0f, transPos.z * -1.0f );
					boxPos.x += x;
					boxPos.y += y;
					boxPos.z += z;
					#if CHUNK_OPTIMIZATIONS
					m_ChunkPrim.AddBoxOpt(boxPos);
					#else
					m_ChunkPrim.AddBox(boxPos);
					#endif
					
					//if(m_pBoxGrid[ idx ] == NULL) {
						//printf("Box call failed\n");
					//}
					
					//printf("Box Pos: %f %f %f\n", g_pBoxGrid[ idx ]->vPos.x, g_pBoxGrid[ idx ]->vPos.y, g_pBoxGrid[ idx ]->vPos.z);
		
					//m_GameObjects[ idx ].renderable = m_pBoxGrid[ idx ];
					m_GameObjects[ idx ].phys = new PhysObject;
		
					//center point
					m_GameObjects[ idx ].phys->center.Set(transPos.x + (j*8), (transPos.y *0.5f) + (h*8), transPos.z + (i*8));
					m_GameObjects[ idx ].phys->halfwidths.Set(4.0f,4.0f,4.0f);

					m_pPhysContainer->AddObject( m_GameObjects[ idx ].phys );
					
					nCount++;
				} else {
					m_GameObjects[ idx ].renderable = NULL;
					m_GameObjects[ idx ].phys = NULL;
					//printf("renderable set to NULL\n");
				}
	
			}
		}
	}

	m_pPhysContainer->halfwidths.Set(width * 4.0f + 0.01f, height * 4.0f + 0.01f, depth * 4.0f + 0.01f);
	m_pPhysContainer->center.Set( transPos.x + ((width-1)*4.0f) , (height * 4.0f) + transPos.y, transPos.z + ((depth-1.0f)*4.0f));
	PhysicsManager::GetInstance().AddContainer( m_pPhysContainer );

	//m_pPhysContainer->SetDebug( true );
	//Vector3d boxPos;
	//m_ChunkPrim.AddBox(boxPos);
	
	#if CHUNK_OPTIMIZATIONS
	m_ChunkPrim.AddFaces();
	#endif
	m_ChunkPrim.Bake();
	m_ChunkPrim.m_Prim.vPos.Set( transPos );

	//printf("%d boxes created\n", nCount);
	
	//m_ChunkPrim.PrintFaces();
	counter.Stop();
	
	printf("Chunk generated in %f seconds.\n", counter.TimeInMilliseconds());
}

void Chunk::OnRender( float elapsed ) {

	if( m_GameObjects == NULL ) {
		return;
	}

	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(!pVideoInterface3D) {
		return;
	}

	/*
	for ( int i = 0; i < ( g_BoxWidth * g_BoxDepth * g_BoxHeight ); ++i ) {
		if ( m_GameObjects[i].renderable ) {
			pVideoInterface3D->RenderPrim3DVBO( m_GameObjects[i].renderable );
		}
	}
	*/
	m_ChunkPrim.OnRender( elapsed );

}

Prim_t* Chunk::CreateBox( const char* tex, float scale ) {
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if ( !pVideoInterface3D) {
		return NULL;
	}

	Prim_t* pPrim = new Prim_t;
	if( pPrim ) {
		HTEXTURE hTex = pVideoInterface3D->CreateTexture( tex );

		int nFaces = 6;

		pPrim->vPos.Set(0.0f, -10.0f, 0.0f);
		pPrim->hTex = hTex;

		pPrim->numVertsBaked = 4 * nFaces;
		pPrim->vertsBaked = new Vert3dBaked_t[pPrim->numVertsBaked];

		pPrim->numIdxBaked = 6 * nFaces;
		pPrim->idxBaked = new unsigned short[pPrim->numIdxBaked];


		//set up the verts

		//bottom
		SetupVert(&pPrim->vertsBaked[0], -scale, 0.0f, scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[1], scale, 0.0f, scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[2], scale, 0.0f, -scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[3], -scale, 0.0f, -scale, 0.0f, 1.0f);

		pPrim->idxBaked[0] = 3;
		pPrim->idxBaked[1] = 1;
		pPrim->idxBaked[2] = 0;
		pPrim->idxBaked[3] = 3;
		pPrim->idxBaked[4] = 2;
		pPrim->idxBaked[5] = 1;

		//left
		SetupVert(&pPrim->vertsBaked[4], -scale, scale * 2.0f, scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[5], -scale, scale * 2.0f, -scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[6], -scale, 0.0f, -scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[7], -scale, 0.0f, scale, 0.0f, 1.0f);

		pPrim->idxBaked[6] = 7;
		pPrim->idxBaked[7] = 5;
		pPrim->idxBaked[8] = 4;
		pPrim->idxBaked[9] = 7;
		pPrim->idxBaked[10] = 6;
		pPrim->idxBaked[11] = 5;

		//back
		SetupVert(&pPrim->vertsBaked[8], scale, scale * 2.0f, scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[9], -scale, scale * 2.0f, scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[10], -scale, 0.0f, scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[11], scale, 0.0f, scale, 0.0f, 1.0f);

		pPrim->idxBaked[12] = 11;
		pPrim->idxBaked[13] = 9;
		pPrim->idxBaked[14] = 8;
		pPrim->idxBaked[15] = 11;
		pPrim->idxBaked[16] = 10;
		pPrim->idxBaked[17] = 9;

		//right
		SetupVert(&pPrim->vertsBaked[12], scale, scale * 2.0f, -scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[13], scale, scale * 2.0f, scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[14], scale, 0.0f, scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[15], scale, 0.0f, -scale, 0.0f, 1.0f);

		pPrim->idxBaked[18] = 15;
		pPrim->idxBaked[19] = 13;
		pPrim->idxBaked[20] = 12;
		pPrim->idxBaked[21] = 15;
		pPrim->idxBaked[22] = 14;
		pPrim->idxBaked[23] = 13;

		//front
		SetupVert(&pPrim->vertsBaked[16], -scale, scale * 2.0f, -scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[17], scale, scale * 2.0f, -scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[18], scale, 0.0f, -scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[19], -scale, 0.0f, -scale, 0.0f, 1.0f);

		pPrim->idxBaked[24] = 19;
		pPrim->idxBaked[25] = 17;
		pPrim->idxBaked[26] = 16;
		pPrim->idxBaked[27] = 19;
		pPrim->idxBaked[28] = 18;
		pPrim->idxBaked[29] = 17;

		SetupVert(&pPrim->vertsBaked[20], -scale, scale * 2.0f, scale, 0.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[21], scale, scale * 2.0f, scale, 1.0f, 0.0f);
		SetupVert(&pPrim->vertsBaked[22], scale, scale * 2.0f, -scale, 1.0f, 1.0f);
		SetupVert(&pPrim->vertsBaked[23], -scale, scale * 2.0f, -scale, 0.0f, 1.0f);

		pPrim->idxBaked[30] = 23;
		pPrim->idxBaked[31] = 21;
		pPrim->idxBaked[32] = 20;
		pPrim->idxBaked[33] = 23;
		pPrim->idxBaked[34] = 22;
		pPrim->idxBaked[35] = 21;

		pPrim->FastBake();

		return pPrim;
	}
	
	return NULL;
}

