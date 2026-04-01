#pragma once

#include "thirdparty.h"
#include <Cesium3DTilesSelection/Tileset.h>
#include <CesiumGeospatial/LocalHorizontalCoordinateSystem.h>
#include <CesiumGltf/Model.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <optional>

namespace Nexus {

/**
 * @brief Cesium 全球参考坐标系组件 (Actor: ACesiumGeoreference)
 * 负责定义当前引擎世界坐标系原点所对应的真实地球经纬度（ECEF）
 */
struct CesiumGeoreference {
    // 地球经度 (Longitude)
    double m_longitude = 0.0;
    
    // 地球纬度 (Latitude)
    double m_latitude = 0.0;
    
    // 地表高度 (Height)
    double m_height = 0.0;
    
    // 缓存的局部坐标系统转换器，用于 ECEF 和 引擎局部坐标 之间的互转
    std::optional<CesiumGeospatial::LocalHorizontalCoordinateSystem> m_localCoordinateSystem;
};

/**
 * @brief Cesium 3D Tileset 数据集组件 (Actor: ACesium3DTileset)
 * 持有底层的 Tileset 实例并管理这套瓦片的生命周期
 */
struct Cesium3DTileset {
    // 指向核心算法库 Tileset 的唯一指针
    std::unique_ptr<Cesium3DTilesSelection::Tileset> m_tileset;
    
    // 配置的加载 URL（本地文件或网络地址）
    std::string m_url;
    
    // Ion 资产 ID（可选）
    int64_t m_ionAssetId = 0;
    
    // Ion 访问令牌（可选）
    std::string m_ionAccessToken;
    
    // 是否暂停更新视野
    bool m_suspendUpdate = false;
};

/**
 * @brief Cesium 实例化瓦片的网格组件载体 (Actor: UCesiumGltfComponent)
 * 挂载于代表具体被选中加载的瓦片实体之上，保存解码后的几何模型数据
 */
struct CesiumGltfComponent {
    // 所属的 Tileset 指针（非拥有，随父实体生命周期）
    Cesium3DTilesSelection::Tileset* m_parentTileset = nullptr;
    
    // 所属的 Cesium Native Raster/Gltf 模型数据，渲染后可销毁或保存用于碰撞
    const CesiumGltf::Model* m_gltfModel = nullptr;
    
    // 从 ECEF 换算到当前坐标系需要叠加的最后几何变换矩阵
    glm::dmat4 m_tileTransform = glm::dmat4(1.0);
};

} // namespace Nexus
