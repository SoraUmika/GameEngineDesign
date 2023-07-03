#ifndef COMPONENTSYSTEM_H
#define COMPONENTSYSTEM_H
#include <typeindex>
#include <unordered_map>
#include <Logger.h>
#include <EntitySYSTEM.h>
#include <cassert>

using ComponentType = std::size_t;

class IComponentArray
{
    public:
        virtual ~IComponentArray() = default;
        virtual void Entity_Destroyed(Entity entity) = 0;
    private:
};

template <typename T>
class ComponentArray: public IComponentArray
{
    public:
        void Insert_Data(Entity entity, T component)
        {
            assert(entity_to_index_map.find(entity) == entity_to_index_map.end() && "Component added to same entity more than once.");
            size_t newIndex = size;
            entity_to_index_map[entity] = newIndex;
            index_to_entity_map[newIndex] = entity;
            component_array[newIndex] = component;
            ++size;
        }
        void Remove_Data(Entity entity)
        {
            assert(entity_to_index_map.find(entity) != entity_to_index_map.end() && "Removing non-existent component.");
            size_t indexOfRemovedEntity = entity_to_index_map[entity];
            size_t indexOfLastElement = size - 1;
            if (indexOfRemovedEntity != indexOfLastElement) {
                component_array[indexOfRemovedEntity] = component_array[indexOfLastElement];
                Entity entityOfLastElement = index_to_entity_map[indexOfLastElement];
                entity_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
                index_to_entity_map[indexOfRemovedEntity] = entityOfLastElement;
                index_to_entity_map.erase(indexOfLastElement);
            }
            entity_to_index_map.erase(entity);
            --size;
        }
        T& Get_Data(Entity entity)
        {
            assert(entity_to_index_map.find(entity) != entity_to_index_map.end() && "Retrieving non-existent component.");
            // Return the data
            return component_array[entity_to_index_map[entity]];
        }

        bool Has_Data(Entity entity)
        {
            return entity_to_index_map.find(entity) != entity_to_index_map.end();
        }

        void Entity_Destroyed(Entity entity) override
        {
            if (entity_to_index_map.find(entity) != entity_to_index_map.end())
            {
                // Remove the entity's component if it existed
                Remove_Data(entity);
            }
        }
    private:
        std::array<T, MAX_ENTITIES> component_array;
        std::unordered_map<Entity,size_t> entity_to_index_map;
        std::unordered_map<size_t, Entity> index_to_entity_map;
        size_t size{};
};

class ComponentSYSTEM
{
    public:
        template<typename T>
        void Register_Component_Type()
        {
            const std::type_index typeName = std::type_index(typeid(T));
            assert(component_types.find(typeName) == component_types.end() && "Registering component type more than once.");
            component_types.insert({typeName, next_component_type});
            component_arrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
            ++next_component_type;
            Logger::log(LogLevel::INFO, "Component type %s registered", typeid(T).name());
        }
        
        template<typename T>
        ComponentType Get_Component_Type()
        {
            std::type_index typeName = std::type_index(typeid(T));
            assert(component_types.find(typeName) != component_types.end() && "Component not registered before use.");
            // Return this component's type - used for creating signatures
            return component_types[typeName];
        }
        
        template<typename T>
        void Add_Component(Entity entity, T component)
        {
            Get_Component_Array<T>()->Insert_Data(entity, component);
            Logger::log(LogLevel::INFO, "Added component %s to entity %d", typeid(T).name(), entity);
        }
        
        template<typename T>
        void Remove_Component(Entity entity)
        {
            Get_Component_Array<T>()->Remove_Data(entity);
            Logger::log(LogLevel::INFO, "Removed component %s from entity %d", typeid(T).name(), entity);
        }

        template<typename T>
        T& Get_Component(Entity entity)
        {
            return Get_Component_Array<T>()->Get_Data(entity);
        }

        void Remove_All_Entity_Components(Entity entity)
        {
            for(auto const& pair: component_arrays)
            {
                auto const& component = pair.second;
                component->Entity_Destroyed(entity);
                Logger::log(LogLevel::INFO, "All components of entity %d are destroyed", entity);
            }
        }

        template<typename T>
        bool Entity_Has_Component(Entity entity)
        {
            return Get_Component_Array<T>()->Has_Data(entity);
        }

    private:
        std::unordered_map<std::type_index, ComponentType> component_types{};
        std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> component_arrays{};
        ComponentType next_component_type{};

        template<typename T>
        std::shared_ptr<ComponentArray<T>> Get_Component_Array()
        {
            std::type_index typeName = std::type_index(typeid(T));
            assert(component_types.find(typeName) != component_types.end() && "Component not registered before use.");
            return std::static_pointer_cast<ComponentArray<T>>(component_arrays[typeName]);
        }
};

#endif // !COMPONENT_H