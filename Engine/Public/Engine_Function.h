#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
			iRefCnt = Instance->AddRef();

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
		{
			iRefCnt = Instance->Release();
			if (0 == iRefCnt)
				Instance = nullptr;
		}
		return iRefCnt;
	}


	static string WideToUtf8(const wchar_t* wstr)
	{
		if (!wstr) return "";

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		std::string strUtf8(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strUtf8[0], size_needed, NULL, NULL);
		strUtf8.resize(strUtf8.length() - 1); // 널 종료자 제거

		return strUtf8;
	}

	// UTF-8을 와이드 문자열로 변환 (반대 방향 변환도 필요함)
	static wstring Utf8ToWide(const string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
		wstrTo.resize(wstrTo.length() - 1); // 널 종료자 제거

		return wstrTo;
	}


	static FMOD_VECTOR VecToFMOD(XMVECTOR in)
	{
		// +x = 오른쪽 +y는 윗쪽 , +z 는 전방
		// FMOD (+z 전방, +x 오른쪽, +y 위쪽) 좌표로 변환
		XMFLOAT3 tmp = {};
		XMStoreFloat3(&tmp, in);
		return FMOD_VECTOR{ tmp.x, tmp.y, tmp.z };
	}

	static bool EndsWith(const wstring& str, const wstring& suffix) {
		if (str.length() < suffix.length()) {
			return false;
		}
		return equal(suffix.rbegin(), suffix.rend(), str.rbegin());
	}

	static unsigned int ConvertColorToId(XMFLOAT4 color) {
		// 부동소수점 값을 [0, 255] 범위의 바이트 값으로 변환
		// 값의 범위를 벗어나지 않도록 clamp 처리도 고려할 수 있습니다.
		// r = std::max(0.0f, std::min(1.0f, r)); (필요시)
		unsigned char r_byte = static_cast<unsigned char>(color.x * 255.0f);
		unsigned char g_byte = static_cast<unsigned char>(color.y * 255.0f);
		unsigned char b_byte = static_cast<unsigned char>(color.z * 255.0f);

		// 바이트 값을 조합하여 ID 복원
		return (static_cast<unsigned int>(r_byte) << 16) |
			(static_cast<unsigned int>(g_byte) << 8) |
			static_cast<unsigned int>(b_byte);
	}

	static string FloatToString(float value, int precision) {
		ostringstream oss;
		oss << std::fixed << std::setprecision(precision) << value;
		return oss.str();
	}

	static float lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	// 파일 존재 여부 확인 함수 (헬퍼 함수)
	static bool DoesFileExist(const wchar_t* filePath) {
		ifstream file(filePath);
		return file.good();
	}
}
