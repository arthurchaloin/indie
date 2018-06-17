//
//  BattleSceneModel.hpp
//  engine
//
//  Created by Arthur Chaloin on 14/05/2018.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <game/map/Map.hpp>
#include "engine/core/Game.hpp"
#include "engine/systems/PhysicsSystem.hpp"
#include "engine/components/LightComponent.hpp"
#include "engine/components/IrrlichtComponent.hpp"
#include "engine/components/HitboxComponent.hpp"
#include "engine/components/TransformComponent.hpp"
#include "engine/components/AnimationComponent.hpp"
#include "engine/components/TagComponent.hpp"
#include "engine/components/CameraComponent.hpp"
#include "engine/components/PhysicsComponent.hpp"
#include "engine/components/ItemComponent.hpp"
#include "engine/components/HoldComponent.hpp"
#include "engine/components/ParticlesComponent.hpp"
#include "engine/components/TimeoutComponent.hpp"
#include "game/components/MasterComponent.hpp"
#include "game/components/PlayerComponent.hpp"
#include "game/components/WeaponComponent.hpp"
#include "game/events/Vector.hpp"

namespace worms { namespace scene {

		static const auto battle = [](engine::Game& game, engine::Scene& scene) {

			engine::Entity master(engine::Entity::nullId, engine::Entity::nullId, &scene.getEntities());
			master.set<MasterComponent>().currentPlayer = 0;

			scene.registerEntityModel("camera", [&](engine::Entity const& entity) {
				entity.set<engine::TransformComponent>();


				auto& cameraComponent = entity.set<engine::CameraComponent>(game.device());

				scene.registerEvent<Vector3f>("camera.goto", entity.getId(), [&](auto const& position) {
					cameraComponent.node->setPosition(position);
					return 0;
				});

				scene.registerEvent<Vector3f>("camera.lookat", entity.getId(), [&](auto const& position) {
					cameraComponent.node->setTarget(position);
					return 0;
				});

				scene.registerEvent<Vector3f>("camera.move", entity.getId(), [&](auto const& offset) {
					cameraComponent.node->setPosition(cameraComponent.node->getPosition() + offset);
					cameraComponent.node->setTarget(cameraComponent.node->getTarget() + offset);
					return 0;
				});
			});

			scene.registerEntityModel("explosion", [&](engine::Entity const& entity) {
				auto& transformComponent = entity.set<engine::TransformComponent>();
				auto& particlesComponent = entity.set<engine::ParticlesComponent>(game.device(), 1, 2);

				entity.set<engine::TimeoutComponent>(.1f, [&particlesComponent]() -> void {
					particlesComponent.node->setEmitter(nullptr);
				});

				entity.set<engine::TimeoutComponent>(1.f, [entity]() -> void {
					entity.kill();
				});

				particlesComponent.node->setMaterialTexture(0, engine::ResourceManager<engine::Texture*>::instance().get("texture/explosion_particle.jpg"));
				particlesComponent.node->getEmitter()->setMinStartSize(irr::core::dimension2df(1.f, 1.f));
				particlesComponent.node->getEmitter()->setMaxStartSize(irr::core::dimension2df(3.f, 3.f));
				particlesComponent.node->getEmitter()->setMinStartColor(irr::video::SColor(0, 255, 200, 190));
				particlesComponent.node->getEmitter()->setMaxStartColor(irr::video::SColor(0, 255, 255, 255));
			});

			scene.registerEntityModel("player", [&](engine::Entity const& entity) {
				entity.set<PlayerComponent>(0);
				entity.set<engine::IrrlichtComponent>(&game, "obj/silinoid.ms3d", "texture/silinoid.png");
                entity.set<engine::TagComponent>(std::string("player"));
                std::cout << "player " << entity.getId() << std::endl;

				entity.set<engine::TimeoutComponent>(1.f, []() -> void {
					std::cout << "callback after timeout" << std::endl;
				});

				auto& physicsComponent = entity.set<engine::PhysicsComponent>();
				auto& transformComponent = entity.set<engine::TransformComponent>();
				auto& animationComponent = entity.set<engine::AnimationComponent>("idle", 60);

				engine::AnimationBoundaries idle(0, 80);
				engine::AnimationBoundaries run(80, 40);
				engine::AnimationBoundaries jump(120, 35);
				engine::AnimationBoundaries inAir(155, 25);

				animationComponent.states.emplace("idle", idle);
				animationComponent.states.emplace("run", run);
				animationComponent.states.emplace("jump", jump);
				animationComponent.states.emplace("inAir", inAir);

				irr::s32 holdHeavyOffset = 290;
				animationComponent.states.emplace("idleHoldHeavy", idle + holdHeavyOffset);
				animationComponent.states.emplace("runHoldHeavy", run + holdHeavyOffset);
				animationComponent.states.emplace("jumpHoldHeavy", jump + holdHeavyOffset);
				animationComponent.states.emplace("inAirHoldHeavy", inAir + holdHeavyOffset);

				transformComponent.scale = {0.5f, 0.5f, 0.5f};
				transformComponent.position = {0.f, 25.f, 0.f};

				auto& hitboxComponent = entity.set<engine::HitboxComponent>("(-1 0, -1 4, 1 4, 1 0)");
				hitboxComponent.hasDebugMode = true;
				hitboxComponent.rebound = 0.1f;

				auto& hc = entity.set<engine::HoldComponent>();

				scene.registerEvent<std::string>("player.move", entity.getId(), [&](std::string const& move) {
                    auto _move = (Vector2f)move;
					physicsComponent.move = _move;

                    if (_move.x > 0)
                        transformComponent.direction = true;
                    else if (_move.x < 0)
                        transformComponent.direction = false;
					return 0;
				});

				scene.registerEvent<std::string>("player.jump", entity.getId(), [entity, &scene, &physicsComponent, &animationComponent, &hc](std::string const& jump) {
					if (engine::PhysicsSystem::isGrounded(scene.getEntities(), entity)) {
						physicsComponent.velocity += (Vector2f) jump;
						animationComponent.currentState = PlayerSystem::getState("jump", hc);
						animationComponent.playOnce = true;
						animationComponent.nextState = PlayerSystem::getState("inAir", hc);
					}

					return 0;
				});

				scene.registerEvent<std::string>("player.pick", entity.getId(), [entity, &hc](std::string const& s) {
					if (hc.hasReachableEntity) {
						if (hc.items.size() == hc.count) {
							engine::Entity& item = hc.items[hc.current];
							item.detach();

							auto item2 = entity.attach(hc.reachableEntity);
							hc.items[hc.current] = item2;
						} else {
							if (hc.current >= 0) {
								hc.items[hc.current].disable();
							}
							auto item = entity.attach(hc.reachableEntity);
							hc.items[++hc.current] = item;
							hc.count += 1;
						}
						hc.hasReachableEntity = false;
					}
					return 0;
				});
//
				scene.registerEvent<std::string>("player.use", entity.getId(), [entity, &hc](std::string const& s) {
						engine::Entity& item = hc.items[hc.current];
					if (hc.current >= 0) {
						if (item.has<engine::ItemComponent>()) {
							item.get<engine::ItemComponent>().use();
						}
					}
					return 0;
				});

                scene.registerEvent<std::string>("player.aim", entity.getId(), [entity, &hc](std::string const& move) {
					if (hc.current >= 0) {
						engine::Entity& item = hc.items[hc.current];
						if (item.has<engine::ItemComponent>() && item.has<WeaponComponent>()) {
                            auto& weapon = item.get<WeaponComponent>();
                            if (weapon.hasAim)
                                weapon.aimPosition += (Vector2f)move;
						}
					}
					return 0;
				});

				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_KEY_Q, "player.move", entity.getId(), Vector2f(-10.f, 0.f), engine::EVT_SYNCED);
				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_KEY_Q, "player.move", entity.getId(), Vector2f(0.f, 0.f), engine::EVT_SYNCED | engine::EVT_RELEASE);
				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_KEY_D, "player.move", entity.getId(), Vector2f(10.f, 0.f), engine::EVT_SYNCED);
				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_KEY_D, "player.move", entity.getId(), Vector2f(0.f, 0.f), engine::EVT_SYNCED | engine::EVT_RELEASE);
				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_SPACE, "player.jump", entity.getId(), Vector2f(0.f, 100.f), engine::EVT_SYNCED);
				game.eventsHandler.subscribe<std::string>(scene, engine::KeyCode::KEY_KEY_R, "player.pick", entity.getId(), "0", engine::EVT_SYNCED);
				game.eventsHandler.subscribe<std::string>(scene, engine::KeyCode::KEY_KEY_U, "player.use", entity.getId(), "0", engine::EVT_SYNCED);
                game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_UP, "player.aim", entity.getId(), Vector2f(0.f, 1.f), engine::EVT_SYNCED);
                game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_RIGHT, "player.aim", entity.getId(), Vector2f(1.f, 0.f), engine::EVT_SYNCED);
                game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_DOWN, "player.aim", entity.getId(), Vector2f(0.f, -1.f), engine::EVT_SYNCED);
				game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_LEFT, "player.aim", entity.getId(), Vector2f(-1.f, 0.f), engine::EVT_SYNCED);
			});

        scene.registerEntityModel("sword", [&](engine::Entity const& entity) {
            entity.set<engine::TagComponent>(std::string("sword"));
            std::cout << "sword " << entity.getId() << std::endl;

            auto& wc = entity.set<WeaponComponent>();
            wc.hasAim = true;

            entity.set<engine::IrrlichtComponent>(&game, "obj/sword.obj", "obj/sword.mtl");
            entity.set<engine::PhysicsComponent>();

			auto& transformComponent = entity.set<engine::TransformComponent>();
			transformComponent.position = {10.f, 10.f, 0.f};
            transformComponent.scale = {0.25f, 0.25f, 0.25f};
            transformComponent.offset = {0.f, -5.8f, 0.f};
            transformComponent.offsetRotation = {0.f, -90.f, 0.f};

			auto& hitboxComponent = entity.set<engine::HitboxComponent>("(-6 -1.5, -6 1.5, 6 1.5, 6 -1.5)");
			hitboxComponent.rebound = 0.2;
			hitboxComponent.hasDebugMode = true;

            auto& ic = entity.set<engine::ItemComponent>();
            ic.use = [&]() {
                auto bullet = scene.spawnEntity("sword.bullet");
                auto& bt = bullet.get<engine::TransformComponent>();
                auto& bp = bullet.get<engine::PhysicsComponent>();
                bt.position = transformComponent.position;
                bp.velocity = wc.aimPosition;
                bp.velocity = bp.velocity.normalize() * 100.f;
            };
            ic.offset = {1.5f, 1.f, 0.f};
		});

        scene.registerEntityModel("sword.bullet", [&](engine::Entity const& entity) {
            entity.set<engine::TagComponent>(std::string("projectile"));

            entity.set<engine::IrrlichtComponent>(&game, "obj/missile.obj", "texture/missile.png");
            entity.set<engine::PhysicsComponent>();

			auto& transformComponent = entity.set<engine::TransformComponent>();
            transformComponent.scale = {0.25f, 0.25f, 0.25f};
			auto& hitboxComponent = entity.set<engine::HitboxComponent>("(-1 -1, -1 1, 1 1, 1 -1)");
            hitboxComponent.onCollide = [entity, &scene, &transformComponent](engine::Entity const& collideWith) -> void {
				entity.set<engine::TimeoutComponent>(0.0001f, [entity, &scene, &transformComponent]() -> void {
					Wornite::Map::tryDestroyMap(scene, transformComponent.position.X, transformComponent.position.Y, 2.f);
					entity.kill();
				});
				auto& explosionTransform = scene.spawnEntity("explosion").get<engine::TransformComponent>();
				explosionTransform.position = transformComponent.position;
            };
            hitboxComponent.hasDebugMode = true;
		});

        scene.registerEntityModel("rpg", [&](engine::Entity const& entity) {
            entity.set<engine::TagComponent>(std::string("rpg"));

			entity.set<engine::IrrlichtComponent>(&game, "obj/rpg.obj", "texture/rpg.png");
            entity.set<engine::PhysicsComponent>();
			auto& ic = entity.set<engine::ItemComponent>([]() {
                std::cout << "use item" << std::endl;
            });

            ic.offset = {1.f, 2.f, 0.f};

			auto& transformComponent = entity.set<engine::TransformComponent>();
			transformComponent.position = {-10.f, 10.f, 0.f};
            transformComponent.scale = {0.5f, 0.5f, 0.5f};

			auto& hitboxComponent = entity.set<engine::HitboxComponent>("(-1 -1, -1 1, 1 1, 1 -1)");
			hitboxComponent.rebound = 0.2;
			hitboxComponent.hasDebugMode = true;
		});

		scene.registerEvent<std::string>("player.spawn", 0, [&](std::string const&) {
            scene.spawnEntity("rpg");
			scene.spawnEntity("player");
			return 0;
		});

		scene.registerEvent<std::string>("master.changePlayer", 0, [&](std::string const& player) {
			scene.getEntities().each<MasterComponent>([&](engine::Entity const& e, auto& m) {
				m.currentPlayer = std::stoi(player);
				if (m.currentPlayer != 0) { //TODO: Replace by player id
					game.eventsHandler.disableKeyEvent(engine::KeyCode::KEY_KEY_D);
					game.eventsHandler.disableKeyEvent(engine::KeyCode::KEY_KEY_Q);
					game.eventsHandler.disableKeyEvent(engine::KeyCode::KEY_SPACE);
				} else {
					game.eventsHandler.enableKeyEvent(engine::KeyCode::KEY_KEY_D);
					game.eventsHandler.enableKeyEvent(engine::KeyCode::KEY_KEY_Q);
					game.eventsHandler.enableKeyEvent(engine::KeyCode::KEY_SPACE);
				}
			});

			return 0;
		});

		scene.registerEntityModel("background", [&](engine::Entity const& entity) {
			auto& i = entity.set<engine::IrrlichtComponent>(&game, "obj/spaceBackground.obj");
			auto& t = entity.set<engine::TransformComponent>();

			t.scale *= 550;
			t.scale.Y *= 1200.f / 1920.f;
			t.rotation.X = 10;
			t.position.Z = 200;
		});

		Wornite::Map().genMap(game, scene);

		scene.registerEntityModel("map", [&](engine::Entity const& entity) {
			Wornite::Map().genMap(game, scene).getSeed();

			scene.registerEvent<std::string>("map.hitbox.display", 0, [&](std::string const& move) {
				static bool DebugMode = true;
				engine::Entities entities = scene.getEntities();
				entities.withTag("map", [&](engine::Entity const& chunk) {
					entities.eachChilds(chunk.getId(), [&](engine::Entity const &child) {
						auto& h = child.get<engine::HitboxComponent>();

						h.hasDebugMode = DebugMode;
					});
				});
				DebugMode = !DebugMode;
				return 0;
			});


			return 0;
		});

		game.eventsHandler.subscribe<std::string>(scene, engine::KeyCode::KEY_KEY_E, "player.spawn", 0, std::to_string(1));
		game.eventsHandler.subscribe<std::string>(scene, engine::KeyCode::KEY_KEY_P, "master.changePlayer", 0, std::to_string(1));
		game.eventsHandler.subscribe<std::string>(scene, engine::KeyCode::KEY_KEY_O, "master.changePlayer", 0, std::to_string(0));
		game.eventsHandler.subscribe<Vector2f>(scene, engine::KeyCode::KEY_KEY_H, "map.hitbox.display", 0, Vector2f(0.f, 0.f), engine::EVT_SYNCED);

		scene.spawnEntity("camera");
		scene.spawnEntity("map");
		scene.spawnEntity("player");
		scene.spawnEntity("rpg");
		scene.spawnEntity("sword");
		scene.spawnEntity("background");
	};
}}
