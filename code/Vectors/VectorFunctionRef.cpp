//The following are some reference functions you may use for the purposes
//of SIMD


//Loads XMFLOAT2 int XMVECTOR
XMVECTOR XMLoadFloat2(CONST XMFLOAT2 *pSource);
//Loads XMFLOAT3 int XMVECTOR    
XMVECTOR XMLoadFloat3(CONST XMFLOAT3 *pSource);
//Loads XMFLOAT4 int XMVECTOR    
XMVECTOR XMLoadFloat4(CONST XMFLOAT4 *pSource);

//We also have
//Loads 3-element UINT array into XMVECTOR
XMVECTOR XMLoadInt3(CONST UINT* pSource);
//Loads XMCOLOR into XMVECTOR
XMVECTOR XMLoadColor(CONST XMCOLOR *pSource);
//Loads XMBYTE4 into XMVECTOR
XMVECTOR XMLoadByte4(CONST XMBYTE4 *pSource);
//among many others


//The following methods store data from XMVECTOR into XMFLOAT*

//Loads XMVECTOR into XMFLOAT2
VOID XMStoreFloat2(XMFLOAT2 *pDestination, FXMVECTOR V);

//Loads XMVECTOR into XMFLOAT3
VOID XMStoreFloat3(XMFLOAT3 *pDestination, FXMVECTOR V);

//LOADS XMVECTOR into XMFLOAT4
VOID XMStoreFloat4(XMFLOAT4 *pDestination, FXMVECTOR V);

//We also have
//Loads XMVECTOR into 3 element UINT array
VOID XMStoreInt3(UINT* pDestination, FXMVECTOR V);

// Loads XMVECTOR into XMCOLOR
VOID XMStoreColor(XMCOLOR* pDestination, FXMVECTOR V);

//Loads XMVECTOR into XMBYTE4
VOID XMStoreByte4(XMBYTE4 *pDestination, FXMVECTOR V);
//among many others

//Sometimes we want to get or set one component of an XMVECTOR and we have getters and setters as so:

FLOAT XMVectorGetX(FXMVECTOR V);
FLOAT XMVectorGetY(FXMVECTOR V);
FLOAT XMVectorGetZ(FXMVECTOR V);
FLOAT XMVectorGetW(FXMVECTOR V);

XMVECTOR XMVectorSetX(FXMVECTOR V, FLOAT x);
XMVECTOR XMVectorSetY(FXMVECTOR V, FLOAT y);
XMVECTOR XMVectorSetZ(FXMVECTOR V, FLOAT z);
XMVECTOR XMVectorSetW(FXMVECTOR V, FLOAT w);

//
// The following is how to pass the XMVECTOR as a parameter
//

//32-bit Windows
typedef const XMVECTOR FXMVECTOR;
typedef const XMVECTOR& CXMVECTOR;

//64-bit Windows
typedef const XMVECTOR& FXMVECTOR;
typedef const XMVECTOR& CXMVECTOR;

//The following demonstrates the rule of the different data types for parameters of XMVECTOR
XMINLINE XMMATRIX XMMatrixTransform(
    FXMVECTOR ScalingOrigin,
    FXMVECTOR ScalingOrientationQuaternion,
    FXMVECTOR Scaling,
    CXMVECTOR RotationOrigin,
    CXMVECTOR RotationQuaternion,
    CXMVECTOR Translation);
//Where the first 3 values should be of type FXMVECTOR and any subsequent XMVECTOR parameters in the function
//declaration and definition must be of type CXMVECTOR

//Another example where there can be values in between the XMVECTOR data types but it still follows the rule stated
//above

XMINLINE XMMATRIX XMMatrixTransformation2D(
    FXMVECTOR ScalingOrigin,
    FLOAT ScalingOrientation,
    FXMVECTOR Scaling,
    FXMVECTOR RotationOrigin,
    FLOAT Rotation,
    CXMVECTOR Translation);

//To initialize XMVECTOR with values we must use XMVECTORF32
//Below is an example of this taken from the DirectX SDK CascadedShadowMaps11 sample:
static const XMVECTORF32 g_vHalfVector = {0.5f, 0.5f, 0.5f, 0.5f};
static const XMVECTORF32 g_vZero = {0.0f, 0.0f, 0.0f, 0.0f};

XMVECTORF32 vRightTop = {
    vViewFrust.RightSlope,
    vViewFrust.TopSlope,
    1.0f, 1.0f};

XMVECTORF32 vLeftBottom = {
    vViewFrust.LeftSlope,
    vViewFrust.BottomSlope,
    1.0f, 1.0f};

