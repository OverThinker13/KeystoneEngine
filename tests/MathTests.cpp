#include "Test.h"
#include "keystone/core/Math.h"
#include <cmath>

void RunMathTests() {
    using ks::Vec3;

    // 0.加法
    KS_TEST((Vec3{1, 2, 3} + Vec3{4, 5, 6}) == Vec3{5, 7, 9});
    // 1. 减法
    KS_TEST((Vec3{4, 5, 6} - Vec3{1, 2, 3}) == Vec3{3, 3, 3});
    // 2. 数乘：v * 2.0f 和 2.0f * v 都要测
    KS_TEST((Vec3{1, 2, 3} * 2.0f) == Vec3{2, 4, 6});
    KS_TEST((2.0f * Vec3{1, 2, 3}) == Vec3{2, 4, 6});
    // 3. 取负 -v
    KS_TEST((-Vec3{1, 2, 3}) == Vec3{-1, -2, -3});
    // 4. += 和 -=
    KS_TEST((Vec3{1, 2, 3} += Vec3{4, 5, 6}) == Vec3{5, 7, 9});
    KS_TEST((Vec3{5, 7, 9} -= Vec3{4, 5, 6}) == Vec3{1, 2, 3});
    // 5. == 和 !=
    KS_TEST((Vec3{1, 2, 3} == Vec3{1, 2, 3}));
    KS_TEST((Vec3{1, 2, 3} != Vec3{4, 5, 6}));
    // 6. Dot：垂直（Dot({1,0,0},{0,1,0}) == 0）、平行（Dot({1,0,0},{2,0,0}) == 2）
    KS_TEST((ks::Dot(Vec3{1, 0, 0}, Vec3{0, 1, 0}) == 0.0f));
    KS_TEST((ks::Dot(Vec3{1, 0, 0}, Vec3{2, 0, 0}) == 2.0f));
    // 7. Cross：X×Y = Z、Y×Z = X、Z×X = Y  ← 第三组必须写
    KS_TEST((ks::Cross(Vec3{1, 0, 0}, Vec3{0, 1, 0}) == Vec3{0, 0, 1}));
    KS_TEST((ks::Cross(Vec3{0, 1, 0}, Vec3{0, 0, 1}) == Vec3{1, 0, 0}));
    KS_TEST((ks::Cross(Vec3{0, 0, 1}, Vec3{1, 0, 0}) == Vec3{0, 1, 0}));
    // 8. LengthSquared({3,4,0}) == 25、Length({3,4,0}) == 5
    KS_TEST((ks::LengthSquared(Vec3{3, 4, 0}) == 25.0f));
    KS_TEST((ks::Length(Vec3{3, 4, 0}) == 5.0f));
    // 9. Normalize 后 Length ≈ 1（用 KS_TEST_NEAR）
    KS_TEST_NEAR(ks::Length(ks::Normalize(Vec3{3, 4, 0})), 1.0f, 1e-5f);
    // 10. Normalize({0,0,0}) 不产生 NaN（用 std::isnan 检查三个分量都是 false）
    Vec3 z = ks::Normalize(Vec3{0, 0, 0});
    KS_TEST(!std::isnan(z.x));
    KS_TEST(!std::isnan(z.y));
    KS_TEST(!std::isnan(z.z));

    // 11. 默认构造必须是零向量
    //     回归测试：曾经把成员的 = 0.0f 删掉，导致 Vec3 v; 是垃圾值
    Vec3 d;
    KS_TEST(d == Vec3{0, 0, 0});

    // 12. 归一化后方向也要对（只测长度测不出方向错）
    KS_TEST(ks::NearlyEqual(ks::Normalize(Vec3{3, 4, 0}), Vec3{0.6f, 0.8f, 0.0f}));

    // 13. 叉积反交换律：a x b == -(b x a)
    KS_TEST(ks::NearlyEqual(ks::Cross(Vec3{1, 2, 3}, Vec3{4, 5, 6}),
                            -ks::Cross(Vec3{4, 5, 6}, Vec3{1, 2, 3})));

    // 14. 共线向量叉积为零（退化三角形会走到这条路径）
    KS_TEST(ks::Cross(Vec3{1, 2, 3}, Vec3{2, 4, 6}) == Vec3{0, 0, 0});
}