#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "PhysicsManager.h"
#include "GameObjectManager.h"

static float ClipVelocity( float velDir, float cap ) {
	if( velDir > 0.0f ) {
		if( velDir > cap ) {
			velDir = cap;
		}
	} else if( velDir < 0.0f ) {
		if( velDir < -cap ) {
			velDir = -cap;
		}
	}
	return velDir;
}

static float VelocityMultiplier( bool onground ) {
	return 15.0f;
	/*
	if( onground ) {
		return 16.667f;
	}

	return 16.667f;//24.0f;
	*/
}

class Player : public GameObject {
public:
	Player();
	virtual ~Player();

	float GetSpeed(){
		return m_xyspeed;
	}

	virtual void Update( float elapsed );

	void SetDims( Vector3d& _halfwidths );

	virtual void SetPos( Vector3d& _center );

	void MoveTo( Vector3d& pos );

	void TurnOnMoveFlag( int flag );
	void TurnOffMoveFlag( int flag );

	void RotateOnYaw( float fDeltaYaw );

	float GetYaw() { return m_Yaw; }

	void RotateOnPitch( float fDeltaPitch );

	float GetPitch() { return m_Pitch; }

private:
	int m_nMoveFlags;
	float m_Yaw;
	float m_Pitch;
	Vector3d m_Velocity;
	bool m_bOnGround;
	float m_xyspeed;
};

#endif
