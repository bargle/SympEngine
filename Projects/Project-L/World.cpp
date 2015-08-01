#include "system_core.h"
#include "performance.h"
#include "platform.h"
#include "VideoInterface3D.h"
#include "PhysicsManager.h"
#include "GameObjectManager.h"
#include "performance.h"

#include "Interfaces.h"

#include "World.h"
#include "Chunk.h"

namespace {
	Prim_t primGround;
	PhysObject phys_ground;
	Prim_t primSky;
	PhysObject phys_sky;
	PhysContainer g_PhysContainer;

}

World::World(){
}

World::~World(){
	free(primGround.verts);
	free(primSky.verts);

	ChunkList::iterator iterChunk = m_Chunks.begin();
	while( iterChunk != m_Chunks.end() ){
		delete (*iterChunk);
		++iterChunk;
	}

}

void World::Init() {

	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if( !pVideoInterface3D ) {
		//ERROR!
		printf("World::Init failed!\n");
		assert(0);
		return;
	}
	
		//Init prims
	//////////////////////////////////////////////////////////////////////////////////////

	
	//ground prim
	HTEXTURE hTexGrass = pVideoInterface3D->CreateTexture("grass");
	primGround.vPos.Set(0.0f, -10.0f, 0.0f);
	primGround.hTex = hTexGrass;
	primGround.count = 4;
	primGround.verts = (Vert3d_t*)malloc(sizeof(Vert3d_t) * 4);
	
	float extent = 1024.0f;

	float uvScale = 128.0f;
	primGround.verts[3].vPos.Set(-extent, 0.0f, extent);
	primGround.verts[3].vTexCoords.Set(uvScale, 0.0f);
	primGround.verts[3].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primGround.verts[2].vPos.Set(extent, 0.0f, extent);
	primGround.verts[2].vTexCoords.Set(uvScale, uvScale);
	primGround.verts[2].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primGround.verts[1].vPos.Set(extent, 0.0f, -extent);
	primGround.verts[1].vTexCoords.Set(0.0f, uvScale);
	primGround.verts[1].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primGround.verts[0].vPos.Set(-extent, 0.0f, -extent);
	primGround.verts[0].vTexCoords.Set(0.0f, 0.0f);
	primGround.verts[0].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primGround.Bake();
	phys_ground.center.Set(0.0f, -12.0f, 0.0f);
	phys_ground.halfwidths.Set(extent, 2.0f, extent);

	g_PhysContainer.AddObject( &phys_ground );

	
	//SKY PRIM
	HTEXTURE hSkyTex = pVideoInterface3D->CreateTexture("sky");
	primSky.vPos.Set(0.0f, 512.0f, 0.0f);
	primSky.hTex = hSkyTex;
	primSky.count = 4;
	primSky.verts = (Vert3d_t*)malloc(sizeof(Vert3d_t) * 4);
	
	primSky.verts[0].vPos.Set(-1024.0f, 0.0f, 1024.0f);
	primSky.verts[0].vTexCoords.Set(0.0f, 0.0f);
	primSky.verts[0].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primSky.verts[1].vPos.Set(1024.0f, 0.0f, 1024.0f);
	primSky.verts[1].vTexCoords.Set(1.0f, 0.0f);
	primSky.verts[1].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primSky.verts[2].vPos.Set(1024.0f, 0.0f, -1024.0f);
	primSky.verts[2].vTexCoords.Set(1.0f, 1.0f);
	primSky.verts[2].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primSky.verts[3].vPos.Set(-1024.0f, 0.0f, -1024.0f);
	primSky.verts[3].vTexCoords.Set(0.0f, 1.0f);
	primSky.verts[3].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	primSky.Bake();

	phys_sky.center.Set(0.0f, 512.0f, 0.0f);
	phys_sky.halfwidths.Set(1024.0f, 16.0f, 1024.0f);

	g_PhysContainer.AddObject( &phys_sky );

	g_PhysContainer.center.Set(0.0f,0.0f,0.0f);
	g_PhysContainer.halfwidths.Set(1024.0f,1024.0f,1024.0f);
	PhysicsManager::GetInstance().AddContainer( &g_PhysContainer );


	#if 1
	CPerformanceCounter counter;
	counter.Start();
	for( int y = 0; y < 4; ++y ) {
		for( int x = 0; x < 4; ++x ) {
			Chunk* pChunk = new Chunk;
			Vector3d chunkPos;
			chunkPos.Set( ( 64.0f * x ), -10.0f , ( 64.0f * y ) );
            //chunkPos.Set(128.0f + (64.0f * x), -10.0f , 128.0f + (64.0f * y) );

			pChunk->SetPos(chunkPos);
			pChunk->Init();
			m_Chunks.push_back(pChunk);
		}
	}
	counter.Stop();
	printf("Chunks created in %f seconds.\n", counter.TimeInMilliseconds());
	
	#else
	
	{
			Chunk* pChunk = new Chunk;
			Vector3d chunkPos;
			chunkPos.Set(-128.0f + (-64.0f * 1), -10.0f , -128.0f + (-64.0f * 1) );
			pChunk->SetPos(chunkPos);
			pChunk->Init();
			m_Chunks.push_back(pChunk);
	}

	#endif
	
}

void World::OnRender( float elapsed ) {
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(!pVideoInterface3D) {
		return;
	}

	//Render ground and sky
	pVideoInterface3D->RenderPrim3DVBO(&primGround);
	pVideoInterface3D->RenderPrim3DVBO(&primSky);

	ChunkList::iterator iterChunk = m_Chunks.begin();
	while( iterChunk != m_Chunks.end() ){
		(*iterChunk)->OnRender( elapsed );
		++iterChunk;
	}
}

int	World::GetNumFaces() {
	int nFaces = 0;

	ChunkList::iterator iterChunk = m_Chunks.begin();
	while( iterChunk != m_Chunks.end() ){
		nFaces += (*iterChunk)->GetNumFaces();
		++iterChunk;
	}

	return nFaces;
}

