/*
** EPITECH PROJECT, 2017
** bomberman
** File description:
** A file for bomberman - Paul Laffitte
*/

#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include <irrlicht/irrlicht.h>
#include <stack>
#include "engine/core/Scene.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "EventsHandler.hpp"

namespace engine {

	class System;
	using MeshNode = irr::scene::IAnimatedMesh;

	class Game {
	public:
		Game();
		virtual ~Game();

		using SceneModel = std::function<Scene& (Scene&)>;
		using SceneModels = std::unordered_map<std::string, SceneModel>;

		void play(std::string const& name);

		void replaceScene(std::string const& name);
		void pushScene(std::string const& name);
		void popScene();

		void registerSystem(std::string const& name, System* system);
		void registerSceneModel(std::string const& name, SceneModel const& sceneModel);

		irr::IrrlichtDevice& device();
		irr::IrrlichtDevice const& device() const;

		ResourceManager<MeshNode*> meshManager;
		EventsHandler eventsHandler;

	private:
		irr::IrrlichtDevice* _device = nullptr;
		Event<KeyState> _keyEvents;
		EventsReceiver _eventReceiver;
		std::unordered_map<std::string, System*> _systems;
		std::list<Scene> _scenes;
		SceneModels _sceneModels;

		void _updateScenes();
	};
}
