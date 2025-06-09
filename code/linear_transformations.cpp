
//Constructs a scaling matrix
XMMATRIX XMMatrixScaling(
    FLOAT ScaleX,
    FLOAT ScaleY,
    FLOAT ScaleZ);

//Constructs a scaling matrix from components in a vector
XMMATRIX XMMatrixScalingFromVector(
    FXMVECTOR Scale); //Scaling factors (sx, sy, sz)

//Constructs a x-axis rotation matrix: Rx
XMMATRIX XMMatrixRotationX(
    FLOAT Angle); //Clockwise angle theta to rotate

//Constructs a y-axis rotation matrix: Ry
XMMATRIX XMMatrixRotationY(
    FLOAT Angle); //Clockwise angle theta to rotate

//Constructs a z-axis rotation matrix: Rz
XMMATRIX XMMatrixRotationZ(
    FLOAT Angle); //Clockwise angle theta to rotate

//Constructs an arbitrary axis rotation matrix: Rn
XMMATRIX XMMatrixRotationAxis(
    FXMVECTOR Axis, //Axis n to rotate about
    FLOAT Angle); //Clockwise angle theta to rotate

//Constructs a translation matrix:
XMMATRIX XMMatrixTranslation(
    FLOAT OffsetX,
    FLOAT OffsetY,
    FLOAT OffsetZ); //Translation factors

//Constructs a translation matrix from components in a vector;
XMMATRIX XMMatrixTranslationFromVector(
    FXMVECTOR Offset); //Translation factors (tx, ty, tz)

//Computes the vector-matrix product vM:
XMVECTOR XMVector3Transform(
    FXMVECTOR V, //Input v
    CXMMATRIX M); //Input M

//Computes the vector-matrix product vM where vw = 1 (for transforming points)
XMVECTOR XMVector3TransformCoord(
    FXMVECTOR V, //Input v
    CXMMATRIX M); //Input M

//Computes the vector-matrix product vM where vw = 0 (for transforming vectors)
XMVECTOR XMVectorTransformNormal(
    FXMVECTOR V, //Input v
    CXMMATRIX M); //Input M
