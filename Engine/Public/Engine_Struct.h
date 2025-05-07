#pragma once

namespace Engine
{
	typedef struct tagEngineDesc 
	{
		HINSTANCE		hInstance;
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

	typedef struct tagVertexCubeTexture {
		XMFLOAT3		vPosition;
		XMFLOAT3		vTextureCoord;
	}VTXCUBETEX;
#pragma endregion


	typedef struct tagParticleAttribute
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vVelocity;
		XMFLOAT3		vAcceleration;
		float			fSize;
		float			fLifetime;
		float			fAge;
		XMFLOAT4		vCurrentColor;
		XMFLOAT4		vInitialColor;
		XMFLOAT4		vColorFade;
		bool			bIsAlive;
		unsigned int	iIndex;
		XMFLOAT3		vDir;
		float fFrameProgress = 0.0f;
	} ATTRIBUTE;

	



	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = {2};

		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

}