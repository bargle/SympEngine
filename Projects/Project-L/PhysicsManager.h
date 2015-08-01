#ifndef _PHYSICS_MANAGER_H_
#define _PHYSICS_MANAGER_H_

#include <list>
#include <algorithm>

class PhysObject {
public:
	virtual ~PhysObject(){}

	Vector3d center;
	Vector3d halfwidths;
};

class PhysContainer : public PhysObject {
public: 
	PhysContainer() :m_bDebug(false) {}
	virtual ~PhysContainer(){}

	void AddObject( PhysObject* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}
		//TODO: make sure we're not already in the list...

		phys_objects.push_back( obj );
	}

	void RemoveObject( PhysObject* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}

		PhysicsObjectList::iterator iter = std::find(phys_objects.begin(), phys_objects.end(), obj);
		if ( iter != phys_objects.end() ) {
			phys_objects.erase( iter );
		}
	}

	int TestBox( Vector3d& center, Vector3d& halfwidths ) {
		
		if( m_bDebug ) {
			printf("Collision with container! (%f, %f, %f)\n", center.x, center.y, center.z);
		}
		
		//loop objects
		PhysicsObjectList::iterator iter = phys_objects.begin();
		while ( iter != phys_objects.end() ) {
			PhysObject* pObj = (*iter);

			if ( (fabsf(center.x - pObj->center.x) > (halfwidths.x + pObj->halfwidths.x )) ||
				 (fabsf(center.y - pObj->center.y) > (halfwidths.y + pObj->halfwidths.y )) || 
				 (fabsf(center.z - pObj->center.z) > (halfwidths.z + pObj->halfwidths.z )) 
				){
					++iter;
					continue;
				}

				//Collision detected!
				return 1;
		}

		return 0;
	}

	void SetDebug( bool bDebug ) { m_bDebug = bDebug; }

private:
	typedef std::list<PhysObject*> PhysicsObjectList;
	PhysicsObjectList phys_objects;
	bool m_bDebug;
};

class PhysicsManager {

public:
	PhysicsManager() {
	}

	~PhysicsManager() {
	}

	static PhysicsManager& GetInstance() {
		static PhysicsManager s_PhysicsManager;
		return s_PhysicsManager;
	}

	void AddContainer( PhysContainer* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}
		//TODO: make sure we're not already in the list...

		phys_objects.push_back( obj );
	}

	void RemoveContainer( PhysContainer* obj ) {
		if( obj == NULL ) {
			//TODO: assert
			return;
		}

		PhysContainerList::iterator iter = std::find(phys_objects.begin(), phys_objects.end(), obj);
		if ( iter != phys_objects.end() ) {
			phys_objects.erase( iter );
		}
	}

	int ApplyGravity( float elapsed, Vector3d& pos ) {
		if( elapsed > 1.0f ) {
			return -1;
		}

		pos.y -= 50.0f * elapsed;
		return 0;
	}

	int TestBox( Vector3d& center, Vector3d& halfwidths ) {
		//loop objects
		PhysContainerList::iterator iter = phys_objects.begin();
		while ( iter != phys_objects.end() ) {
			PhysContainer* pObj = (*iter);

			if ( (fabsf(center.x - pObj->center.x) > (halfwidths.x + pObj->halfwidths.x )) ||
				 (fabsf(center.y - pObj->center.y) > (halfwidths.y + pObj->halfwidths.y )) ||
				 (fabsf(center.z - pObj->center.z) > (halfwidths.z + pObj->halfwidths.z ))
				){
					++iter;
					continue;
				}

			//Check the container
			if ( (*iter)->TestBox( center, halfwidths ) ) {
				return 1;
			}

			++iter;
		}

		return 0;
	}

private:
	typedef std::list<PhysContainer*> PhysContainerList;
	PhysContainerList phys_objects;
};

#endif

