#ifndef _GAME_OBJECT_MANAGER_H_
#define _GAME_OBJECT_MANAGER_H_

#include <list>
#include <algorithm>

class Prim_t;
class PhysObject;

class GameObject {
public:
	GameObject():renderable(NULL),phys(NULL),m_Velocity( 0.0f, 0.0f, 0.0f ){}
	virtual ~GameObject(){}
	virtual void Update( float elapsed ){}

	virtual void SetPos(Vector3d& _pos) { m_Pos.Set( _pos ); }
	Vector3d& GetPos() { return m_Pos; }
	Vector3d& GetVelocity(){ return m_Velocity; }

	void GetHalfWidths( Vector3d &half ){ if(phys) { half.Set(phys->halfwidths); } }

	Prim_t* renderable;
	PhysObject* phys;
	Vector3d m_Velocity;
	Vector3d m_Pos;
};

class GameObjectManager {

public:
	GameObjectManager() {
	}

	~GameObjectManager() {
	}

	static GameObjectManager& GetInstance() {
		static GameObjectManager s_GameObjectManager;
		return s_GameObjectManager;
	}

	void AddObject( GameObject* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}
		//TODO: make sure we're not already in the list...

		game_objects.push_back( obj );
	}

	void RemoveObject( GameObject* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}

		GameObjectList::iterator iter = std::find(game_objects.begin(), game_objects.end(), obj);
		if ( iter != game_objects.end() ) {
			game_objects.erase( iter );
		}
	}


private:
	typedef std::list<GameObject*> GameObjectList;
	GameObjectList game_objects;
};

#endif

