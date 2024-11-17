/**************************************************************************
 * Flakkari Engine Library v0.4.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file EntityFactory.hpp
 * @brief Flakkari::Engine::ECS::EntityFactory class header. This class is
 *        used to create an entity from a template. It is used to create
 *        entities from the server or from the client.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.4.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef FLAKKARI_ENTITYFACTORY_HPP_
#define FLAKKARI_ENTITYFACTORY_HPP_

#include <nlohmann/json.hpp>

#include "Registry.hpp"

#include "Components/Components2D.hpp"
#include "Components/Components3D.hpp"
#include "Components/ComponentsCommon.hpp"

namespace Flakkari::Engine::ECS {

class EntityFactory {
public:
    using nl_template = nlohmann::json;

public:
    /**
     * @brief Create a Entity From Template object based on a template JSON
     *
     * @details This function will create an entity based on the template JSON
     *        (using the template name)
     *
     * @see ResourceManager::getTemplateById
     *
     * @param registry  The registry to add the entity to
     * @param templateJson  The template JSON
     * @return Entity  The created entity
     */
    static Entity createEntityFromTemplate(Registry &registry, const nl_template &templateJson)
    {
        Entity entity = registry.spawn_entity();

        addEntityToRegistryByTemplate(registry, entity, templateJson);

        return entity;
    }

    /**
     * @brief Add an entity to the registry based on a template JSON
     *
     * @details This function will add all the components to the entity
     *         based on the template JSON (using the template name)
     *
     * @see ResourceManager::getTemplateById
     *
     * @param registry  The registry to add the entity to
     * @param entity  The entity to add the components to
     * @param templateJson  The template JSON
     */
    static void addEntityToRegistryByTemplate(Registry &registry, Entity entity, const nl_template &templateJson)
    {
        for (auto &component : templateJson.items())
        {
            auto componentName = component.key();
            auto componentContent = component.value();

            //*_ 2D Components _*//

            if (componentName == "Collider")
            {
                registry.registerComponent<Engine::ECS::Components::_2D::Collider>();
                Engine::ECS::Components::_2D::Collider collider;
                collider._size = Engine::Math::Vector2f(componentContent["size"]["x"], componentContent["size"]["y"]);
                registry.add_component<Engine::ECS::Components::_2D::Collider>(entity, std::move(collider));
                continue;
            }

            if (componentName == "Control")
            {
                registry.registerComponent<Engine::ECS::Components::_2D::Control>();
                Engine::ECS::Components::_2D::Control control;
                control._up = componentContent["up"];
                control._down = componentContent["down"];
                control._left = componentContent["left"];
                control._right = componentContent["right"];
                control._shoot = componentContent["shoot"];
                registry.add_component<Engine::ECS::Components::_2D::Control>(entity, std::move(control));
                continue;
            }

            if (componentName == "Movable")
            {
                registry.registerComponent<Engine::ECS::Components::_2D::Movable>();
                Engine::ECS::Components::_2D::Movable movable;
                movable._velocity =
                    Engine::Math::Vector2f(componentContent["velocity"]["x"], componentContent["velocity"]["y"]);
                movable._acceleration = Engine::Math::Vector2f(componentContent["acceleration"]["x"],
                                                               componentContent["acceleration"]["y"]);
                registry.add_component<Engine::ECS::Components::_2D::Movable>(entity, std::move(movable));
                continue;
            }

            if (componentName == "RigidBody")
            {
                registry.registerComponent<Engine::ECS::Components::_2D::RigidBody>();
                Engine::ECS::Components::_2D::RigidBody rigidBody;
                rigidBody._mass = componentContent["mass"];
                rigidBody._restitution = componentContent["restitution"];
                rigidBody._friction = componentContent["friction"];
                rigidBody._gravityScale = componentContent["gravityScale"];
                rigidBody._isGravityAffected = componentContent["isGravityAffected"];
                rigidBody._isKinematic = componentContent["isKinematic"];
                registry.add_component<Engine::ECS::Components::_2D::RigidBody>(entity, std::move(rigidBody));
                continue;
            }

            if (componentName == "Transform")
            {
                registry.registerComponent<Engine::ECS::Components::_2D::Transform>();
                Engine::ECS::Components::_2D::Transform transform;
                transform._position =
                    Engine::Math::Vector2f(componentContent["position"]["x"], componentContent["position"]["y"]);
                transform._rotation = componentContent["rotation"];
                transform._scale =
                    Engine::Math::Vector2f(componentContent["scale"]["x"], componentContent["scale"]["y"]);
                registry.add_component<Engine::ECS::Components::_2D::Transform>(entity, std::move(transform));
                continue;
            }

            //*_ 3D Components _*//

            if (componentName == "BoxCollider")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::BoxCollider>();
                Engine::ECS::Components::_3D::BoxCollider boxCollider;
                boxCollider._size = Engine::Math::Vector3f(componentContent["size"]["x"], componentContent["size"]["y"],
                                                           componentContent["size"]["z"]);
                boxCollider._center = Engine::Math::Vector3f(
                    componentContent["center"]["x"], componentContent["center"]["y"], componentContent["center"]["z"]);
                registry.add_component<Engine::ECS::Components::_3D::BoxCollider>(entity, std::move(boxCollider));
                continue;
            }

            if (componentName == "3D_Control")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::Control>();
                Engine::ECS::Components::_3D::Control control;
                control._move_up = componentContent["move_up"];
                control._move_down = componentContent["move_down"];
                control._move_left = componentContent["move_left"];
                control._move_right = componentContent["move_right"];
                control._move_front = componentContent["move_front"];
                control._move_back = componentContent["move_back"];
                control._look_up = componentContent["look_up"];
                control._look_down = componentContent["look_down"];
                control._look_left = componentContent["look_left"];
                control._look_right = componentContent["look_right"];
                control._shoot = componentContent["shoot"];
                registry.add_component<Engine::ECS::Components::_3D::Control>(entity, std::move(control));
                continue;
            }

            if (componentName == "3D_Movable")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::Movable>();
                Engine::ECS::Components::_3D::Movable movable;
                movable._velocity =
                    Engine::Math::Vector3f(componentContent["velocity"]["x"], componentContent["velocity"]["y"],
                                           componentContent["velocity"]["z"]);
                movable._acceleration =
                    Engine::Math::Vector3f(componentContent["acceleration"]["x"], componentContent["acceleration"]["y"],
                                           componentContent["acceleration"]["z"]);
                registry.add_component<Engine::ECS::Components::_3D::Movable>(entity, std::move(movable));
                continue;
            }

            if (componentName == "RigidBody")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::RigidBody>();
                Engine::ECS::Components::_3D::RigidBody rigidBody;
                rigidBody._mass = componentContent["mass"];
                rigidBody._drag = componentContent["drag"];
                rigidBody._angularDrag = componentContent["angularDrag"];
                rigidBody._useGravity = componentContent["useGravity"];
                rigidBody._isKinematic = componentContent["isKinematic"];
                registry.add_component<Engine::ECS::Components::_3D::RigidBody>(entity, std::move(rigidBody));
                continue;
            }

            if (componentName == "SphereCollider")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::SphereCollider>();
                Engine::ECS::Components::_3D::SphereCollider sphereCollider;
                sphereCollider._center = Engine::Math::Vector3f(
                    componentContent["center"]["x"], componentContent["center"]["y"], componentContent["center"]["z"]);
                sphereCollider._radius = componentContent["radius"];
                registry.add_component<Engine::ECS::Components::_3D::SphereCollider>(entity, std::move(sphereCollider));
                continue;
            }

            if (componentName == "3D_Transform")
            {
                registry.registerComponent<Engine::ECS::Components::_3D::Transform>();
                Engine::ECS::Components::_3D::Transform transform;
                transform._position =
                    Engine::Math::Vector3f(componentContent["position"]["x"], componentContent["position"]["y"],
                                           componentContent["position"]["z"]);
                transform._rotation =
                    Engine::Math::Vector3f(componentContent["rotation"]["x"], componentContent["rotation"]["y"],
                                           componentContent["rotation"]["z"]);
                transform._scale = Engine::Math::Vector3f(
                    componentContent["scale"]["x"], componentContent["scale"]["y"], componentContent["scale"]["z"]);
                registry.add_component<Engine::ECS::Components::_3D::Transform>(entity, std::move(transform));
                continue;
            }

            //*_ Common Components _*//

            if (componentName == "Child")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Child>();
                Engine::ECS::Components::Common::Child child(componentContent["name"]);
                registry.add_component<Engine::ECS::Components::Common::Child>(entity, std::move(child));
                continue;
            }

            if (componentName == "Evolve")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Evolve>();
                Engine::ECS::Components::Common::Evolve evolve(componentContent["name"]);
                registry.add_component<Engine::ECS::Components::Common::Evolve>(entity, std::move(evolve));
                continue;
            }

            if (componentName == "Health")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Health>();
                Engine::ECS::Components::Common::Health health;
                health.maxHealth = componentContent["maxHealth"];
                health.currentHealth = componentContent["currentHealth"];
                health.maxShield = componentContent["maxShield"];
                health.shield = componentContent["shield"];
                registry.add_component<Engine::ECS::Components::Common::Health>(entity, std::move(health));
                continue;
            }

            if (componentName == "Parent")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Parent>();
                Engine::ECS::Components::Common::Parent parent(componentContent["entity"]);
                registry.add_component<Engine::ECS::Components::Common::Parent>(entity, std::move(parent));
                continue;
            }

            if (componentName == "Level")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Level>();
                Engine::ECS::Components::Common::Level level;
                level.level = componentContent["level"];
                level.currentExp = componentContent["currentExp"];
                level.requiredExp = componentContent["requiredExp"];
                level.currentWeapon = componentContent["currentWeapon"].get<std::string>().c_str();
                registry.add_component<Engine::ECS::Components::Common::Level>(entity, std::move(level));
                continue;
            }

            if (componentName == "Spawned")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Spawned>();
                Engine::ECS::Components::Common::Spawned spawned(componentContent["has_spawned"]);
                registry.add_component<Engine::ECS::Components::Common::Spawned>(entity, std::move(spawned));
                continue;
            }

            if (componentName == "Tag")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Tag>();
                Engine::ECS::Components::Common::Tag tag(componentContent["tag"]);
                registry.add_component<Engine::ECS::Components::Common::Tag>(entity, std::move(tag));
                continue;
            }

            if (componentName == "Template")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Template>();
                Engine::ECS::Components::Common::Template template_(componentContent["name"]);
                registry.add_component<Engine::ECS::Components::Common::Template>(entity, std::move(template_));
                continue;
            }

            if (componentName == "Weapon")
            {
                registry.registerComponent<Engine::ECS::Components::Common::Weapon>();
                Engine::ECS::Components::Common::Weapon weapon;
                weapon.damage = componentContent["damage"];
                weapon.fireRate = componentContent["fireRate"];
                weapon.level = componentContent["level"];
                registry.add_component<Engine::ECS::Components::Common::Weapon>(entity, std::move(weapon));
                continue;
            }
        }
    }
};

} // namespace Flakkari::Engine::ECS

#endif /* !FLAKKARI_ENTITYFACTORY_HPP_ */
