#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "PhysicsManager.h"
#include "GameObjectManager.h"
#include "VideoInterface3D.h"

class Camera : public GameObject {
public:
	Camera():GameObject(){
		m_Pos.Set(0.0f, 0.0f, 0.0f);
		phys = new PhysObject;
	}
	virtual ~Camera(){
		delete phys;
	}

	void SetDims( Vector3d& _halfwidths ) {
		phys->halfwidths.Set( _halfwidths );
	}
	
	void SetPos( Vector3d& _center ) {
		phys->center.Set( _center );
	}
	
	void MoveTo( Vector3d& pos ) {
		
	}

	ViewPort& GetViewPort() { return m_ViewPort; }

public: //privitize this..
	ViewPort	m_ViewPort;
};

#endif
