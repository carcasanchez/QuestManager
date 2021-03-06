#include "j1GameLayer.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "j1Gui.h"
#include "j1CollisionManager.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1QuestManager.h"


// just for temporal wall collider
#include "Entity.h"


j1GameLayer::j1GameLayer() : j1Module()
{
	name = ("game");
	em = new j1EntityManager();
	playerId = em->entities.end();
	//hud = new Hud();
}

j1GameLayer::~j1GameLayer()
{
	RELEASE(hud);
	RELEASE(em);
}

bool j1GameLayer::Awake(pugi::xml_node& conf)
{
	em->Awake(conf);
	//hud->Awake(conf);
	return true;
}

// Called before the first frame
bool j1GameLayer::Start()
{
	active = true;


	em->Create(LINK, 150, 150);

	return true;
}

//preUpdate
bool j1GameLayer::PreUpdate()
{
	em->PreUpdate();
	//hud->PreUpdate();

	return true;
}

//update
bool j1GameLayer::Update(float dt)
{
	bool ret = true;

	em->Update(dt);
	//ret = hud->Update(dt);

	App->render->CameraFollow((*playerId)->currentPos);

	return ret;
}

//postUpdate
bool j1GameLayer::PostUpdate()
{
	em->PostUpdate();
	//hud->PostUpdate();

	return true;
}

// Called before quitting
bool j1GameLayer::CleanUp()
{
	active = false;

	//hud->CleanUp();
	em->CleanUp();

	return true;
}

bool j1GameLayer::On_Collision_Callback(Collider * c1, Collider * c2 , float dt)
{
	if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_NPC)
	{
		if((*playerId) != nullptr)
			(*playerId)->currentPos = (*playerId)->lastPos;
	}

	//TODO 9: Call the TriggerCollisionCallback if c2->type is COLLIDER_EVENT.
	//If return false, call StepCollisionCallback (give both c2 as an argument)
	if (c2->type == COLLIDER_EVENT)
	{
		if(App->quest->TriggerCollisionCallback(c2) == false)
			App->quest->StepCollisionCallback(c2);
	}

	

	return true;
}
