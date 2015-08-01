#ifndef _WORLD_H_
#define _WORLD_H_

#include <list>

class Chunk;

class World {
public:
	World();
	~World();

	void Init();
	void OnRender( float elapsed );
	int	GetNumFaces();

private:
	typedef std::list<Chunk*> ChunkList;
	ChunkList m_Chunks;

	
};

#endif
