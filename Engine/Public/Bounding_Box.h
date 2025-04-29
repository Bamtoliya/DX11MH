#pragma once
#include "Base.h"          // _float3 등 기본 타입 정의 가정

BEGIN(Engine)

// Axis-Aligned Bounding Box 구조체 (DirectXMath _vector 사용 버전)
// _vector 멤버를 가지므로 16바이트 정렬이 필요합니다.
typedef struct alignas(16) tagBoundingBox // C++11 alignas 키워드로 정렬 보장
{
    _vector m_vMin; // AABB의 최소점 (_vector)
    _vector m_vMax; // AABB의 최대점 (_vector)

    // 기본 생성자: Min을 양의 무한대로, Max를 음의 무한대로 초기화
    // 이는 점이나 다른 AABB를 병합하여 AABB를 점진적으로 구축할 때 유용합니다.
    tagBoundingBox() noexcept // noexcept 추가
        : m_vMin(XMVectorReplicate(std::numeric_limits<float>::infinity())) // 모든 컴포넌트에 +inf 복제
        , m_vMax(XMVectorReplicate(-std::numeric_limits<float>::infinity())) // 모든 컴포넌트에 -inf 복제
    {
        // 참고: AABB는 보통 x, y, z 컴포넌트만 의미가 있으며 W는 무시됩니다.
        // XMVectorReplicate는 W 컴포넌트에도 무한대 값을 설정합니다.
        // 필요하다면 W 컴포넌트를 특정 값(예: 0 또는 1)으로 설정할 수 있습니다.
        // m_vMin = XMVectorSetW(m_vMin, 0.0f);
        // m_vMax = XMVectorSetW(m_vMax, 0.0f);
    }

    // Min, Max (_vector)를 직접 지정하는 생성자
    // _vector 파라미터는 호출 규칙에 따라 F_vector 또는 C_vector 사용
    tagBoundingBox(_fvector vMin, _fvector vMax) noexcept
        : m_vMin(vMin), m_vMax(vMax) {
    }

    // Min, Max (_float3)를 지정하는 생성자 (기존 코드 호환성 및 편의성)
    tagBoundingBox(const _float3& vMin, const _float3& vMax) noexcept
    {
        m_vMin = XMLoadFloat3(&vMin); // _float3를 _vector로 로드 (W는 0.0f로 설정됨)
        m_vMax = XMLoadFloat3(&vMax); // _float3를 _vector로 로드 (W는 0.0f로 설정됨)
    }

    /**
     * @brief 주어진 점이 AABB 내부에 있는지 확인합니다 (_vector 사용).
     * @param Point 확인할 점의 좌표 (_vector). W 컴포넌트는 이 계산에서 무시됩니다.
     * @return 점이 AABB 내부에 있으면 true, 그렇지 않으면 false
     */
     // 메서드가 멤버 변수를 변경하지 않으므로 const 추가
    bool Is_Point_Inside(_fvector Point) const noexcept // const 및 noexcept 추가
    {
        // DirectXMath의 벡터 비교 함수를 사용합니다.
        // 각 컴포넌트별로 비교를 수행하고, 결과는 해당 컴포넌트에 마스크(true: 0xFFFFFFFF, false: 0x0)로 저장됩니다.

        // 1. Point >= m_vMin 검사 (모든 컴포넌트가 m_vMin보다 크거나 같은지)
        _vector vMinCompare = XMVectorGreaterOrEqual(Point, m_vMin);

        // 2. Point <= m_vMax 검사 (모든 컴포넌트가 m_vMax보다 작거나 같은지)
        _vector vMaxCompare = XMVectorLessOrEqual(Point, m_vMax);

        // 3. 두 비교 결과를 결합 (논리 AND)
        //    두 비교 결과 벡터에서 x, y, z 컴포넌트가 모두 0xFFFFFFFF (true)여야 함
        //    XMVectorAndInt는 두 벡터의 각 컴포넌트에 대해 비트 단위 AND 연산을 수행합니다.
        _vector vCombined = XMVectorAndInt(vMinCompare, vMaxCompare);

        // 4. 결합된 결과 벡터의 X, Y, Z 컴포넌트가 모두 true인지 확인
        //    XMVector3AllTrue 함수는 결과 벡터의 x, y, z 컴포넌트의 상위 비트를 검사하여
        //    모두 설정되어 있으면(즉, 모두 0xFFFFFFFF이면) true를 반환합니다.
        return Internal::XMVector3AllTrue(vCombined);
    }

    /**
     * @brief 주어진 점(_float3)이 AABB 내부에 있는지 확인합니다 (편의 오버로드).
     * @param p 확인할 점의 좌표 (_float3)
     * @return 점이 AABB 내부에 있으면 true, 그렇지 않으면 false
     */
     // 파라미터를 const 참조로 변경하는 것이 좋습니다. 멤버 변수를 변경하지 않으므로 메서드에 const 추가.
    bool Is_Point_Inside(const _float3& p) const noexcept // const 추가
    {
        // _float3를 _vector로 로드 (W는 0.0f로 로드됨)
        _vector Point = XMLoadFloat3(&p);

        // W 컴포넌트는 XMVector3AllTrue에서 고려되지 않으므로 별도로 설정할 필요 없음
        // _vector 버전 함수 호출
        return Is_Point_Inside(Point);
    }

    // --- 추가적으로 구현하면 유용한 함수들 (예시) ---

    /**
     * @brief AABB를 확장하여 주어진 점을 포함하도록 합니다.
     * @param Point 포함시킬 점 (_vector)
     */
    void Merge(_fvector Point) noexcept
    {
        m_vMin = XMVectorMin(m_vMin, Point); // 각 컴포넌트별 최소값 선택
        m_vMax = XMVectorMax(m_vMax, Point); // 각 컴포넌트별 최대값 선택
    }

    /**
     * @brief AABB를 확장하여 다른 AABB를 포함하도록 합니다.
     * @param OtherBox 포함시킬 다른 AABB
     */
    void Merge(const tagBoundingBox& OtherBox) noexcept
    {
        m_vMin = XMVectorMin(m_vMin, OtherBox.m_vMin);
        m_vMax = XMVectorMax(m_vMax, OtherBox.m_vMax);
    }

    /**
     * @brief 이 AABB가 다른 AABB와 교차하는지 확인합니다.
     * @param OtherBox 검사할 다른 AABB
     * @return 교차하면 true, 그렇지 않으면 false
     */
    bool Intersects(const tagBoundingBox& OtherBox) const noexcept
    {
        // 각 축에 대해 겹치는지 확인합니다. 한 축이라도 겹치지 않으면 교차하지 않습니다.
        // this.min <= other.max && this.max >= other.min

        // 1. this->m_vMin <= OtherBox.m_vMax (컴포넌트별 비교)
        _vector vMinLessEqualMax = XMVectorLessOrEqual(m_vMin, OtherBox.m_vMax);
        // 2. this->m_vMax >= OtherBox.m_vMin (컴포넌트별 비교)
        _vector vMaxGreaterEqualMin = XMVectorGreaterOrEqual(m_vMax, OtherBox.m_vMin);

        // 3. 두 조건을 모두 만족하는지 확인 (AND)
        _vector vCombined = XMVectorAndInt(vMinLessEqualMax, vMaxGreaterEqualMin);

        // 4. X, Y, Z 모든 축에서 조건을 만족하는지 확인
        return Internal::XMVector3AllTrue(vCombined);
    }


} BOUNDINGBOX; // 기존 코드와의 호환성을 위해 typedef 이름 유지

END