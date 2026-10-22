# Sonic Frontiers 모작 — DX11MH

C++와 DirectX 11로 제작한 개인 게임 프로젝트입니다. Sonic Frontiers의 이동과 전투, 스테이지 기믹을 구현하고, 제작에 필요한 엔진 기능과 ImGui 편집 도구를 함께 개발했습니다.

- 개발 기간: 2025.04 ~ 2025.07
- 개발 인원: 1명
- 개발 환경: Windows, Visual Studio 2022, C++17, DirectX 11, HLSL
- 주요 라이브러리: PhysX, Assimp, FMOD, Dear ImGui, ImGuizmo, simdjson, nlohmann/json

주신아카데미의 [JusinDX11 프레임워크](https://github.com/Jusin151/JusinDX11)를 기반으로 진행한 프로젝트입니다. 아래에는 이 저장소에 구현된 게임 기능과 엔진·도구 구성을 정리했습니다.

## 주요 구현

### 캐릭터 이동과 전투

지상 이동, 공중 이동, 스핀, 벽 이동, 전투를 상태별 클래스로 나누었습니다. 컨트롤러가 입력과 캐릭터 상태를 관리하고, 각 상태에서 이동 속도와 애니메이션 전환을 처리합니다.

- 걷기·달리기·부스트, 점프와 경사면 이동
- 타깃 탐색과 호밍 어택, 콤보 입력 버퍼
- 스프링, 대시 패널, 대시 링 등 스테이지 오브젝트와의 상호작용
- 링 획득·차감과 적 충돌 처리

관련 코드: [Sonic_Controller.cpp](Editor/Private/Sonic_Controller.cpp), [Sonic_Movement_State.cpp](Editor/Private/Sonic_Movement_State.cpp), [Sonic_Combat_State.cpp](Editor/Private/Sonic_Combat_State.cpp)

### 렌더링과 애니메이션

DirectX 11 렌더러에서 그림자, 불투명 오브젝트, 조명, 최종 화면 합성, 반투명 오브젝트와 UI를 순서대로 처리합니다.

- 여러 렌더 타깃을 사용하는 Deferred Rendering
- 그림자 및 조명 패스, 디버그 콜라이더 표시
- 스켈레탈 애니메이션과 상태 전환 시 애니메이션 블렌딩
- 위치·크기 보간과 Quaternion Slerp를 이용한 회전 보간

관련 코드: [Renderer.cpp](Engine/Private/Renderer.cpp), [Animator.cpp](Engine/Private/Animator.cpp), [Channel.cpp](Engine/Private/Channel.cpp), [Shader_Deferred.hlsl](ShaderFiles/Shader_Deferred.hlsl)

### 물리와 제작 도구

PhysX 기반의 물리 처리와 ImGui 편집 화면을 연결했습니다. 오브젝트를 배치하고 속성을 수정한 뒤, 프로토타입과 레벨 오브젝트를 파일로 저장해 다시 불러올 수 있습니다.

- PhysX를 이용한 물리 시뮬레이션과 충돌 처리
- Hierarchy, Inspector, 2D·3D Viewport와 변환 기즈모
- JSON 기반 프로토타입·게임 오브젝트 저장 및 불러오기
- Assimp로 읽은 모델을 자체 바이너리 형식으로 변환하는 도구

관련 코드: [Physics_System.cpp](Engine/Private/Physics_System.cpp), [Hierarchy.cpp](Editor/Private/Hierarchy.cpp), [Inspector.cpp](Editor/Private/Inspector.cpp), [ToolBar.cpp](Editor/Private/ToolBar.cpp)

## 저장소 구조

- `Engine/`: 공통 엔진 코드. 렌더링, 물리, 모델·애니메이션, 입력, 리소스 및 오브젝트 관리
- `Editor/`: 편집 도구와 Sonic 게임 코드. 실제 플레이 구현도 이 프로젝트에 포함되어 있습니다.
- `Client/`: 별도의 클라이언트 프로젝트
- `ShaderFiles/`: HLSL 셰이더
- `Saves/`: 레벨과 프로토타입 저장 데이터
- `External Libraries/`: 외부 라이브러리
- `EngineSDK/`: 엔진 빌드 후 헤더와 라이브러리가 복사되는 폴더

## 빌드와 실행

현재 프로젝트에는 개발 당시의 리소스 경로와 라이브러리 설정이 남아 있습니다. 저장소를 복제한 뒤 아래 항목을 맞춰야 합니다.

### 준비 환경

1. Visual Studio 2022의 **C++를 사용한 데스크톱 개발**, MSVC v143, Windows 10 SDK를 설치합니다.
2. `Framework.sln`을 열고 **Debug / x64**를 선택합니다.
3. NuGet 패키지를 복원합니다. 프로젝트는 `Microsoft.GameInput`과 `directxtex_uwp` 패키지를 참조합니다.
4. `External Libraries/`와 `Engine/ThirdPartyLib/`의 헤더·LIB·DLL을 확인합니다. PhysX, FMOD, Wwise, Assimp, DirectXTK, Effects11 등의 경로와 링크 설정은 [PropertySheet.props](Engine/Default/PropertySheet.props) 및 각 `.vcxproj`에 있습니다. 누락된 파일은 해당 SDK와 프로젝트 설정에 맞게 별도로 준비해야 합니다.

### 리소스 경로

[Engine_Macro.h](Engine/public/Engine_Macro.h)의 `ASSETPATH`와 `BANKPATH`는 개발 당시의 `G:/My Drive/Assets/` 경로를 사용합니다. 로컬 에셋 및 FMOD 뱅크 위치에 맞게 수정해야 합니다.

[Renderer.cpp](Engine/Private/Renderer.cpp)의 `Shader_Deferred.hlsl` 로드 경로에도 절대 경로가 남아 있으므로 함께 확인합니다. 저장 데이터의 모델·텍스처 경로도 로컬 파일 위치와 일치해야 합니다. `.gitignore`에 `Assets` 제외 규칙이 있어, 저장소에 있는 리소스만으로 전체 플레이에 필요한 파일이 갖춰졌다고 보장할 수는 없습니다.

### 빌드 순서

1. `Engine` 프로젝트를 먼저 빌드합니다.
2. 엔진의 Debug / x64 빌드 후 이벤트가 [UpdateLib.bat](UpdateLib.bat)를 실행해 헤더·LIB를 `EngineSDK/`에, 엔진 및 일부 외부 DLL을 실행 폴더에 복사합니다.
3. FMOD DLL은 솔루션 루트에서 [UpdateFMODLib.bat](UpdateFMODLib.bat)를 실행해 복사합니다. Wwise 런타임을 사용할 경우 [UpdateWwiseLib.bat](UpdateWwiseLib.bat)의 경로와 복사 대상도 확인합니다.
4. `Editor` 프로젝트를 빌드하고 시작 프로젝트로 지정합니다. 프로젝트 속성의 **디버깅 → 작업 디렉터리**는 `$(ProjectDir)..\Bin`으로 설정합니다. `SOLUTIONPATH`가 실행 폴더 기준 `../../`를 사용하기 때문입니다.
5. 실행 후 로고 화면에서 `Enter`를 눌러 스테이지로 이동합니다.

`EngineSDK/`는 생성 결과이므로 엔진 소스 수정은 `Engine/`에서 진행합니다. 프로젝트의 Unity Build는 C++ 소스 파일을 묶어 컴파일하는 옵션이며, Unity 게임 엔진을 의미하지 않습니다. 기존 빌드 설정에서는 한글이 없는 경로를 사용합니다.

## 기본 조작

- `W / A / S / D`: 이동
- `Space`: 점프
- `Left Shift`: 부스트
- `O`: 편집 도구 툴바 표시 전환
- `Enter`: 로고 화면에서 스테이지 진입

추가 상태별 입력은 [Sonic_Movement_State.cpp](Editor/Private/Sonic_Movement_State.cpp)와 [Sonic_Combat_State.cpp](Editor/Private/Sonic_Combat_State.cpp)에서 확인할 수 있습니다.

## 제작 목적

게임 클라이언트, 렌더링, 물리, 제작 도구를 함께 다루기 위한 학습 및 포트폴리오 프로젝트입니다. 원작 게임의 명칭과 사용 리소스의 권리는 각 권리자에게 있습니다.
