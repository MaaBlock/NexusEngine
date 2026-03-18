#pragma once
// ============================================================================
// NxURDF — 零依赖、面向数据的 URDF 解析库
//
// 设计原则:
//   1. Zero-dependency: XML 解析器 (tinyxml2) 作为私有依赖，绝不暴露
//   2. Modern C++17: std::optional / std::string_view / structured bindings
//   3. Math-Agnostic: 仅使用 std::array<double, N>，不引入任何数学库
//   4. Data-Oriented: 扁平 struct，无继承树，直接可读
//
// 用法:
//   auto result = NxURDF::parse("robot.urdf");
//   if (result) {
//       const auto& model = *result;
//       for (const auto& link : model.links) { ... }
//   }
// ============================================================================

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace NxURDF {

// ── 基础类型别名 ──────────────────────────────────────────────────────────
using Vec3  = std::array<double, 3>;
using Vec4  = std::array<double, 4>;      // RGBA 或四元数
using Mat3  = std::array<double, 6>;      // ixx, ixy, ixz, iyy, iyz, izz

// ── 空间姿态 ─────────────────────────────────────────────────────────────
struct Pose {
    Vec3 xyz{};                            // 平移
    Vec3 rpy{};                            // Roll-Pitch-Yaw (弧度)
};

// ── 几何描述 ─────────────────────────────────────────────────────────────
enum class GeometryType : uint8_t {
    None, Box, Cylinder, Sphere, Mesh
};

struct Geometry {
    GeometryType type = GeometryType::None;
    Vec3         size{};                   // Box: (x,y,z)
    double       radius  = 0.0;           // Sphere / Cylinder
    double       length  = 0.0;           // Cylinder
    std::string  meshFilename;             // Mesh 文件路径 (原始 package:// URI)
    Vec3         meshScale{1.0, 1.0, 1.0};
};

// ── 材质 ─────────────────────────────────────────────────────────────────
struct Material {
    std::string name;
    Vec4 color{1.0, 1.0, 1.0, 1.0};       // RGBA
    std::string textureFilename;           // 纹理路径（可选）
};

// ── Visual / Collision ──────────────────────────────────────────────────
struct Visual {
    std::string            name;
    Pose                   origin;
    Geometry               geometry;
    std::optional<Material> material;
};

struct Collision {
    std::string name;
    Pose        origin;
    Geometry    geometry;
};

// ── 惯性参数 ────────────────────────────────────────────────────────────
struct Inertial {
    Pose   origin;
    double mass = 0.0;
    Mat3   inertia{};                      // ixx, ixy, ixz, iyy, iyz, izz
};

// ── Link ────────────────────────────────────────────────────────────────
struct Link {
    std::string              name;
    std::optional<Inertial>  inertial;
    std::vector<Visual>      visuals;      // 一个 Link 可以有多个 Visual
    std::vector<Collision>   collisions;   // 一个 Link 可以有多个 Collision
};

// ── Joint 类型 ──────────────────────────────────────────────────────────
enum class JointType : uint8_t {
    Fixed, Revolute, Continuous, Prismatic, Floating, Planar
};

struct JointLimits {
    double lower    = 0.0;
    double upper    = 0.0;
    double effort   = 0.0;
    double velocity = 0.0;
};

struct JointDynamics {
    double damping  = 0.0;
    double friction = 0.0;
};

struct Joint {
    std::string                  name;
    JointType                    type = JointType::Fixed;
    std::string                  parentLink;
    std::string                  childLink;
    Pose                         origin;
    Vec3                         axis{0.0, 0.0, 1.0};
    std::optional<JointLimits>   limits;
    std::optional<JointDynamics> dynamics;
};

// ── 完整 URDF 模型 ─────────────────────────────────────────────────────
struct Model {
    std::string          name;
    std::vector<Link>    links;
    std::vector<Joint>   joints;

    // 便捷索引 (name -> vector index)
    std::unordered_map<std::string, size_t> linkIndex;
    std::unordered_map<std::string, size_t> jointIndex;

    // 查找辅助
    const Link*  findLink(std::string_view name) const;
    const Joint* findJoint(std::string_view name) const;
    std::string  rootLinkName() const;
};

// ── 解析 API ────────────────────────────────────────────────────────────

/// 从文件路径解析 URDF，失败返回 std::nullopt
std::optional<Model> parseFile(std::string_view filePath);

/// 从内存中的 XML 字符串解析 URDF，失败返回 std::nullopt
std::optional<Model> parseString(std::string_view xmlContent);

/// 将 package:// URI 解析为实际文件路径
/// 例如 "package://go2_description/dae/base.dae" → basePath + "/dae/base.dae"
std::string resolveMeshPath(std::string_view uri, std::string_view packageBasePath);

} // namespace NxURDF
