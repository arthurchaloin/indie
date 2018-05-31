/*
** EPITECH PROJECT, 2018
** Indie
** File description:
** My Script Parser Hpp
*/

#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#include "engine/core/EntityId.hpp"
#include "engine/core/Entity.hpp"
#include "engine/core/Game.hpp"
#include "engine/components/ButtonComponent.hpp"
#include "engine/menu/classes/parsers/ButtonFactory.hpp"
#include "engine/components/IrrlichtComponent.hpp"

namespace engine { namespace Menu {

	class MyScriptParser {
		public:
			MyScriptParser(std::string path, engine::Scene *scene, engine::Game *game);
			~MyScriptParser() = default;

			int checkStart(std::string line, std::string chars);
			int checkElement(std::string elementName, int line);
			void createElement(std::string type);
			void parseFile();
			void fillMap();

			void manageButton();
		private:
			engine::Scene							*_scene;
			engine::Game							*_game;
			
			std::string							_path;
			std::vector<std::string>					_storage;
			std::unordered_map<std::string, std::function<void(void)>>	_elementHandled;

			int								_lineNb;
			std::vector<std::string>::iterator				_it;
			bool								_isOver;
	};
}}