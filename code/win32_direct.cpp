#include <windows.h>
#include <directxmath.h>
#include <stdio.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 bool32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

using namespace DirectX;

void PrintXMVector(FXMVECTOR v)
{
    XMFLOAT3 dest;
    XMStoreFloat3(&dest, v);
    
    char buffer[256];
    sprintf_s(buffer, sizeof(buffer),
	      "(%f, %f, %f)\n",
	      dest.x, dest.y, dest.z);

    OutputDebugString(buffer);
}

int CALLBACK WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow)
{
    XMVECTOR n = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
    XMVECTOR v = XMVectorSet(-2.0f, 1.0f, -3.0f, 0.0f);
    XMVECTOR w = XMVectorSet(0.707f, 0.707f, 0.0f, 0.0f);

    XMVECTOR a = u + v;
    XMVECTOR b = u - v;

    XMVECTOR c = 10.0f * u;

    XMVECTOR l = XMVector3Length(u);
    XMVECTOR d = XMVector3Normalize(u);
    XMVECTOR s = XMVector3Dot(u, v);
    XMVECTOR e = XMVector3Cross(u, v);

    XMVECTOR projW;
    XMVECTOR perpW;

    XMVector3ComponentsFromNormal(&projW, &perpW, w, n);

    bool32 equal = XMVector3Equal(projW + perpW, w) != 0;
    bool32 notEqual = XMVector3NotEqual(projW + perpW, w) != 0;

    XMVECTOR angleVec = XMVector3AngleBetweenVectors(projW, perpW);
    r32 angleRadians = XMVectorGetX(angleVec);
    r32 angleDegrees = XMConvertToDegrees(angleRadians);

    PrintXMVector(u);
    PrintXMVector(v);    
    PrintXMVector(w);
    PrintXMVector(n);
    PrintXMVector(a);
    PrintXMVector(b);
    PrintXMVector(c);
    PrintXMVector(d);
    PrintXMVector(e);
    PrintXMVector(l);
    PrintXMVector(s);
    PrintXMVector(projW);
    PrintXMVector(perpW);

    if (equal) OutputDebugString("projW + perpW == w = true\n");
    else OutputDebugString("projW + perpW == w = false\n");	

    if (notEqual) OutputDebugString("projW + perpW != w = true\n");
    else OutputDebugString("projW + perpW != w = false\n");

    char r32Buffer[256];
    sprintf_s(r32Buffer, sizeof(r32Buffer),
	      "angle: %f\n", angleDegrees);
    OutputDebugString(r32Buffer);
    
    return(0);
}


