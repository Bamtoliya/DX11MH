#pragma once
/* 직교투영, 카메라, 객체만듬, 후처리 쉐이딩, 블러, 그림자, 소프트이펙트,
픽셀피킹, 모델 로딩, 애니메이팅, 파티클, 인스턴싱, 스프레팅쉐이더 */

/* 특정 헤더파일에 소속되지 않는 전역정보들을 선언해두는 역활 */
/* 기타 다른 정의파일(.h)들을 인클루드하는 역활. */
#include "../Default/framework.h"
#include <process.h>
#include "Editor_Enum.h"

namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;
}

extern unsigned int iWinResizeX;
extern unsigned int iWinResizeY;
extern bool g_SwapChainNeedsResize;
extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Editor;