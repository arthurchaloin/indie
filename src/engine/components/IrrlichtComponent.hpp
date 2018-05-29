//
//  IrrlichtComponent.hpp
//  engine
//
//  Created by Arthur Chaloin on 21/05/2018.
//

#pragma once

namespace engine {

	struct IrrlichtComponent {
		using Constraint = ComponentConstraint<IrrlichtComponent, false>;

		IrrlichtComponent();
		IrrlichtComponent(Game* game, std::string const& asset);

		irr::scene::IAnimatedMeshSceneNode* node = nullptr;
	};
}
