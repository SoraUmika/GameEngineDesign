#ifndef COMPONENTSYSTEM_H
#define COMPONENTSYSTEM_H
#include <SYSTEM.h>
#include <typeindex>
#include <unordered_map>
#include <cassert>
#include <functional>
#include <memory>
#include <Components.h>
#include <JSON/json.hpp>

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
                Remove_Data(entity);
            }
        }
        typename std::unordered_map<Entity, size_t>::iterator begin() 
        {
            return entity_to_index_map.begin();
        }
        typename std::unordered_map<Entity, size_t>::iterator end() 
        {
            return entity_to_index_map.end();
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
        void Register_Component_Type(const std::string& componentType, std::function<void(Entity, const std::string&)> customBuilder=nullptr)
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            if(component_types.find(typeIndex) != component_types.end())
            {
                Logger::log(LogLevel::WARNING, "Registering component type %s falied, type has already been registered.", componentType.c_str());
            }
            else
            {
                auto it = std::find_if(component_names.begin(), component_names.end(), [componentType](const auto& pair) {
                    return pair.second == componentType;
                });
                if (it != component_names.end())
                {
                    Logger::log(LogLevel::WARNING, "Registering component type %s falied, type name has already been registered.", componentType.c_str());
                }
                else
                {
                    component_types.insert({typeIndex, next_component_type});
                    component_arrays.insert({typeIndex, std::make_shared<ComponentArray<T>>()});
                    component_names.insert({typeIndex, componentType});
                    std::function<void(Entity, const std::string&)> builder = [this](Entity entiy, const std::string& componentInfo="")
                    {
                        T compType;
                        if(!componentInfo.empty()){compType = T(componentInfo);}
                        this->Add_Component<T>(entiy, compType);
                    };
                    if(customBuilder != nullptr){builder = customBuilder;}
                    component_builders.insert({componentType, builder});
                    ++next_component_type;
                    Logger::log(LogLevel::INFO, "Component type %s registered", componentType.c_str());
                }
            }
        }

        template<typename T>
        ComponentType Get_Component_Type()
        {
            const std::type_index typeName = std::type_index(typeid(T));
            assert(component_types.find(typeName) != component_types.end() && "Component not registered before use.");
            return component_types[typeName];
        }

        template<typename T>
        std::string Get_Component_Name()
        {
            const std::type_index typeName = std::type_index(typeid(T));
            assert(component_types.find(typeName) != component_types.end() && "Component not registered before use.");
            return component_names[typeName];           
        }

        bool Has_Component_Type(const std::string& componentType)
        {
            if(component_builders.find(componentType) != component_builders.end()){return true;}
            else{return false;}
        }

        template<typename T>
        void Add_Component(Entity entity, T component)
        {
            Get_Component_Array<T>()->Insert_Data(entity, component);
            Logger::log(LogLevel::INFO, "Added component %s to entity %d", Get_Component_Name<T>().c_str(), entity);
        }

        void Add_Component_By_Name(Entity entity, const std::string& component, const std::string& componentInfo="")
        {
            if(component_builders.find(component) == component_builders.end())
            {
                Logger::log(LogLevel::WARNING, "Adding component %s by name failed, component name does not exists", component.c_str());
            }
            else
            {
                component_builders[component](entity,componentInfo);
            }       
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

        template<typename T>
        std::shared_ptr<ComponentArray<T>> Get_Component_Array()
        {
            std::type_index typeIndex = std::type_index(typeid(T));
            assert(component_types.find(typeIndex) != component_types.end() && "Component not registered before use.");
            return std::static_pointer_cast<ComponentArray<T>>(component_arrays[typeIndex]);
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
        std::unordered_map<std::string, std::function<void(Entity, const std::string&)>> component_builders;
        std::unordered_map<std::type_index, std::string> component_names;
        ComponentType next_component_type{};

};

#endif // !COMPONENT_H