/*
** EPITECH PROJECT, 2017
** worms
** File description:
** A file for worms - Paul Laffitte
*/

#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include "engine/components/TestComponent.hpp"
#include "engine/components/DisplayComponent.hpp"
#include "engine/components/ComponentVisitor.hpp"

namespace engine {

	using AnyComponent = boost::variant<TestComponent, DisplayComponent>;

	template <typename ComponentType>
	using EntityComponents = std::multimap<EntityId, ComponentType>;

	template <typename ComponentType>
	using UniqueEntityComponents = std::map<EntityId, std::shared_ptr<ComponentType> >;

	template <typename ComponentType>
	using Components = std::unordered_map<std::type_index, EntityComponents<ComponentType> >;

	template <typename ComponentType>
	using UniqueComponents = std::unordered_map<std::type_index, UniqueEntityComponents<ComponentType> >;

	/**
	 * Contains several type of components attached to Entities by EntityId
	 */
	class ComponentPool {
	public:

		template <typename ComponentType>
		ComponentType&
		addComponent(EntityId entityId)
		{
			if (!this) {
				throw std::runtime_error("null component pool");
			}

			EntityComponents<AnyComponent>& components = getComponents<ComponentType>();
			EntityComponents<AnyComponent>::iterator componentIt = components.emplace(entityId, ComponentType());

			if (componentIt == std::end(components))
				throw std::runtime_error("unable to add a new component");
			return boost::get<ComponentType>(componentIt->second);
		}

		template <typename ComponentType>
		ComponentType&
		getComponent(EntityId entityId)
		{
			EntityComponents<AnyComponent>& components = getComponents<ComponentType>();
			EntityComponents<AnyComponent>::iterator componentIt = components.find(entityId);

			if (componentIt == std::end(components))
				throw std::runtime_error("component not found");
			return boost::get<ComponentType>(componentIt->second);

		}

		template <typename ComponentType>
		typename EntityComponents<ComponentType>::iterator
		getComponents(EntityId entityId)
		{
			return this->getComponents<ComponentType>().find(entityId);
		}

		template <typename ComponentType>
		EntityComponents<AnyComponent>& getComponents()
		{
			return _components[typeid(ComponentType)];
		}

		void copyComponents(EntityId dest, EntityId src);

	private:
		Components<AnyComponent> _components;
	};
}
