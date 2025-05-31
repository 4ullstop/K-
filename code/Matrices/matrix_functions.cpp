//The following are functions for matrices in XNA which utilize SIMD

//To represent 4x4 matrices in XNA we use the XMMATRIX class which is defined as follows:

//Matrix type: Sixteen 32 bit floating point components aligned on a
//16 byte boundary and mapped to four hardware vector registers

#if (defined(_XM_X86_) || defined(_XM_X64)) && defined(_XM_NO_INTRINSICS_)
typedef struct _XMMATRIX
#else
typedef _DECLSPEC_ALIGN_16_ struct _XMMATRIX
#endif
{
    union
    {
	//Use 4 XMVECTORs to represent the matrix for SIMD
	XMVECTOR r[4];
	struct
	{
	    FLOAT _11, _12, _13, _14;
	    FLOAT _21, _22, _23, _24;
	    FLOAT _31, _32, _33, _34;
	    FLOAT _41, _42, _43, _44;
	};
	FLOAT m{4][4];
    };

#ifdef __cplusplus

    _XMMATRIX() {};

    //Initialize matrix by specifying 4 row vectors
    _XMMATRIX(FXMVECTOR R0, FXMVECTOR R1, FXMVECTOR R2, CXMVECTOR R3);

    //Initialize matrix by specifying the 16 elements
    _XMMATRIX(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
	      FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
	      FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
	      FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);

    //Pass array of sixteen floats to construct matrix
    _XMMATRIX(CONST FLOAT *pArray);

    FLOAT operator() (UINT Row, UINT Column) CONST {return m[Row][Column]; }

    FLOAT& operator() (UINT Row, UINT Column) {return m[Row][Column]; }

    _XMMATRIX& operator= (CONST _XMMATRIX& M);

#ifndef XM_NO_OPERATOR_OVERLOADS
    _XMMATRIX& operator*= (CONST _XMMATRIX& M);
    _XMMATRIX operator* (CONST _XMMATRIX& M) CONST;
#endif
#endif    
} XMMATRIX;

//Matrices can be created with various constructor methods including:
XMMATRIX XMMatrixSet(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
		     FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
		     FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
		     FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);

//4x4 matrix: 32 bit floating point components
typedef struct _XMFLOAT4X4
{
    union
    {
	struct
	{
	    FLOAT _11, _12, _13, _14;
	    FLOAT _21, _22, _23, _24;
	    FLOAT _31, _32, _33, _34;
	    FLOAT _41, _42, _43, _44;	    
	};
	FLOAT m[4][4];
    };
#ifdef __cplusplus
    _XMFLOAT4X4() {};
    _XMFLOAT4X4(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
		FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
		FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
		FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);
    _XMFLOAT4X4(CONST FLOAT *pArray);

    FLOAT operator() (UINT Row, UINT Column) CONST {return m[Row][Column]; }
    FLOAT& operator() (UINT Row, UINT Column) {return m[Row][Column]; }

    _XMFLOAT4X4 operator= (CONST _XMFLOAT4X4& Float4x4);
} XMFLOAT4X4;

//The following are useful functions for matrices

XMMATRIX XMMatrixIdentity(); //Returns the matrix I

BOOL XMMatrixIsIdentity( //Returns true if M is the identity matrix
    CXMMATRIX M); //Input M

XMMATRIX XMMatrixMultiply( //Returns the matrix product AB
    CXMMATRIX A, //Input A
    CXMMATRIX B); //Input B

XMMATRIX XMMatrixTranspose( //Returns M^T
    CXMMATRIX M); //Input M

XMVECTOR XMMatrixDeterminant( //Returns (det M, det M, det M, det M)
    CXMMATRIX M); //Input M

XMMATRIX XMMatrixInverse( //Returns M^-1
    XMVECTOR* pDeterminant, //Input (det M, det M, det M, det M)
    CXMMATRIX M); //Input M
