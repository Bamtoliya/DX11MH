#pragma once

namespace Engine
{
#pragma region EngineDesc
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;
#pragma endregion

#pragma region PhysxDebug

	typedef struct tagPhysXDebugLine
	{
		XMFLOAT3 vP0;
		XMFLOAT3 vP1;
		XMFLOAT4 vColor;
	} DEBUG_LINE;

	typedef struct tagPhysXDebugTriangle
	{
		XMFLOAT3 vP0;
		XMFLOAT3 vP1;
		XMFLOAT3 vP2;
		XMFLOAT4 vColor;
	} DEBUG_TRIANGLE;
#pragma endregion

#pragma region Animation
	typedef struct tagKeyFrame
	{
		/* 행렬이 아닌이유? : 상태와 상태 사이를 보간해주기위해서. */
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;

		/* 이 상태를 취해야하는 재생위치  */
		float			fTrackPosition;
	}KEYFRAME;

	typedef struct ENGINE_DLL BlendParameters
	{
		float fSpeed = 0.f;
		float fDirection = 0.f; // 예: 2D 블렌드를 위한 각도 (라디안)
		bool  bIsJumping = false;
		bool  bIsAttacking = false;
		// 기타 게임 상태에 따른 파라미터 추가 가능
		map<const wstring, float> customParamsFloat;
		map<const wstring, int>   customParamsInt;
		map<const wstring, bool>  customParamsBool;

		// 특정 파라미터 값을 가져오는 헬퍼 함수 (옵션)
		bool GetBoolParam(const wstring& name, bool defaultValue = false) const {
			auto it = customParamsBool.find(name);
			return (it != customParamsBool.end()) ? it->second : defaultValue;
		}
		float GetFloatParam(const wstring& name, float defaultValue = 0.f) const {
			auto it = customParamsFloat.find(name);
			return (it != customParamsFloat.end()) ? it->second : defaultValue;
		}
		int GetIntParam(const wstring& name, int defaultValue = 0) const {
			auto it = customParamsInt.find(name);
			return (it != customParamsInt.end()) ? it->second : defaultValue;
		}
	} BLEND_PARAM;
#pragma endregion

#pragma region Light
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE				eType;
		XMFLOAT4			vDirection;
		XMFLOAT4			vPosition;
		float				fRange;

		XMFLOAT4			vDiffuse;
		XMFLOAT4			vAmbient;
		XMFLOAT4			vSpecular;
	}LIGHT_DESC;
#pragma endregion

#pragma region VTX Structures
	/* 화면에 그려야하는 최종적인 색 = Light.Diffuse * 재질.Diffuse */
	typedef struct ENGINE_DLL tagVertexPosition
	{
		XMFLOAT3		vPosition;		

		static const unsigned int					iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;		
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexDebugDraw
	{
		XMFLOAT3 vPosition;
		XMFLOAT4 vColor;

		static const unsigned int					iNumElements = { 2 };

		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	} VTXDEBUGDRAW;

	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = {2};

		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexCubePositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int					iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBETEX;

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
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 5 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexSplineModel
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT3	vTangent;
		XMFLOAT3	vBinormal;
		XMFLOAT2	vTexcoord;

		static const unsigned int					iNumElements = { 5 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	} VTXSPLINEMODEL;

	typedef struct ENGINE_DLL tagVertexSplinePoint
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT3	vTangent;
		XMFLOAT3	vBinormal;

		static const unsigned int					iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	} VTXSPLINEPOINT;

	typedef struct ENGINE_DLL tagVertexSplineModelInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		static const unsigned int					iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	} VTXSPLINEMODEL_INSTANCE;

	typedef struct ENGINE_DLL tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int					iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertexMeshInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;		

		static const unsigned int					iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH_INSTANCE;

	typedef struct ENGINE_DLL tagVertexRectParticleInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;		
		XMFLOAT2		vLifeTime;		

		static const unsigned int					iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXRECT_PARTICLE_INSTANCE;

	typedef struct ENGINE_DLL tagVertexPointParticleInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;

		static const unsigned int					iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS_PARTICLE_INSTANCE;
#pragma endregion
}