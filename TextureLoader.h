#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Texture.h"

class TextureLoader
{
private:
	std::vector<std::shared_ptr<Texture>> texturesLoaded;

public:
	TextureLoader() {};
	~TextureLoader() {};
	
	std::shared_ptr<Texture> getTexture(std::string fileName, std::string typeName) {
		for (size_t j = 0; j < texturesLoaded.size(); j++)
		{
			if (fileName == this->texturesLoaded[j]->path && typeName == this->texturesLoaded[j]->name)
			{
				return this->texturesLoaded[j];
			}
		}
		// if texture hasn't been loaded already, load it
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(fileName.c_str(), typeName, true, true);
		texturesLoaded.push_back(texture);
		return texture;
	}
	
};

