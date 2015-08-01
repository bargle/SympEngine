#ifndef _MATRIX_H_ 
#define _MATRIX_H_

class Matrix3x3
{
public:
	Matrix3x3(){}
	void Identity(){
		ma[0][0] = 1.0f;
		ma[0][1] = 0.0f;
		ma[0][2] = 0.0f;

		ma[1][0] = 0.0f;
		ma[1][1] = 1.0f;
		ma[1][2] = 0.0f;

		ma[2][0] = 0.0f;
		ma[2][1] = 0.0f;
		ma[2][2] = 1.0f;
	}

	//Vars
	union {
	float ma[3][3];
	float m[9];
	};

};

class Matrix4x4
{
public:
	Matrix4x4(){}
	void Identity(){
		ma[0][0] = 1.0f;
		ma[0][1] = 0.0f;
		ma[0][2] = 0.0f;
		ma[0][3] = 0.0f;

		ma[1][0] = 0.0f;
		ma[1][1] = 1.0f;
		ma[1][2] = 0.0f;
		ma[1][3] = 0.0f;

		ma[2][0] = 0.0f;
		ma[2][1] = 0.0f;
		ma[2][2] = 1.0f;
		ma[2][3] = 0.0f;

		ma[3][0] = 0.0f;
		ma[3][1] = 0.0f;
		ma[3][2] = 0.0f;
		ma[3][3] = 1.0f;
	}
	
	void Set(Matrix4x4& mat)
	{
		ma[0][0] = mat.ma[0][0];
		ma[0][1] = mat.ma[0][1];
		ma[0][2] = mat.ma[0][2];
		ma[0][3] = mat.ma[0][3];

		ma[1][0] = mat.ma[1][0];
		ma[1][1] = mat.ma[1][1];
		ma[1][2] = mat.ma[1][2];
		ma[1][3] = mat.ma[1][3];

		ma[2][0] = mat.ma[2][0];
		ma[2][1] = mat.ma[2][1];
		ma[2][2] = mat.ma[2][2];
		ma[2][3] = mat.ma[2][3];

		ma[3][0] = mat.ma[3][0];
		ma[3][1] = mat.ma[3][1];
		ma[3][2] = mat.ma[3][2];
		ma[3][3] = mat.ma[3][3];
	}

	void Transpose(){
		float temp;
		temp = ma[0][1]; ma[0][1] = ma[1][0]; ma[1][0] = temp;
		temp = ma[0][2]; ma[0][2] = ma[2][0]; ma[2][0] = temp;
		temp = ma[0][3]; ma[0][3] = ma[3][0]; ma[3][0] = temp;
		temp = ma[1][2]; ma[1][2] = ma[2][1]; ma[2][1] = temp;
		temp = ma[1][3]; ma[1][3] = ma[3][1]; ma[3][1] = temp;
		temp = ma[2][3]; ma[2][3] = ma[3][2]; ma[3][2] = temp;
	}

	/*
	void SetTranslation(Vector3d &vPos)
	{
		ma[3][0] = vPos.x;
		ma[3][1] = vPos.y;
		ma[3][2] = vPos.z;
	}
	*/
	
	void SetTranslation(Vector3d &vPos)
	{
		ma[3][0] = ma[0][0] * vPos.x + ma[1][0] * vPos.y + ma[2][0] * vPos.z;
		ma[3][1] = ma[0][1] * vPos.x + ma[1][1] * vPos.y + ma[2][1] * vPos.z;
		ma[3][2] = ma[0][2] * vPos.x + ma[1][2] * vPos.y + ma[2][2] * vPos.z;
	}

	void RotateYaw(float angle)
	{
		float cos0 = cosf(angle);
		float sin0 = sinf(angle);

		ma[0][0] = cos0;
		ma[0][2] = sin0;
		ma[2][0] = -sin0;
		ma[2][2] = cos0;
	}
	
	void RotatePitch(float angle)
	{
		float cos0 = cosf(angle);
		float sin0 = sinf(angle);

		ma[1][1] = cos0;
		ma[1][2] = -sin0;
		ma[2][1] = sin0;
		ma[2][2] = cos0;
	}
	
