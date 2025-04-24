#pragma once

namespace Engine
{
	typedef struct tagEngineDesc 
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iNumLevels;		
	}ENGINE_DESC;

#pragma region VIBuffer
	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexPosition {
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct tagVertexTexture {
		XMFLOAT3		vPosition;
		XMFLOAT2		vTextureCoord;
	}VTXTEX;

	typedef struct tagVertexNormalTexture {
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTextureCoord;
	}VTXNORTEX;

	typedef struct tagVertexCubeTexture {
		XMFLOAT3		vPosition;
		XMFLOAT3		vTextureCoord;
	}VTXCUBETEX;
#pragma endregion

	
}