#ifndef COMPONENTSYSTEM_H
#define COMPONENTSYSTEM_H
#include <SYSTEM.h>
#include <typeindex>
#include <unordered_map>
#include <cassert>
#include <functional>
#include <memory>
#include <Components.h>
#include <vector>
#include <bitset>
#include <sol/sol.hpp>


using ComponentType = std::size_t;

class IComponentArray
{
    public:
        virtual ~IComponentArray() = default;
        virtual void Entity_Destroyed(Entity entity) = 0;
    protected:

    private:
};


template <typename T>
class ComponentArray: public IComponentArray
{
    public:
        ComponentArray(const std::string& name):component_name(name){}
        void Insert_Data(Entity entity, const T& component){
            assert(entity_to_index_map.find(entity) == entity_to_index_map.end() && "Component added to same entity more than once.");
            size_t newIndex = size;
            entity_to_index_map[entity] = newIndex;
            index_to_entity_map[newIndex] = entity;
            component_array[newIndex] = component;
            ++size;
            Logger::log(LogLevel::INFO, "Added component %s to entity %d", component_name.c_str(), entity);  
        }
        void Remove_Data(Entity entity){
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
            Logger::log(LogLevel::INFO, "Removed component %s from entity %d", component_name.c_str(), entity);
        }
        T& Get_Data(Entity entity){
            if(entity_to_index_map.find(entity) == entity_to_index_map.end())
            {
                Logger::log(LogLevel::ERROR, "Retrieving non-existent %s component for entity %d", component_name.c_str(), entity);
            }
            return component_array[entity_to_index_map[entity]];
        }
        void Entity_Destroyed(Entity entity) override{
            if (entity_to_index_map.find(entity) != entity_to_index_map.end())
            { 
                Remove_Data(entity);
            }
        }
        const std::string& Get_Name(){
            return component_name;
        }
        std::array<T, MAX_ENTITIES>& Get_Array()
        {
            return component_array;
        }
        bool Has_Data(Entity entity)
        {
            return entity_to_index_map.find(entity) != entity_to_index_map.end();
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
        std::bitset<MAX_ENTITIES> entity_has_component;

        std::string component_name{};

        std::unordered_map<Entity, size_t> entity_to_index_map;
        std::unordered_map<size_t, Entity> index_to_entity_map;
        size_t size{};
};

class ComponentSYSTEM
{
    public:
        template<typename T, typename... Args>
        void Register_Component_Type(sol::state& lua, const std::string& comp_type, Args&&... args)
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            if(component_types.find(typeIndex) != component_types.end())
            {
                Logger::log(LogLevel::WARNING, "Registering component type %s falied, type has already been registered.", comp_type.c_str());
            }
            else
            {
                auto it = std::find_if(component_names.begin(), component_names.end(), [comp_type](const auto& pair) {
                    return pair.second == comp_type;
                });
                if (it != component_names.end())
                {
                    Logger::log(LogLevel::WARNING, "Registering component type %s falied, type name has already been registered.", comp_type.c_str());
                }
                else
                {
                    component_types.insert({typeIndex, next_component_type});
                    component_arrays.insert({typeIndex, std::make_shared<ComponentArray<T>>(comp_type)});
                    component_names.insert({typeIndex, comp_type});
                    lua.new_usertype<T>(comp_type, std::forward<Args>(args)...);
                    lua["ComponentSYSTEM"]["Get_Component_" + comp_type] = &ComponentSYSTEM::Get_Component<T>;
                    //lua["ComponentSYSTEM"]["Add_Component"] = &ComponentSYSTEM::Add_Component<T>;

                    std::function<void(Entity)> builder = [this](Entity entiy)
                    {
                        this->Add_Component<T>(entiy, T());
                    };
                    component_builders.insert({comp_type, builder});                    

                    ++next_component_type;
                    Logger::log(LogLevel::INFO, "Component type %s registered", comp_type.c_str());
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

        bool Has_Component_Type(const std::string& componentType)
        {
            if(component_builders.find(componentType) != component_builders.end()){return true;}
            else{return false;}
        }

        template<typename T>
        T& Add_Component(Entity entity, const T& component)
        {
            Get_Component_Array<T>()->Insert_Data(entity, component);   
            return Get_Component_Array<T>()->Get_Data(entity);
        }

        void Add_Component_By_Name(Entity entity, const std::string& component)
        {
            if(component_builders.find(component) == component_builders.end())
            {
                Logger::log(LogLevel::WARNING, "Adding component %s by name failed, component name does not exists", component.c_str());
            }
            else
            {
                component_builders[component](entity);
            }       
        }
        
        template<typename T>
        void Remove_Component(Entity entity)
        {
            Get_Component_Array<T>()->Remove_Data(entity);
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
        std::unordered_map<std::string, std::function<void(Entity)>> component_builders;
        std::unordered_map<std::type_index, std::string> component_names;
        ComponentType next_component_type{};
};

#endif // !COMPONENT_H