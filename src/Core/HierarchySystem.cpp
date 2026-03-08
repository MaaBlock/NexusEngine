#include "HierarchySystem.h"
#include "Components.h"

namespace Nexus {

void HierarchySystem::update(Registry& registry) {
    auto view = registry.view<TransformComponent>();
    
    // 找出所有根节点：要么没有 HierarchyComponent，要么 parent == null
    std::vector<entt::entity> roots;
    for (auto entity : view) {
        if (!registry.has<HierarchyComponent>(entity)) {
            roots.push_back(entity);
        } else {
            auto& hier = registry.get<HierarchyComponent>(entity);
            if (hier.parent == entt::null) {
                roots.push_back(entity);
            }
        }
    }

    // 从根节点开始 DFS 更新
    for (auto root : roots) {
        updateNode(registry, root, nullptr);
    }
}

void HierarchySystem::updateNode(Registry& registry, entt::entity entity, const std::array<float, 16>* parentWorldMatrix) {
    if (!registry.has<TransformComponent>(entity)) return;

    auto& transform = registry.get<TransformComponent>(entity);
    auto localMatrix = transform.computeLocalMatrix();

    if (parentWorldMatrix) {
        transform.worldMatrix = multiplyMat4(*parentWorldMatrix, localMatrix);
    } else {
        transform.worldMatrix = localMatrix;
    }

    // 递归更新子节点
    if (registry.has<HierarchyComponent>(entity)) {
        auto& hier = registry.get<HierarchyComponent>(entity);
        for (auto child : hier.children) {
            updateNode(registry, child, &transform.worldMatrix);
        }
    }
}

} // namespace Nexus
