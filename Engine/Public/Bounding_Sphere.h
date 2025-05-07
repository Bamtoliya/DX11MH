#pragma once
#include "Base.h"          // DirectXMath.h 또는 관련 헤더가 포함되어 있다고 가정
#include "Bounding_Box.h"  // tagBoundingBox 구조체가 정의되어 있다고 가정 (XMVECTOR 사용 권장)

// 만약 Bounding_Box.h 가 이 파일을 포함하거나, 그 반대의 경우 전방 선언 필요
// struct tagBoundingBox; // 예시

NS_BEGIN(Engine)

// 구 형태의 경계 볼륨을 정의하는 구조체 (DirectXMath XMVECTOR 사용 버전)
// 참고: XMVECTOR 멤버는 16바이트 정렬이 필요할 수 있습니다. 클래스/구조체 할당 시 주의해야 합니다.
//      std::vector<BOUNDINGSPHERE> 같은 컨테이너 사용 시 커스텀 얼로케이터가 필요할 수 있습니다.
typedef struct alignas(16) tagBoundingSphere // C++11 alignas 키워드로 정렬 보장 시도
{
    XMVECTOR m_vCenter; // 구의 중심점 좌표 (XMVECTOR) - W 컴포넌트는 보통 1.0f 로 사용
    float             m_fRadius; // 구의 반지름

    // 기본 생성자: 중심 (0,0,0,1), 반지름 0으로 초기화
    tagBoundingSphere() noexcept // noexcept 추가 권장
        : m_vCenter(XMVectorSet(0.f, 0.f, 0.f, 1.f)), m_fRadius(0.0f) {
    }

    // 중심(XMVECTOR)과 반지름을 지정하는 생성자
    // XMVECTOR 파라미터는 규칙에 따라 FXMVECTOR, GXMVECTOR, HXMVECTOR, CXMVECTOR 사용
    tagBoundingSphere(FXMVECTOR center, float radius) noexcept
        : m_vCenter(center), m_fRadius(radius)
    {
        // 중심점 W 컴포넌트를 1.0f로 설정하는 것이 일반적 (행렬 변환 시 이동에 영향)
        // m_vCenter = XMVectorSetW(m_vCenter, 1.0f); // 필요에 따라 주석 해제
    }

    // 중심(_float3)과 반지름을 지정하는 생성자 (기존 코드 호환성 및 편의성)
    tagBoundingSphere(const _float3& center, float radius) noexcept
        : m_fRadius(radius)
    {
        // _float3를 XMVECTOR로 로드 (XMLoadFloat3는 W를 0.0f로 설정함)
        m_vCenter = XMLoadFloat3(&center);
        // 구의 중심은 점이므로 W 컴포넌트를 1.0f로 설정
        m_vCenter = XMVectorSetW(m_vCenter, 1.0f);
    }

    /**
     * @brief 주어진 점이 구 내부에 있는지 확인합니다 (XMVECTOR 사용).
     * @param Point 확인할 점의 좌표 (XMVECTOR, W=1.0f 권장)
     * @return 점이 구 내부에 있으면 true, 그렇지 않으면 false
     */
    bool Is_Point_Inside(FXMVECTOR Point) const noexcept // FXMVECTOR 사용, noexcept 추가
    {
        // 중심점에서 주어진 점까지의 벡터 계산 (Point와 m_vCenter 모두 XMVECTOR)
        // 점(W=1) - 점(W=1) = 벡터(W=0)
        XMVECTOR diff = XMVectorSubtract(Point, m_vCenter);

        // 벡터의 길이 제곱 계산 (결과는 모든 컴포넌트에 복제됨)
        // 3D 벡터 길이 계산이므로 XMVector3LengthSq 사용
        XMVECTOR distSqVec = XMVector3LengthSq(diff);

        // 길이 제곱 값을 float으로 추출 (첫 번째 컴포넌트(x)만 가져와도 동일)
        float distSq = XMVectorGetX(distSqVec);
        // 또는:
        // XMStoreFloat(&distSq, distSqVec);

        // 거리 제곱이 반지름 제곱보다 작거나 같으면 구 내부에 있음
        return distSq <= (m_fRadius * m_fRadius);

        /* --- DirectXMath 비교 함수 사용 (옵션) ---
           SIMD 최적화를 최대한 활용하려면 비교 결과도 벡터로 유지할 수 있으나,
           단순 bool 반환에는 float 비교가 더 직관적일 수 있습니다.

        // 반지름 제곱을 벡터 레지스터에 로드 (모든 컴포넌트에 복제)
        float fRadiusSq = m_fRadius * m_fRadius;
        XMVECTOR vRadiusSq = XMVectorReplicatePtr(&fRadiusSq); // 또는 XMVectorReplicate

        // XMVector3LessOrEqual 사용: distSqVec <= vRadiusSq 인지 비교
        // 이 함수는 비교 결과를 담은 컨트롤 벡터를 반환하며, 모든 관련 컴포넌트(x,y,z)가
        // 조건을 만족하는지 확인해야 함 (XMComparisonAllTrue 등 사용)
        // return XMComparisonAllTrue(XMVector3LessOrEqualR(distSqVec, vRadiusSq));
        // 또는 XMVector3LessOrEqual 결과 확인
        // if (XMVector3LessOrEqual(distSqVec, vRadiusSq)) { return true; } else { return false; }
        */
    }

    /**
     * @brief 주어진 점(_float3)이 구 내부에 있는지 확인합니다 (편의 오버로드).
     * @param p 확인할 점의 좌표 (_float3)
     * @return 점이 구 내부에 있으면 true, 그렇지 않으면 false
     */
    bool Is_Point_Inside(const _float3& p) const noexcept
    {
        // _float3를 XMVECTOR로 로드 (W는 0.0f로 로드됨)
        XMVECTOR Point = XMLoadFloat3(&p);
        // 점으로 사용하기 위해 W를 1.0f로 설정
        Point = XMVectorSetW(Point, 1.0f);
        // XMVECTOR 버전 함수 호출
        return Is_Point_Inside(Point);
    }

    // (선택 사항) AABB로부터 최소 경계 구를 계산하는 함수 (XMVECTOR 사용)
    // tagBoundingBox 구조체가 XMVECTOR 타입의 m_vMin, m_vMax를 가지고 있다고 가정
    // tagBoundingBox도 alignas(16)으로 정렬하는 것이 안전함
    static tagBoundingSphere CreateFromAABB(const tagBoundingBox& box) noexcept
    {
        // AABB의 중심 계산 (min + max) * 0.5f
        XMVECTOR center = XMVectorScale(XMVectorAdd(box.m_vMin, box.m_vMax), 0.5f);
        // 중심점 W는 1.0f로 설정
        center = XMVectorSetW(center, 1.0f);

        // AABB의 대각선 절반 벡터 계산 (max - min) * 0.5f
        XMVECTOR halfDiagonal = XMVectorScale(XMVectorSubtract(box.m_vMax, box.m_vMin), 0.5f);

        // 절반 대각선 벡터의 길이(3D)를 구의 반지름으로 설정
        // XMVector3Length는 결과 벡터의 모든 컴포넌트에 길이를 저장
        XMVECTOR radiusVec = XMVector3Length(halfDiagonal);

        // 반지름 값을 float으로 추출 (x 컴포넌트만 읽어도 됨)
        float radius = XMVectorGetX(radiusVec);
        // 또는:
        // XMStoreFloat(&radius, radiusVec);

        // 생성자 호출하여 반환
        return tagBoundingSphere(center, radius);
    }

    // 만약 tagBoundingBox가 _float3 멤버를 가진다면:
    /*
    static tagBoundingSphere CreateFromAABB_Float3(const tagBoundingBox_Float3& box) noexcept // _float3 멤버를 가진 박스 구조체 가정
    {
        // _float3를 XMVECTOR로 로드
        XMVECTOR vMin = XMLoadFloat3(&box.m_vMin);
        XMVECTOR vMax = XMLoadFloat3(&box.m_vMax);

        // 중심 계산
        XMVECTOR center = XMVectorScale(XMVectorAdd(vMin, vMax), 0.5f);
        center = XMVectorSetW(center, 1.0f); // W=1

        // 대각선 절반 벡터 계산
        XMVECTOR halfDiagonal = XMVectorScale(XMVectorSubtract(vMax, vMin), 0.5f);

        // 반지름 계산
        XMVECTOR radiusVec = XMVector3Length(halfDiagonal);
        float radius = XMVectorGetX(radiusVec);

        return tagBoundingSphere(center, radius);
    }
    */

} BOUNDINGSPHERE;

NS_END