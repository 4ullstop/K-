#include <windows.h>
#include <directxmath.h>
#include <stdio.h>
#include <stdint.h>
#include <cmath>

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

void PrintArray(i32** array, i32 n)
{
    for (int i = 0; i < n; ++i)
    {
	for (int j = 0; j < n; ++j)
	{
	    i32 ij = array[i][j];
	    char buffer[256];
	    sprintf_s(buffer, sizeof(buffer),
		      "%d, ", ij);
	    OutputDebugString(buffer);
	}
	OutputDebugString("\n");
    }
}

i32** TransposeArray(i32** oldArray, i32 n)
{
    i32** fillM = (i32**)malloc(n * sizeof(i32*));
    for (int i = 0; i < n; ++i)
    {
	fillM[i] = (i32*)malloc(n * sizeof(i32));
    }
    
    for (int i = 0; i < n; ++i)
    {
	for (int j = 0; j < n; ++j)
	{
	    i32 ij = oldArray[j][i];
	    fillM[i][j] = ij;
	}
    }
    return fillM;
}

i32 DeterminantOfM2(i32 m2[][2])
{
    return (m2[0][0] * m2[1][1]) - (m2[0][1] * m2[1][0]);
}

i32 DeterminantOfM3(i32 m3[][3])
{
    
    i32 m2Determinants[3];
    i32 determinantIndex = 0;
    for (int k = 0; k < 3; k++)
    {
	i32 m2[2][2];

	i32 m2I = 0;
	
	//between each loop of getting the minor matrix, add to this array using the DeterminantOfM2 function
	for (i32 i = 1; i < 3; ++i)
	{
	    i32 m2J = 0;	    
	    for (i32 j = 0; j < 3; ++j)
	    {
		if (j == determinantIndex) continue;
		m2[m2I][m2J] = m3[i][j];
		m2J++;
	    }
	    m2I++;

	}
	determinantIndex++;
	m2Determinants[k] = DeterminantOfM2(m2);
    }

    return (m3[0][0] * m2Determinants[0]) - (m3[0][1] * m2Determinants[1]) + (m3[0][2] * m2Determinants[2]);
}

typedef struct
{
    i32 determinant;
    i32 determinantsOfM3[4];
} m4_data;

m4_data DeterminantOfM4(i32 m4[][4])
{
    m4_data data = {};
    i32 m3Determinants[4];
    i32 determinantIndex = 0;
    for (i32 k = 0; k < 4; ++k)
    {
	i32 m3[3][3];
	
	i32 m3I = 0;
	for (i32 i = 1; i < 4; ++i)
	{
	    i32 m3J = 0;
	    for (i32 j = 0; j < 4; ++j)
	    {
		if (j == determinantIndex) continue;
		m3[m3I][m3J] = m4[i][j];
		m3J++;
	    }
	    m3I++;
	}
	determinantIndex++;
	m3Determinants[k] = DeterminantOfM3(m3);
	data.determinantsOfM3[k] = m3Determinants[k];
    }

    
    data.determinant =  (m4[0][0] * m3Determinants[0]) - (m4[0][1] * m3Determinants[1]) + (m4[0][2] * m3Determinants[2]) - (m4[0][3] * m3Determinants[3]);

    return data;
}

int CALLBACK WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow)
{
    //Matrix setup
    i32 n = 3;
    i32** matrix = (i32**)malloc(n * sizeof(i32*));
    for (int i = 0; i < n; ++i)
    {
	matrix[i] = (i32*)malloc(n * sizeof(i32));
    }

    for (int i = 0; i < n; ++i)
    {
	for (int j = 0; j < n; ++j)
	{
	    matrix[i][j] = (i * j + 1) + i;
	}
    }

    PrintArray(matrix, n);
    OutputDebugString("\n");
    OutputDebugString("\n");    
    //Write a program that computes the transpose of an n x n matrix using an array of arrays
    i32** fillM = TransposeArray(matrix, n);
    PrintArray(fillM, n);   
    //Write a program that computers the determinant and inverse of matrices usign an array of arrays

    i32 m4[4][4] = {
	{1, 2, 3, 4},
	{5, 6, 7, 8},
	{9, 10, 12, 11},
	{13, 14, 15, 17}
    };

    m4_data m4Determinant = DeterminantOfM4(m4);

    //Define all of the determinants of all of the values of M4
    i32 allDeterminants[4][4];
    for (i32 k = 0; k < 4; ++k)
    {
	for (i32 l = 0; l < 4; ++l)
	{
	    i32 m3I = 0;
	    i32 tempM3[3][3];	    
	    for (i32 i = 0; i < 4; ++i)
	    {
		i32 m3J = 0;
		if (i == k) continue;
		for (i32 j = 0; j < 4; ++j)
		{
		    if (j == l) continue;
		    tempM3[m3I][m3J] = m4[i][j];
		    m3J++;
		}
		m3I++;
	    }
	    allDeterminants[k][l] = DeterminantOfM3(tempM3);	    
	}

    }

    
    char buffer[256];
    sprintf_s(buffer, sizeof(buffer),
	      "Determinant of m4 = %d\n", m4Determinant.determinant);
    OutputDebugString(buffer);

    //Now that the determinant is found, we need to find the adjoint
    i32** adjoint = (i32**)malloc(4 * sizeof(i32*));
    for (int i = 0; i < 4; ++i)
    {
	adjoint[i] = (i32*)malloc(4 * sizeof(i32));
    }    
    for (i32 i = 0; i < 4; ++i)
    {
	for (i32 j = 0; j < 4; ++j)
	{
	    adjoint[i][j] = (i32)pow(-1, (i + 1) + (j + 1)) * allDeterminants[i][j];
	}
    }
    adjoint = TransposeArray(adjoint, 4);

    PrintArray(adjoint, 4);
    OutputDebugString("\n");
    OutputDebugString("\n");
    OutputDebugString("\n");    
//The current issue is, is that you aren't getting the right determinant values for the adjoint to
    //appear properly you're doing a11 a12 a13 a14, you need to do a11 a12 a21 a22 for getting the determinants
    // of the m3 matrix
    r32 scalar = (1 / (r32)m4Determinant.determinant);
    r32 outMat[4][4];

    for (int i = 0; i < 4; ++i)
    {
	for (int j = 0; j < 4; ++j)
	{
	    outMat[i][j] = scalar * adjoint[i][j];
	    char inverseBuffer[256];
	    sprintf_s(inverseBuffer, sizeof(inverseBuffer),
		      "%f, ", outMat[i][j]);
	    OutputDebugString(inverseBuffer);
	}
	OutputDebugString("\n");
    }
    return(0);
}