	void RotateRoll(float angle)
	{
		float cos0 = cosf(angle);
		float sin0 = sinf(angle);

		ma[0][0] = cos0;
		ma[0][1] = sin0;
		ma[1][0] = -sin0;
		ma[1][1] = cos0;
	}

	void NormalizeRotation()
	{
		float f = sqrtf( (ma[0][0]*ma[0][0]) + (ma[0][1]*ma[0][1]) + (ma[0][2]*ma[0][2]) );
		ma[0][0] /= f;
		ma[0][1] /= f;
		ma[0][2] /= f;
		
		f = sqrtf( (ma[1][0]*ma[1][0]) + (ma[1][1]*ma[1][1]) + (ma[1][2]*ma[1][2]) );
		ma[1][0] /= f;
		ma[1][1] /= f;
		ma[1][2] /= f;
		
		f = sqrtf( (ma[2][0]*ma[2][0]) + (ma[2][1]*ma[2][1]) + (ma[2][2]*ma[2][2]) );
		ma[2][0] /= f;
		ma[2][1] /= f;
		ma[2][2] /= f;
		
		float a = sqrtf( (ma[0][0]*ma[0][0]) + (ma[1][0]*ma[1][0]) + (ma[2][0]*ma[2][0]) );
		ma[0][0] /= a;
		ma[1][0] /= a;
		ma[2][0] /= a;
		
		a = sqrtf( (ma[0][1]*ma[0][1]) + (ma[1][1]*ma[1][1]) + (ma[2][1]*ma[2][1]) );
		ma[0][1] /= a;
		ma[1][1] /= a;
		ma[2][1] /= a;
		
		a = sqrtf( (ma[0][2]*ma[0][2]) + (ma[1][2]*ma[1][2]) + (ma[2][2]*ma[2][2]) );
		ma[0][2] /= a;
		ma[1][2] /= a;
		ma[2][2] /= a;
	}
	
