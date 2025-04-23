# JusinDX11

차명훈 개인 DX11 레포지토리

## 주의사항
    주의할점 리소스 사용할때 bin/Resource사용을 피하기
    생성된 Bin 폴더는 무시되기 때문에 리소스를 직접 추가 해줘야함
    현재 수정하고 있는 헤더가 EngineSDK 인지 Engine인지 확인하기

## 자동 dll 생성 배치파일 실행
엔진 속성에 빌드후 이벤트에
```
CD "$(SolutionDir)"
CALL "$(SolutionDir)\UpdateLib.bat"
CALL "$(SolutionDir)\UpdateFMODLib.bat"
```
적어주세요

## 빌드 속도 높이기
### Unity 빌드 사용시
한글 디렉토리를 포함하면 작동 못함.

imGui 사용,
imGizmo 사용,
유니티 빌드 사용,
다중프로세서 컴파일 사용중,