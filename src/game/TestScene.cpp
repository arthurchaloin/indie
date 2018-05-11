/*
** EPITECH PROJECT, 2017
** bomberman
** File description:
** A file for bomberman - Paul Laffitte
*/

#include <iostream>
#include "TestScene.hpp"

testGame::TestScene::TestScene(engine::Game* game) : _game(game)
{}

testGame::TestScene::~TestScene()
{}

engine::Game::SceneModel
testGame::TestScene::getSceneModel()
{
	return [&](engine::Scene& scene) -> engine::Scene& {
		scene.registerEntityModel("map", [&](engine::Entity const& entity) -> engine::Entity const& {
			auto& displayComponent = entity.addComponent<engine::DisplayComponent>();
			displayComponent.init(_game, "plant.md3");
			return entity;
		});

		scene.registerEvent<int>("display map", [&](void const*) {
			std::cout << "ok" << std::endl;
			scene.spawnEntity("map");
		});

		_game->eventsHandler.subscribe([&](engine::KeyState const& keystate) -> int {
			std::cout << "hey" << std::endl;
			if (keystate.Key == engine::KeyCode::KEY_KEY_E && keystate.PressedDown) {
				scene.triggerEvent("display map", 0);
			}

			return 0;
		});

		return scene;
	};
}