//XMVECTORF32 is a 16-byte aligned structure with a XMVECTOR conversion operator defined as follows:
typedef _DECLSPEC_ALIGN_16_ struct XMVECTORF32 {
    union {
	float f[4];
	XMVECTOR v;
    };
#if defined (__cplusplus)
    inline operator XMVECTOR() const {return v;}
#if !defined (_XM_NO_INTRINSICS_) && defined (_XM_SEE_INTRINSICS_)
    inline operator __m128i() const
	{return reinterpret_cast<const __m128i *>(&v)[0];}
    inline operator __m128d() const
	{return reinterpret_cast<const __m128d *>(&v)[0];}
#endif
#endif //__cplusplus    
} XMVECTORF32;

//
//The following are overloaded operators for the XMVECTOR data type
//

XMVECTOR operator+ (FXMVECTOR V);
XMVECTOR operator- (FXMVECTOR V);

XMVECTOR operator+= (XMVECTOR& V1, FXMVECTOR V2);
XMVECTOR operator-= (XMVECTOR& V1, FXMVECTOR V2);
XMVECTOR operator*= (XMVECTOR& V1, FXMVECTOR V2);
XMVECTOR operator/= (XMVECTOR& V1, FXMVECTOR V2);
XMVECTOR operator*= (XMVECTOR& V, FLOAT S);
XMVECTOR operator/= (XMVECTOR& V, FLOAT S);

XMVECTOR operator+ (FXMVECTOR V1, FXMVECTOR v2);
XMVECTOR operator- (FXMVECTOR V1, FXMVECTOR v2);
XMVECTOR operator* (FXMVECTOR V1, FXMVECTOR v2);
XMVECTOR operator/ (FXMVECTOR V1, FXMVECTOR v2);
XMVECTOR operator* (FXMVECTOR V, FLOAT S);
XMVECTOR operator* (FLOAT S, XMVECTOR V);
XMVECTOR operator/ (FXMVECTOR V, FLOAT s);

//
//Misc XMVECTOR
//

#define XM_PI 3.141592654f
#define XM_2PI 6.283185307f
#define XM_1DIVPI 0.318309886f
#define XM_1DIV2PI 0.159154943f
#define XM_PIDIV2 1.570796327f
#define XM_PIDEV4 0.785398163f

XMFINLINE FLOAT XMConvertToRadians(FLOAT fDegrees)
{
    return fDegrees * (XM_PI / 180.0f);
}

XMFINLINE FLOAT XMConvertToDegrees(FLOAT fRadians)
{
    return fRadians * (180.0f / XM_PI);
}

#define XMMin(a, b) (((a) < (b)) ? (a) : (b))
#define XMMax(a, b) (((a) > (b)) ? (a) : (b))


//
//Setters for XMVECTOR
//

// Returns the zero vector
XMVECTOR XMVectorZero();
//Returns the Vector (1, 1, 1)
XMVECTOR XMVectorSplatOne();
//Returns the vector (x, y, z, w)
XMVECTOR XMVectorSet(FLOAT X, FLOAT Y, FLOAT Z, FLOAT W);
//Returns the vector (s, s, s, s)
XMVECTOR XMVectorReplicate(FLOAT s);
//Returns the Vector (vx, vx, vx, vx)
XMVECTOR XMVectorSplatX(FXMVECTOR V);
//Returns the Vector (vy, vy, vy)
XMVECTOR XMVectorSplatY(FXMVECTOR V);
//Returns the Vector (vz, vz, vz)
XMVECTOR XMVectorSplatZ(FXMVECTOR V);

//
//Vector Functions
//

//These functions are written for 3D vectors but are available for 2D and 4D with the same names just substituted for
//the 2 and 4 in the spot of the 3

//Returns ||v||
XMVECTOR XMVector3Length(
    FXMVECTOR v); //Input v

//Returns ||v||^2
XMVECTOR XMVector3LengthSq(
    FXMVECTOR v); //Input v

//Returns v1 dot v2
XMVECTOR XMVector3Dot(
    FXMVECTOR v1, //Input v1
    FXMVECTOR v2,); //Input v2

//Returns v1 cross v2
XMVECTOR XMVector3Cross(
    FXMVECTOR v1, //Input v1
    FXMVECTOR v2); //Input v2

//Returns v/||v||
XMVECTOR XMVector3Normalize(
    FXMVECTOR v); //Input v

//Returns a vector orthogonal to v
XMVECTOR XMVector3Orthogonal(
    FXMVECTOR v); //Input v

//Returns the angle between v1 and v2
XMVECTOR XMVector3AngleBetweeVectors(
    FXMVECTOR v1, //Input v1
    FXMVECTOR v2); //Input v2x

VOID XMVector3ComponentsFromNormal(
    XMVECTOR* pParallel, //Returns projn(v)
    XMVECTOR* pPerpendicular, //Returns perpn(v)
    FXMVECTOR v, //Input v
    FXMVECTOR normal); //Input n

//Returns v1 == v2;
BOOL XMVector3Equal(
    FXMVECTOR v1, //Input v1
    FXMVECTOR v2); //Input v2

//Returns v1 != v2
BOOL XMVector3NotEqual(
    FXMVECTOR v1, //Input v1
    FXMVECTOR v2); //Input v2
