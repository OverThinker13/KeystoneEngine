#pragma once
#include "keystone/core/Types.h"
#include <cmath>

namespace ks {

struct Vec3 {

    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;

    Vec3() = default;
    Vec3(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}

    // 加法
    Vec3 operator+(const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }

    // 减法
    Vec3 operator-(const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }

    // 数乘（成员版）
    Vec3 operator*(f32 s) const { return Vec3(x * s, y * s, z * s); }

    // 取负
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    // +=
    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    // -=
    Vec3& operator-=(const Vec3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    // *=
    Vec3& operator*=(f32 s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    // ==
    bool operator==(const Vec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
};

// 数乘,由于要使得 2.0f * v 这种形式也能用，所以要写在类外，写在vec里面会导致 2.0f * v
// 这种形式编译报错，提示找不到匹配的 operator*，因为2.0f是float类型，不是Vec3类型
// 因为写在类外，所以要加上inline，避免重复定义链接错误
inline Vec3 operator*(f32 s, const Vec3& v) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}

// 判断两个向量是否近似相等，允许有一个很小的误差eps,常用在图形学中判断两个浮点数是否相等，避免因为浮点数精度问题导致的误判
inline bool NearlyEqual(const Vec3& a, const Vec3& b, f32 eps = 1e-5f) {
    return std::fabs(a.x - b.x) <= eps && std::fabs(a.y - b.y) <= eps &&
           std::fabs(a.z - b.z) <= eps;
}

// Dot
inline f32 Dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cross，顺序忘了可以用行列式展开ijk，axayaz， bxbybz，展开后就是叉乘公式
inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 计算向量长度平方
inline f32 LengthSquared(const Vec3& v) {
    return Dot(v, v);
}

// 计算向量长度
inline f32 Length(const Vec3& v) {
    return std::sqrt(LengthSquared(v));
}

// 归一化normalize
inline Vec3 Normalize(const Vec3& v) {
    f32 len = Length(v);
    // 用一个极小的阈值来判断长度是否为零，避免除以零导致的NaN
    if (len <= 1e-8f) {
        return Vec3(0.0f, 0.0f, 0.0f); // 零向量返回零，不产生 NaN
    }
    return Vec3(v.x / len, v.y / len, v.z / len);
}

} // namespace ks