#ifndef COMPONENTSYSTEM_H
#define COMPONENTSYSTEM_H
#include <SYSTEM.h>
#include <typeindex>
#include <unordered_map>
#include <cassert>
#include <functional>
#include <memory>
#include <Components.h>
#include <bitset>
#include <sol/sol.hpp>

using ComponentType = std::size_t;

class IComponentArray
{
public:
    IComponentArray(const std::string &name): component_name(name){}
    virtual ~IComponentArray() = default;
    virtual void Entity_Destroyed(Entity entity) = 0;
    const std::string &Get_Name(){return component_name;}
protected:

private:
    std::string component_name{};
};

template <typename T>
class ComponentArray : public IComponentArray
{
public:
    ComponentArray(const std::string &name): IComponentArray(name){}
    void Insert_Data(Entity entity, const T &component){
        assert(!component_bitsets[entity] && "Component added to same entity more than once.");
        component_bitsets[entity] = true;
        component_array.at(entity) = component;
        entities_with_component.push_back(entity);
        Logger::log(LogLevel::INFO, "Added component %s to entity %d", Get_Name().c_str(), entity);
    }
    void Remove_Data(Entity entity){
        assert(component_bitsets[entity] && "Removing non-existent component.");
        Logger::log(LogLevel::INFO, "Removing component %s from entity %d", Get_Name().c_str(), entity);
        component_bitsets[entity] = false;
        entities_with_component.erase(std::remove(entities_with_component.begin(), entities_with_component.end(), entity), entities_with_component.end());
    }
    T &Get_Data(Entity entity){
        if (!component_bitsets[entity]){
            Logger::log(LogLevel::ERROR, "Retrieving non-existent %s component for entity %d", Get_Name().c_str(), entity);
        }
        return component_array[entity];
    }
    bool Has_Data(Entity entity){
        return component_bitsets[entity];
    }
    void Entity_Destroyed(Entity entity) override{
        Remove_Data(entity);
    }
    std::array<T, MAX_ENTITIES>& Get_Array(){
        return component_array;
    }
    const std::vector<Entity>& Get_Entities(){
        return entities_with_component;
    }
private:
    std::array<T, MAX_ENTITIES> component_array;
    std::bitset<MAX_ENTITIES> component_bitsets;
    
    std::vector<Entity> entities_with_component;

};

class ComponentSYSTEM
{
public:
    template <typename T, typename... Args>
    void Register_Component_Type(sol::state &lua, const std::string &comp_type, Args &&...args)
    {
        const std::type_index type_index = std::type_index(typeid(T));
        if (component_arrays.find(type_index) != component_arrays.end()){
            Logger::log(LogLevel::WARNING, "Registering component type %s falied, type has already been registered.", comp_type.c_str());
        }
        else{
            bool name_conflict = false;
            for (const auto &pair : component_arrays){
                if (pair.second->Get_Name() == comp_type){
                    name_conflict = true;
                    break;
                }
            }
            if (name_conflict){
                Logger::log(LogLevel::WARNING, "Registering component type %s falied, type name has already been registered.", comp_type.c_str());
            }
            else{
                component_arrays.insert({type_index, std::make_unique<ComponentArray<T>>(comp_type)});
                lua.new_usertype<T>(comp_type, std::forward<Args>(args)...);
                lua["ComponentSYSTEM"]["Get_Component_" + comp_type] = &ComponentSYSTEM::Get_Component<T>;
                Logger::log(LogLevel::INFO, "Component type %s registered", comp_type.c_str());
            }
        }
    }

    template <typename T>
    const std::string& Get_Component_Name(){
        std::type_index typeIndex = std::type_index(typeid(T));
        assert(component_arrays.find(typeIndex) != component_arrays.end() && "Component not registered before use.");
        return component_arrays[typeIndex]->Get_Name();
    }

    template <typename T>
    T &Add_Component(Entity entity, const T &component){
        Get_Component_Array<T>().Insert_Data(entity, component);
        return Get_Component_Array<T>().Get_Data(entity);
    }

    template <typename T>
    void Remove_Component(Entity entity){
        Get_Component_Array<T>().Remove_Data(entity);
    }

    template <typename T>
    T &Get_Component(Entity entity){
        return Get_Component_Array<T>().Get_Data(entity);
    }

    template <typename T>
    ComponentArray<T>& Get_Component_Array(){
        std::type_index typeIndex = std::type_index(typeid(T));
        assert(component_arrays.find(typeIndex) != component_arrays.end() && "Component not registered before use.");
        return *static_cast<ComponentArray<T>*>(component_arrays[typeIndex].get());
    }

    void Remove_All_Entity_Components(Entity entity){
        for (auto const &pair : component_arrays)
        {
            auto const &component = pair.second;
            component->Entity_Destroyed(entity);
            Logger::log(LogLevel::INFO, "All components of entity %d are destroyed", entity);
        }
    }

    template <typename T>
    bool Entity_Has_Component(Entity entity){
        return Get_Component_Array<T>().Has_Data(entity);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> component_arrays{};
};

#endif // !COMPONENT_H