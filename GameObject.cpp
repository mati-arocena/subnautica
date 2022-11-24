#include "GameObject.h"

LOD GameObject::getLOD()
{
	return LOD::LOD0;
}

void GameObject::render()
{
	renderLOD(getLOD());
}