	void Multiply(Matrix4x4& mat)
	{/*
		ma[0][0] = ( ma[0][0]*mat.ma[0][0] ) + ( ma[0][1]*mat.ma[1][0] ) + ( ma[0][2]*mat.ma[2][0] ) + ( ma[0][3]*mat.ma[3][0] );
		ma[0][1] = ( ma[0][0]*mat.ma[0][1] ) + ( ma[0][1]*mat.ma[1][1] ) + ( ma[0][2]*mat.ma[2][1] ) + ( ma[0][3]*mat.ma[3][1] );
		ma[0][2] = ( ma[0][0]*mat.ma[0][2] ) + ( ma[0][1]*mat.ma[1][2] ) + ( ma[0][2]*mat.ma[2][2] ) + ( ma[0][3]*mat.ma[3][2] );
		ma[0][3] = ( ma[0][0]*mat.ma[0][3] ) + ( ma[0][1]*mat.ma[1][3] ) + ( ma[0][2]*mat.ma[2][3] ) + ( ma[0][3]*mat.ma[3][3] );
		
		//not finished
		ma[1][0] = ( ma[1][0]*mat.ma[0][0] ) + ( ma[1][1]*mat.ma[1][0] ) + ( ma[1][2]*mat.ma[2][1] ) + ( ma[1][3]*mat.ma[3][1] );
		ma[1][1] = ( ma[0][0]*mat.ma[0][1] ) + ( ma[0][1]*mat.ma[1][1] ) + ( ma[0][2]*mat.ma[2][1] ) + ( ma[0][3]*mat.ma[3][1] );
		ma[1][2] = ( ma[0][0]*mat.ma[0][2] ) + ( ma[0][1]*mat.ma[1][2] ) + ( ma[0][2]*mat.ma[2][2] ) + ( ma[0][3]*mat.ma[3][2] );
		ma[1][3] = ( ma[0][0]*mat.ma[0][3] ) + ( ma[0][1]*mat.ma[1][3] ) + ( ma[0][2]*mat.ma[2][3] ) + ( ma[0][3]*mat.ma[3][3] );
		*/

		/*
		   int i,j;
		   for(i=0; i<4; i++)
		   	for(j=0; j<4; j++) {
		   		//ma[i][j]= ma[i][0]*mat.ma[0][j] + ma[i][1]*mat.ma[1][j] + ma[i][2]*mat.ma[2][j] + ma[i][3]*mat.ma[3][j];
		   		ma[i][j]= mat.ma[i][0]*ma[0][j] + mat.ma[i][1]*ma[1][j] + mat.ma[i][2]*ma[2][j] + mat.ma[i][3]*ma[3][j];
		   }
		   */
		   
		   //optimization... we're only multiplying the rotation components
		   
			ma[0][0] = ma[0][0] * mat.ma[0][0] + ma[0][1] * mat.ma[1][0] + ma[0][2] * mat.ma[2][0];
			ma[0][1] = ma[0][0] * mat.ma[0][1] + ma[0][1] * mat.ma[1][1] + ma[0][2] * mat.ma[2][1];
			ma[0][2] = ma[0][0] * mat.ma[0][2] + ma[0][1] * mat.ma[1][2] + ma[0][2] * mat.ma[2][2];

			ma[1][0] = ma[1][0] * mat.ma[0][0] + ma[1][1] * mat.ma[1][0] + ma[1][2] * mat.ma[2][0];
			ma[1][1] = ma[1][0] * mat.ma[0][1] + ma[1][1] * mat.ma[1][1] + ma[1][2] * mat.ma[2][1];
			ma[1][2] = ma[1][0] * mat.ma[0][2] + ma[1][1] * mat.ma[1][2] + ma[1][2] * mat.ma[2][2];

			ma[2][0] = ma[2][0] * mat.ma[0][0] + ma[2][1] * mat.ma[1][0] + ma[2][2] * mat.ma[2][0];
			ma[2][1] = ma[2][0] * mat.ma[0][1] + ma[2][1] * mat.ma[1][1] + ma[2][2] * mat.ma[2][1];
			ma[2][2] = ma[2][0] * mat.ma[0][2] + ma[2][1] * mat.ma[1][2] + ma[2][2] * mat.ma[2][2];

	}

	void Transform(Vector3d &in, Vector3d &out)
	{
		out.x = (in.x * ma[0][0]) + (in.y * ma[1][0]) + (in.z * ma[2][0]) + ma[3][0];
		out.y = (in.x * ma[0][1]) + (in.y * ma[1][1]) + (in.z * ma[2][1]) + ma[3][1];
		out.z = (in.x * ma[0][2]) + (in.y * ma[1][2]) + (in.z * ma[2][2]) + ma[3][2];
	}
	
	bool IsOrthogonal(){
		//We only care about the rotational components
		
		//check rows
		float row1 = sqrtf( (ma[0][0]*ma[0][0]) + (ma[0][1]*ma[0][1]) + (ma[0][2]*ma[0][2]) );
		float row2 = sqrtf( (ma[1][0]*ma[1][0]) + (ma[1][1]*ma[1][1]) + (ma[1][2]*ma[1][2]) );
		float row3 = sqrtf( (ma[2][0]*ma[2][0]) + (ma[2][1]*ma[2][1]) + (ma[2][2]*ma[2][2]) );
		
		if ( fabs( 1.0f - row1 ) > 0.001f ) {
			return false;
		}

		if ( fabs( 1.0f - row2 ) > 0.001f ) {
			return false;
		}
		
		if ( fabs( 1.0f - row3 ) > 0.001f ) {
			return false;
		}
		//check columns
		
		return true;
	}
	
	void Print() {
		
		printf("Mat4x4:\n");
		
		int i;
		for(i = 0; i < 4; i++) {
			printf("[%f %f %f %f]\n", ma[i][0], ma[i][1], ma[i][2], ma[i][3]);
		}
	}

	//Vars
	union {
	float ma[4][4];
	float m[16];
	};

};

#endif
