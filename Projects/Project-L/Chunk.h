#ifndef _CHUNK_H_
#define _CHUNK_H_

#include <vector>
#include <list>

class ChunkFace {
public:
	ChunkFace(){}
	~ChunkFace(){}

	// This check may suffer from floating point precision issues. (may need to do an epsilon check instead)
	#if 1
	bool operator==(const ChunkFace &other) const {
		if ( (verts[0] != other.verts[3] ) ||
			 (verts[1] != other.verts[2] ) ||
			 (verts[2] != other.verts[1] ) ||
			 (verts[3] != other.verts[0] )
			) {
				return false;
			}
		return true;
	}
	#else
	
	bool operator==(const ChunkFace &other) const {
		if ( (verts[0] != other.verts[0] ) ||
			 (verts[1] != other.verts[1] ) ||
			 (verts[2] != other.verts[2] ) ||
			 (verts[3] != other.verts[3] )
			) {
				return false;
			}
		return true;
	}
	#endif

	Vector3d verts[4];
	Vert3dBaked_t vertsBaked[4];
	unsigned short idxsBaked[6];
};

class ChunkPrim {
public:
	ChunkPrim();
	~ChunkPrim();
	void Init();
	void OnRender( float elapsed );

	void AddBox(Vector3d &pos);
	void AddBoxOpt(Vector3d &pos);
	void CacheFace(Vert3dBaked_t *verts, unsigned short *idxs );
	void AddFaces();
	void PrintFaces();
	int GetNumFaces();

	void Bake();

	Prim_t m_Prim;
private:

	typedef std::list<ChunkFace> FaceVector;
	FaceVector m_Faces;
	int m_nBoxes;
};

class Chunk {
public:
	Chunk();
	~Chunk();
	void Init();
	void OnUpdate( float elapsed );
	void OnRender( float elapsed );

	Prim_t* CreateBox( const char* tex, float scale );
	
	void SetPos( Vector3d &pos ) { m_Pos.Set(pos); }
	int GetNumFaces() { return m_ChunkPrim.GetNumFaces(); }

private:
	GameObject	*m_GameObjects;
	Prim_t** m_pBoxGrid;
	PhysContainer *m_pPhysContainer;
	ChunkPrim m_ChunkPrim;
	Vector3d m_Pos;

};

#endif
