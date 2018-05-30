//
//  IrrlichtComponent.cpp
//  engine
//
//  Created by Arthur Chaloin on 10/05/2018.
//

#include "engine/core/Game.hpp"
#include "IrrlichtComponent.hpp"

engine::IrrlichtComponent::IrrlichtComponent()
{}

engine::IrrlichtComponent::IrrlichtComponent(Game* game, std::string const& asset)
{
	auto mesh = game->meshManager.get(asset);

	if (mesh == nullptr) {
		throw std::runtime_error("failed to load asset: '" + asset + "'");
	}

	this->node = game->device()->getSceneManager()->addAnimatedMeshSceneNode(mesh);

	if (this->node == nullptr) {
		throw std::runtime_error("failed to create node");
	}

	this->node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	this->node->setMD2Animation(irr::scene::EMAT_STAND);
}