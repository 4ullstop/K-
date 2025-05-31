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

void PrintXMMatrix(CXMMATRIX m)
{
    //Store the matrix in a 4x4 float
    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, m);
    
    for (int i = 0; i < 4; ++i)
    {
	for (int j = 0; j < 4; ++j)
	{
	    //access the value from the stored matrix bc CXMMATRIX is different than XMMATRIX which
	    //would allow for easier accessing but we passed it in as a parameter
	    float dest = ((float*)&mat)[i * 4 + j];

	    char buffer[256];
	    sprintf_s(buffer, sizeof(buffer),
		      "%f, ", dest);
	    OutputDebugString(buffer);
	}
	OutputDebugString("\n");
    }
}

int CALLBACK WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow)
{
    XMMATRIX a(1.0f, 0.0f, 0.0f, 0.0f,
	       0.0f, 2.0f, 0.0f, 0.0f,
	       0.0f, 0.0f, 4.0f, 0.0f,
	       1.0f, 2.0f, 3.0f, 1.0f);

    XMMATRIX b = XMMatrixIdentity();

    XMMATRIX c = a * b;

    XMMATRIX d = XMMatrixTranspose(a);

    XMVECTOR det = XMMatrixDeterminant(a);
    XMMATRIX e = XMMatrixInverse(&det, a);

    XMMATRIX f = a * e;

    OutputDebugString("a: ");            
    PrintXMMatrix(a);
    OutputDebugString("\n");

    OutputDebugString("b: ");            
    PrintXMMatrix(b);
    OutputDebugString("\n");

    OutputDebugString("c: ");            
    PrintXMMatrix(c);
    OutputDebugString("\n");

    OutputDebugString("d: ");            
    PrintXMMatrix(d);
    OutputDebugString("\n");

    OutputDebugString("det: ");            
    PrintXMVector(det);
    OutputDebugString("\n");
    
    OutputDebugString("e: ");            
    PrintXMMatrix(e);
    OutputDebugString("\n");

    OutputDebugString("f: ");        
    PrintXMMatrix(f);
    OutputDebugString("\n");    
    return(0);
}


