#pragma once
#include "pugixml/pugixml.hpp"

class Loader
{
public:
	static void loadScene();
private:
	static void loadPlayer(pugi::xml_node node);
	static void loadModels(pugi::xml_node node);
	static void loadLight(pugi::xml_node node);
};

