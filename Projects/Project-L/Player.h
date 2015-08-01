#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "PhysicsManager.h"
#include "GameObjectManager.h"

float ClipVelocity( float velDir, float cap ) {
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

float VelocityMultiplier( bool onground ) {
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
	Player():GameObject(),
		m_nMoveFlags(0),
		m_Yaw(0.0f),
		m_Pitch(0.0f),
		m_bOnGround(false),
		m_xyspeed(0.0f)
	{
		m_Pos.Set(0.0f, 0.0f, 0.0f);
		phys = new PhysObject;

		phys->halfwidths.Set(2.5f, 6.0f, 2.5f);
	}
	virtual ~Player(){
		delete phys;
	}

	float GetSpeed(){
		return m_xyspeed;
	}

	virtual void Update( float elapsed ){

		//float airFriction = 60.0f;
		//float groundFriction = 60.0f;

		float blockFrictionMultiplier = 1.0f;//0.85f;
		
		float frictionMultiplier = blockFrictionMultiplier;//1.0f;
		
		//Grab our directons
		float angleX = (float)sin( DEG_TO_RAD(m_Yaw) );
		float angleZ = (float)cos( DEG_TO_RAD(m_Yaw) );
		float angleX_right = (float)sin( DEG_TO_RAD(m_Yaw) - DEG_TO_RAD( 90.0f ) );// * 0.5f;
		float angleZ_right = (float)cos( DEG_TO_RAD(m_Yaw) - DEG_TO_RAD( 90.0f) );//* 0.5f;
		float moveSpeed = 100.0f;

		Vector3d pos;
		pos.Set( m_Pos );

#if 1

		float velMultiplier = VelocityMultiplier(m_bOnGround);
		
		if(0)
		{
			float xzmax = 60.0f;
			if( m_xyspeed > xzmax) {
				float overageFactor = m_xyspeed / xzmax;
				//m_Velocity.x *= 1.0f - overageFactor;
				//m_Velocity.z *= 1.0f - overageFactor;
				
				velMultiplier  *= (1.0f - overageFactor);
				
				if( velMultiplier < 0.0f ) {
					velMultiplier = 1.0f;
				}
			}
		}

		if( m_nMoveFlags & (1<<MOVE_JET) ) {
				m_Velocity.y += 64.0f*elapsed;
				//if( m_Velocity.y > 24.0f ) {
				//	m_Velocity.y = 24.0f;
				//}
		}

		if( m_nMoveFlags & (1<<MOVE_JUMP) ) {
			if(m_bOnGround) {
				m_Velocity.y += 72.0f;
				//if( m_Velocity.y > 6.0f ) {
					//m_Velocity.y = 6.0f;
				//}
			}
			m_nMoveFlags &= ~(1<<MOVE_JUMP);
		}

		if( m_nMoveFlags & (1<<MOVE_FORWARD) ) {
			if ( m_xyspeed < moveSpeed )
			{
				m_Velocity.x -= (angleX * velMultiplier) * moveSpeed * elapsed;
				m_Velocity.z -= (angleZ * velMultiplier) * moveSpeed * elapsed;
			}
		}
	
		if( m_nMoveFlags & (1<<MOVE_BACKWARD) ) {
			if ( m_xyspeed < moveSpeed )
			{
				m_Velocity.x += (angleX * velMultiplier) * moveSpeed * elapsed;
				m_Velocity.z += (angleZ * velMultiplier) * moveSpeed * elapsed;
			}
		}
	
		if( m_nMoveFlags & (1<<MOVE_RIGHT) ) {
			if ( m_xyspeed < moveSpeed )
			{
				m_Velocity.x += (angleX_right * velMultiplier) * moveSpeed * elapsed;
				m_Velocity.z += (angleZ_right * velMultiplier) * moveSpeed * elapsed;
			}
		}
	
		if( m_nMoveFlags & (1<<MOVE_LEFT) ) {
			if ( m_xyspeed < moveSpeed )
			{
				m_Velocity.x -= (angleX_right * velMultiplier) * moveSpeed * elapsed;
				m_Velocity.z -= (angleZ_right * velMultiplier) * moveSpeed * elapsed;
			}
		}
/*
		m_Velocity.x = ClipVelocity(m_Velocity.x, 64.0f);
		m_Velocity.y = ClipVelocity(m_Velocity.y, 64.0f);
		m_Velocity.z = ClipVelocity(m_Velocity.z, 64.0f);
*/
		if ( fabs( m_Velocity.x ) > 0.01f )
		{
			pos.x += m_Velocity.x * elapsed;
			if ( PhysicsManager::GetInstance().TestBox( pos, phys->halfwidths ) ) {
				pos.x = m_Pos.x;
				frictionMultiplier = blockFrictionMultiplier;
			}
		}

		if ( fabs( m_Velocity.z ) > 0.01f )
		{
			pos.z += m_Velocity.z * elapsed;

			if ( PhysicsManager::GetInstance().TestBox( pos, phys->halfwidths ) ) {
				pos.z = m_Pos.z;
				frictionMultiplier = blockFrictionMultiplier;
			}
		}

/*		
		//Gravity???
		pos.y += m_Velocity.y;
		if ( PhysicsManager::GetInstance().TestBox( pos, phys->halfwidths ) ) {
			pos.y = m_Pos.y;
		}
*/
		pos.y += m_Velocity.y * elapsed;

		Vector3d newPos;
		newPos.Set(pos);
		//PhysicsManager::GetInstance().ApplyGravity(elapsed, newPos);

		if ( PhysicsManager::GetInstance().TestBox( newPos, phys->halfwidths ) ) {
			m_bOnGround = true;

			pos.y = m_Pos.y;

			//on ground...
			m_Velocity.y -= 1.0f * elapsed;
			if( m_Velocity.y < -1.0f ) {
				m_Velocity.y = -1.0f;
			}
			
			//friction...
			float frictionConstant = 0.777f;

			float dampen = (frictionConstant * (1.0f - elapsed)) * frictionMultiplier;

			if ( dampen >= 0.99f ) {
				dampen = 0.99f;
			}
				
			if ( fabs( m_Velocity.x ) < 0.001f )
			{
				m_Velocity.x = 0.0f;
			}
			else
			{
				m_Velocity.x *= dampen;
			}

			if ( fabs( m_Velocity.z ) < 0.001f )
			{
				m_Velocity.z = 0.0f;
			}
			else
			{
				m_Velocity.z *= dampen;
			}
			

			//printf("Ground COLLIDE: elapsed: %f dampen: %f (x %f, z %f)\n", elapsed, dampen, m_Velocity.x, m_Velocity.z);
			
		} else {

			m_bOnGround = false;
			//pos.y = newPos.y;
			//printf("Ground move: %f (%f)\n", pos.y, elapsed);
			
			if( !(m_nMoveFlags & (1<<MOVE_JET)) ) {
				//in air...
                
                //GRAVITY
                
				m_Velocity.y -= 256.0f * elapsed;
				if( m_Velocity.y < -1024.0f ) {
                 m_Velocity.y = -1024.0f;
				}
                 
			}


			float frictionConstant = 0.777f; //0.98f
			
			float dampen = ( frictionConstant * (1.0f - elapsed) ) * frictionMultiplier;

			if ( dampen >= 0.99f ) {
				dampen = 0.99f;
			}
				
			if ( fabs( m_Velocity.x ) < 0.001f )
			{
				m_Velocity.x = 0.0f;
			}
			else
			{
				m_Velocity.x *= dampen;
			}

			if ( fabs( m_Velocity.z ) < 0.001f )
			{
				m_Velocity.z = 0.0f;
			}
			else
			{
				m_Velocity.z *= dampen;
			}
		}
		
		//printf("Vel: %f %f %f\n", m_Velocity.x, m_Velocity.y, m_Velocity.z);
		
#else

		//printf("X :%f, Z :%f, XR :%f, XZ :%f\n", angleX, angleZ, angleX_right, angleZ_right);

		if( m_nMoveFlags & (1<<MOVE_JUMP) ) {
			Vector3d testpos;
			testpos.Set( pos.x, pos.y + 10.0f, pos.z );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.y += 10.0f * elapsed;
			}
			//m_nMoveFlags &= ~(1<<MOVE_JUMP);
		}

		if( m_nMoveFlags & (1<<MOVE_FORWARD) ) {

			Vector3d testpos;
			testpos.Set( pos.x - angleX , pos.y, pos.z );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.x -= angleX;
			}

			testpos.Set( pos.x , pos.y, pos.z - angleZ );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.z -= angleZ;
			}
		}

		if( m_nMoveFlags & (1<<MOVE_BACKWARD) ) {
			Vector3d testpos;
			testpos.Set( pos.x + angleX , pos.y, pos.z );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.x += angleX;
			}

			testpos.Set( pos.x , pos.y, pos.z + angleZ );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.z += angleZ;
			}
		}

		if( m_nMoveFlags & (1<<MOVE_RIGHT) ) {

			Vector3d testpos;
			testpos.Set( pos.x + angleX_right , pos.y, pos.z );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.x += angleX_right;
			}

			testpos.Set( pos.x , pos.y, pos.z + angleZ_right );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.z += angleZ_right;
			}
		}

		if( m_nMoveFlags & (1<<MOVE_LEFT) ) {

			Vector3d testpos;
			testpos.Set( pos.x - angleX_right , pos.y, pos.z );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.x -= angleX_right;
			}

			testpos.Set( pos.x , pos.y, pos.z - angleZ_right );
			if ( !PhysicsManager::GetInstance().TestBox( testpos, phys->halfwidths ) ) {
				pos.z -= angleZ_right;
			}
		}

		Vector3d newPos;
		newPos.Set(pos);
		PhysicsManager::GetInstance().ApplyGravity(elapsed, newPos);

		if ( !PhysicsManager::GetInstance().TestBox( newPos, phys->halfwidths ) ) {
			pos.y = newPos.y;
			//printf("Ground move: %f (%f)\n", pos.y, elapsed);
		} else {
			//printf("Ground COLLIDE\n");

			//on ground...

		}

#endif

		m_xyspeed = sqrt( ((pos.x - m_Pos.x)*(pos.x - m_Pos.x)) + ((pos.z - m_Pos.z)*(pos.z - m_Pos.z)) ) / elapsed;
		//printf("XZ speed: %f (elapsed: %f  FPS: %f)\n", m_xyspeed, elapsed, 1.0f / elapsed);
		//printf("elapsed: %f\n", elapsed);
		//SET POS
		m_Pos.Set( pos );

		

	}

	void SetDims( Vector3d& _halfwidths ) {
		phys->halfwidths.Set( _halfwidths );
	}

	virtual void SetPos( Vector3d& _center ) {
		phys->center.Set( _center );
		GameObject::SetPos( _center );
	}

	void MoveTo( Vector3d& pos ) {

	}

	void TurnOnMoveFlag( int flag ) { m_nMoveFlags |= (1<<flag); }
	void TurnOffMoveFlag( int flag ) { m_nMoveFlags &= ~(1<<flag); }

	void RotateOnYaw( float fDeltaYaw ) {
		m_Yaw += fDeltaYaw;
	}

	float GetYaw() { return m_Yaw; }

	void RotateOnPitch( float fDeltaPitch ) {
		m_Pitch += fDeltaPitch;
	}

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
