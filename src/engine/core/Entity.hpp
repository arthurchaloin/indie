/*
** EPITECH PROJECT, 2017
** bomberman
** File description:
** A file for bomberman - Paul Laffitte
*/

#pragma once

#include <cstddef>
#include <engine/core/Event.hpp>
#include <engine/core/EntityId.hpp>
#include <engine/components/ComponentPool.hpp>
#include <engine/components/UniqueComponentPool.hpp>
#include <engine/components/ComponentConstraint.hpp>
#include <engine/components/ComponentFilter.hpp>

namespace engine {

	class Entities;

	/**
	 * An entity composed of components. The smallest logic unit in a game
	 */
	class Entity {
	public:
		Entity();
		Entity(EntityId id, EntityId parentId, Entities* entities);
		Entity(Entity const& entity);
		Entity& operator=(Entity const& entity);
		virtual ~Entity();

		void kill();

		template <typename ComponentType, typename... CtorArgsTypes>
		ComponentType&
		setComponent(CtorArgsTypes... ctorArgs) const
		{
			return UniqueComponentPool<ComponentType>::instance().setComponent(_id, std::forward<CtorArgsTypes>(ctorArgs)...);
		}

		template <typename ComponentType, typename... CtorArgsTypes>
		ComponentType&
		addComponent(CtorArgsTypes... ctorArgs) const
		{
			return ComponentPool<ComponentType>::instance().addComponent(_id, std::forward<CtorArgsTypes>(ctorArgs)...);
		}

		/**
		 * Get entity's components through a callback
		 * @tparam ComponentsTypes Types of components to get
		 * @param callback Callback taking one argument by ComponentsTypes. Arguments can be component or list of component, depending on the component's ComponentConstraint
		 */
		template<typename... ComponentsTypes>
		void
		get(typename ComponentFilter<ComponentsTypes...>::Callback const& callback) const
		{
			ComponentFilter<ComponentsTypes...>().get(_id, callback);
		}

		EntityId getId() const;

		EntityId getParentId() const;

		static EntityId const nullId;

	private:
		EntityId _id;
		EntityId _parentId;

		Entities* _entities;
	};
}
