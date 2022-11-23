#include "World.h"
#include <algorithm>
std::vector<std::shared_ptr<GameObject>> World::getObjectsInFrustum()
{
	objectsInFrustum.clear();

	dynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(frustum->getOverlappingPairCache(), dynamicsWorld->getDispatchInfo(), dynamicsWorld->getDispatcher());
	
	btBroadphasePairArray& collisionPairs = frustum->getOverlappingPairCache()->getOverlappingPairArray();	//New
	const int	numObjects = collisionPairs.size();
	static btManifoldArray	m_manifoldArray;
	bool added;
	for (int i = 0;i < numObjects;i++) {
		const btBroadphasePair& collisionPair = collisionPairs[i];
		m_manifoldArray.resize(0);
		if (collisionPair.m_algorithm) collisionPair.m_algorithm->getAllContactManifolds(m_manifoldArray);
		else printf("No collisionPair.m_algorithm - probably m_dynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(...) must be missing.\n");
		added = false;
		for (int j = 0;j < m_manifoldArray.size();j++) {
			btPersistentManifold* manifold = m_manifoldArray[j];
			if (manifold->getNumContacts() > 0) {
				btCollisionObject* collisionObject = (btCollisionObject*)(manifold->getBody0() == frustum.get() ? manifold->getBody1() : manifold->getBody0());
				btRigidBody* rigidBody = btRigidBody::upcast(collisionObject);

				//Object exist on map but is not already in frusum objects
				if (collisionObjects.find(rigidBody) != collisionObjects.end() && 
					std::find(objectsInFrustum.begin(), objectsInFrustum.end(), collisionObjects[rigidBody]) == objectsInFrustum.end())
				{
					objectsInFrustum.push_back(collisionObjects[rigidBody]);
				}
				break;
			}
		}
	}
	return objectsInFrustum;
}

World::World()
{
	btCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(new btCollisionDispatcher(collisionConfiguration), new btDbvtBroadphase(), 
		new btSequentialImpulseConstraintSolver(), collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

}

void World::update(double deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime, 10);
}

void World::render()
{
}

void World::render_withShader(std::shared_ptr<Shader> shader)
{
}

std::vector<std::shared_ptr<btRigidBody>> World::getCollisionObject()
{
	return std::vector<std::shared_ptr<btRigidBody>>();
}

void World::addToWorld(std::shared_ptr<GameObject> gameObject)
{
	auto rigidBodies = gameObject->getCollisionObject();
	for (auto rigidBody : rigidBodies)
		collisionObjects[rigidBody.get()] = gameObject;
}

void World::addToWorld(const Camera& camera)
{
	frustum = camera.getViewFrustum();
	
	dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback.get());
	dynamicsWorld->addCollisionObject(frustum.get());
}
