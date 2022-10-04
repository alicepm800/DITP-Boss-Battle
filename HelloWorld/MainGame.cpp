#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#define PLAY_ADD_GAMEOBJECT_MEMBERS 
#include "Play.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

enum Agent8State
{
	STATE_APPEAR = 0,
	STATE_FLY,
	STATE_ATTACH,
	STATE_DETACH,
	STATE_DEAD
};

struct GameState {
	int remainingGems = 2;
	int gemCooldown = 0;
	float gameTime = 0;
	int spriteId = 0;
	int level = 1;
	Agent8State agentState = STATE_APPEAR;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_ASTEROID,
	TYPE_METEOR,
	TYPE_AGENT8,
	TYPE_ASTEROID_HOME,
	TYPE_GEM,
	TYPE_ASTEROID_PIECES,
	TYPE_DESTROYED,
	TYPE_PARTICLES
};

void UpdateAgent8();
void HandlePlayerControls();
void UpdateAsteroids();
void UpdateMeteor();
void UpdateGem();
void UpdateAsteroidPieces();
void UpdateParticles();
void UpdateLevelStart();


void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ){
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::MoveSpriteOrigin("spr_agent8_left_strip7", -60, 0);
	Play::MoveSpriteOrigin("spr_agent8_right_strip7", -60, 0);
	Play::LoadBackground( "Data\\Backgrounds\\background.png" );
	Play::StartAudioLoop( "music" );
	
	Play::MoveSpriteOrigin("spr_asteroid_strip", 13, 9);
	int id_asteroid_1 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 200 }, 50, "spr_asteroid_strip");
	GameObject& asteroid_1 = Play::GetGameObject(id_asteroid_1);
	asteroid_1.velocity = { -1, 2};
	asteroid_1.animSpeed = (1.0f);
	asteroid_1.rotation = 115.5;

	int id_asteroid_2 = Play::CreateGameObject(TYPE_ASTEROID, { 0, 500 }, 50, "spr_asteroid_strip");
	GameObject& asteroid_2 = Play::GetGameObject(id_asteroid_2);
	asteroid_2.velocity = { 1, 1 };
	asteroid_2.animSpeed = (1.0f);
	asteroid_2.rotation = 120;

	int id_meteor = Play::CreateGameObject(TYPE_METEOR, { 1280, 10 }, 50, "meteor");
	GameObject& meteor = Play::GetGameObject(id_meteor);
	meteor.velocity = { -1 , 1 };
	meteor.rotation = 115.56;

	int id_agent8 = Play::CreateGameObject(TYPE_AGENT8, { 600, 450 }, 40, "agent8_fly");
	GameObject& obj_agent8 = Play::GetGameObject(id_agent8);	
}


bool MainGameUpdate( float elapsedTime ){
	Play::DrawBackground();
	Play::DrawFontText("64px", "ARROW KEYS TO ROTATE AND SPACE BAR TO LAUNCH",{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 30 }, Play::CENTRE);
	Play::DrawFontText("132px", "REMAINING GEMS: " + std::to_string(gameState.remainingGems), { DISPLAY_WIDTH / 1.21f, 50 }, Play::CENTRE);
	Play::DrawFontText("132px", "LEVEL " + std::to_string(gameState.level), { DISPLAY_WIDTH / 9.0f, 50 }, Play::CENTRE);
	UpdateAgent8();
	UpdateGem();
	UpdateAsteroids();
	UpdateMeteor();
	HandlePlayerControls();
	UpdateAsteroidPieces();
	gameState.gameTime += elapsedTime;
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

void HandlePlayerControls() {
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int>asteroid_list = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);

	if (Play::KeyDown(VK_RIGHT)) {
		obj_agent8.rotation = obj_agent8.oldRot + 0.1;
	}

	if (Play::KeyDown(VK_LEFT)) {
		obj_agent8.rotation = obj_agent8.oldRot - 0.1;
	}	
}

void UpdateAgent8() {
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

	switch (gameState.agentState) {

	case STATE_APPEAR:
		Play::SetSprite(obj_agent8, "agent8_fly", 0.0f);
		obj_agent8.rotation = 0;
		obj_agent8.pos = { 600, 450 };
		obj_agent8.velocity = { 0, 0 };
		if (Play::KeyPressed(VK_SPACE)) { 
			Play::SetGameObjectDirection(obj_agent8, 4, obj_agent8.rotation);
			gameState.agentState = STATE_FLY;
		}
		
		break;
	
	case STATE_FLY:	
		UpdateParticles();
		if (Play::KeyPressed(VK_SPACE)) {
			Play::SetGameObjectDirection(obj_agent8, 4, obj_agent8.rotation);
		
		}
		gameState.gemCooldown--;

		if (gameState.level == 1  && gameState.remainingGems == 0) {
			Play::DestroyGameObjectsByType(TYPE_ASTEROID);
			Play::DestroyGameObjectsByType(TYPE_METEOR);
			Play::DrawFontText("132px", "PRESS SPACE FOR LEVEL 2", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE); 
			if (Play::KeyDown(VK_SPACE)) {
				gameState.level++;
				UpdateLevelStart();
			}
		}
		if (gameState.level == 2 && gameState.remainingGems == 0) {
			Play::DestroyGameObjectsByType(TYPE_ASTEROID);
			Play::DestroyGameObjectsByType(TYPE_METEOR);
			Play::DrawFontText("132px", "PRESS SPACE FOR LEVEL 3", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
			if (Play::KeyDown(VK_SPACE)) {
				gameState.level++;
				UpdateLevelStart();
			}
		}

		if (gameState.level == 3 && gameState.remainingGems == 0) {
			Play::DestroyGameObjectsByType(TYPE_ASTEROID);
			Play::DestroyGameObjectsByType(TYPE_METEOR);
			Play::DrawFontText("132px", "MISSION COMPLETED", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		}
		break;

	case STATE_DEAD:
		Play::SetSprite(obj_agent8, "spr_agent8_dead", 0.18f);
		
		Play::DrawSpriteRotated("spr_agent8_dead_strip2", obj_agent8.pos, obj_agent8.frame, obj_agent8.rotation - (PLAY_PI*2.0f) * 0.25, 1.0f, 1.0f); 
		Play::PlayAudio("die");
		if (Play::KeyDown(VK_RIGHT)) {
			obj_agent8.rotation = obj_agent8.oldRot; 
		}

		if (Play::KeyDown(VK_LEFT)) { 
			obj_agent8.rotation = obj_agent8.oldRot; 
		}
		Play::DrawFontText("132px", "PRESS SPACE TO RESTART GAME", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT/2 }, Play::CENTRE);  
		Play::DestroyGameObjectsByType(TYPE_ASTEROID); 
		Play::DestroyGameObjectsByType(TYPE_METEOR); 

		if (Play::KeyPressed(VK_SPACE) == true){
			UpdateLevelStart();
			gameState.level = 1;
			gameState.remainingGems = 2;
			UpdateLevelStart;
		}
		
		break;

	case STATE_ATTACH:	
		GameObject& asteroid = Play::GetGameObjectByType(TYPE_ASTEROID_HOME);
		Play::UpdateGameObject(asteroid, 1);
		Play::DrawObjectRotated(asteroid);

		obj_agent8.pos = asteroid.pos;
		obj_agent8.animSpeed = 0; 
		
		if (Play::KeyDown(VK_RIGHT)) {
			obj_agent8.rotation = obj_agent8.oldRot + 0.05;
			Play::SetSprite(obj_agent8, "spr_agent8_right_strip7", 1.0f);			
		}

		else if (Play::KeyDown(VK_LEFT)) {
			obj_agent8.rotation = obj_agent8.oldRot - 0.05;
			Play::SetSprite(obj_agent8, "spr_agent8_left_strip7", 1.0f);		
		}
		


		if (Play::KeyDown(VK_SPACE)) {
			Play::SetSprite(obj_agent8, "spr_agent8_fly", 0.0f);
			Play::SetGameObjectDirection(obj_agent8, 4, obj_agent8.rotation);
			gameState.agentState = STATE_FLY;
			gameState.gemCooldown = 60;
			int gem_id = Play::CreateGameObject(TYPE_GEM, asteroid.pos, 20, "gem");

			for (int f = 0; f < 3; f++) {
				int pieces_id = Play::CreateGameObject(TYPE_ASTEROID_PIECES, asteroid.pos, 0, "spr_asteroid_pieces");
				GameObject& obj_piece = Play::GetGameObject(pieces_id);
				obj_piece.frame = f;

				Play::SetGameObjectDirection(obj_piece, 2, (PLAY_PI*2.0f) * (0.25f - (0.33f * f)) );
			}

			Play::DestroyGameObject(asteroid.GetId());	
			Play::PlayAudio("explode");
		}
		break;
	
	}
	if (gameState.agentState == STATE_DEAD) {
		Play::UpdateGameObject(obj_agent8);
	}
	else {
		Play::UpdateGameObject(obj_agent8, 1);
		Play::DrawObjectRotated(obj_agent8);
	}
}

void UpdateAsteroids() {
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int>asteroid_list = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
	
	for (int asteroid_id : asteroid_list) {
		GameObject& asteroid = Play::GetGameObject(asteroid_id);

		Play::UpdateGameObject(asteroid, 1);
		Play::DrawObjectRotated(asteroid);

		if (gameState.agentState == STATE_FLY && Play::IsColliding(obj_agent8, asteroid)) {
			asteroid.type = TYPE_ASTEROID_HOME;
			gameState.agentState = STATE_ATTACH;
			Play::SetSprite(obj_agent8, "spr_agent8_left_strip7", 0);
		}		
	}
}

void UpdateMeteor() {
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int>meteor_list = Play::CollectGameObjectIDsByType(TYPE_METEOR);

	for (int meteor_id : meteor_list) {
		GameObject& meteor = Play::GetGameObject(meteor_id);
		Play::SetSprite(meteor, "meteor", 0.18f);
		Play::UpdateGameObject(meteor, 1);
		Play::DrawObjectRotated(meteor);


		if (Play::IsColliding(meteor, obj_agent8)) {
			gameState.agentState = STATE_DEAD;
		}
	}
}

void UpdateGem() {
	GameObject& asteroid = Play::GetGameObjectByType(TYPE_ASTEROID);
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	std::vector<int>gem_list = Play::CollectGameObjectIDsByType(TYPE_GEM);


	for (int gem_id : gem_list) {
		GameObject& gem = Play::GetGameObject(gem_id);		
		bool hasCollided = false;
		
		if(gameState.gemCooldown <= 0 && Play::IsColliding(gem, obj_agent8)) {
			hasCollided = true;
			gameState.remainingGems--; 
			Play::PlayAudio("reward");
		}
		
		Play::UpdateGameObject(gem, 1);
		Play::DrawObject(gem);

		if (hasCollided == true) {

			Play::DestroyGameObject(gem_id);
		}
	}	
}

void UpdateAsteroidPieces() {
	std::vector<int>vPieces = Play::CollectGameObjectIDsByType(TYPE_ASTEROID_PIECES);

	for (int id_piece : vPieces) {
		GameObject& obj_piece = Play::GetGameObject(id_piece);  
		
		Play::UpdateGameObject(obj_piece);
		Play::DrawObjectRotated(obj_piece);	
	}
}

void UpdateParticles() {
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	int id_particle = Play::CreateGameObject(TYPE_PARTICLES, obj_agent8.pos, 2, "particle"); 
	GameObject& particle = Play::GetGameObject(id_particle);
	particle.timeStamp = gameState.gameTime;
	particle.velocity = { Play::RandomRollRange(-50, 50)/100.f, Play::RandomRollRange(-50,50)/100.f };
		
	std::vector<int>vParticles = Play::CollectGameObjectIDsByType(TYPE_PARTICLES);
	for (int particle_id : vParticles) {
		GameObject& particle  = Play::GetGameObject(particle_id);
		float lifetime = gameState.gameTime - particle.timeStamp;
		Play::UpdateGameObject(particle);
		Play::DrawObjectTransparent(particle, (lifetime - 1.0f)/-8.8f );

		if (lifetime > 0.5f) {
			Play::DestroyGameObject(particle_id);
		}
	}
}


void UpdateLevelStart(){
	if (gameState.agentState == STATE_DEAD) {
		Play::MoveSpriteOrigin("spr_asteroid_strip", 13, 9);
		int id_asteroid_1 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 200 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_1 = Play::GetGameObject(id_asteroid_1);
		asteroid_1.velocity = { -1, 2 };
		asteroid_1.animSpeed = (1.0f);
		asteroid_1.rotation = 115.5;

		int id_asteroid_2 = Play::CreateGameObject(TYPE_ASTEROID, { 0, 500 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_2 = Play::GetGameObject(id_asteroid_2);
		asteroid_2.velocity = { 1, 1 };
		asteroid_2.animSpeed = (1.0f);
		asteroid_2.rotation = 120;

		int id_meteor = Play::CreateGameObject(TYPE_METEOR, { 1280, 10 }, 50, "meteor");
		GameObject& meteor = Play::GetGameObject(id_meteor);
		meteor.velocity = { -1 , 1 };
		meteor.rotation = 115.56;
		gameState.agentState = STATE_APPEAR;
	}

	if (gameState.level == 2 ) {
		gameState.remainingGems = 3;
		int id_asteroid_1 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 200 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_1 = Play::GetGameObject(id_asteroid_1);
		asteroid_1.velocity = { -1, 2 };
		asteroid_1.animSpeed = (1.0f);
		asteroid_1.rotation = 115.5;

		int id_asteroid_2 = Play::CreateGameObject(TYPE_ASTEROID, { 0, 500 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_2 = Play::GetGameObject(id_asteroid_2);
		asteroid_2.velocity = { 1, 1 };
		asteroid_2.animSpeed = (1.0f);
		asteroid_2.rotation = 120;

		int id_asteroid_3 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 500 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_3 = Play::GetGameObject(id_asteroid_3);
		asteroid_3.velocity = { 1, 1 };
		asteroid_3.animSpeed = (1.0f);
		asteroid_3.rotation = 120;

		int id_meteor_1 = Play::CreateGameObject(TYPE_METEOR, { 1280, 10 }, 50, "meteor");
		GameObject& meteor_1 = Play::GetGameObject(id_meteor_1);
		meteor_1.velocity = { -1 , 1 };
		meteor_1.rotation = 115.56;

		int id_meteor_2 = Play::CreateGameObject(TYPE_METEOR, { 0, 10 }, 50, "meteor");
		GameObject& meteor_2 = Play::GetGameObject(id_meteor_2);
		meteor_2.velocity = { 1 , 1 };
		meteor_2.rotation = 107;

		gameState.agentState = STATE_APPEAR;
	}
	if (gameState.level == 3) {
		gameState.remainingGems = 3;
		int id_asteroid_1 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 200 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_1 = Play::GetGameObject(id_asteroid_1);
		asteroid_1.velocity = { -2, 3 };
		asteroid_1.animSpeed = (1.0f);
		asteroid_1.rotation = 115.5;

		int id_asteroid_2 = Play::CreateGameObject(TYPE_ASTEROID, { 0, 500 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_2 = Play::GetGameObject(id_asteroid_2);
		asteroid_2.velocity = { 2, 2 };
		asteroid_2.animSpeed = (1.0f);
		asteroid_2.rotation = 120;

		int id_asteroid_3 = Play::CreateGameObject(TYPE_ASTEROID, { 900, 500 }, 50, "spr_asteroid_strip");
		GameObject& asteroid_3 = Play::GetGameObject(id_asteroid_3);
		asteroid_3.velocity = { 2, 2 };
		asteroid_3.animSpeed = (1.0f);
		asteroid_3.rotation = 120;

		int id_meteor_1 = Play::CreateGameObject(TYPE_METEOR, { 1280, 10 }, 50, "meteor");
		GameObject& meteor_1 = Play::GetGameObject(id_meteor_1);
		meteor_1.velocity = { -2 , 2 };
		meteor_1.rotation = 115.56;

		int id_meteor_2 = Play::CreateGameObject(TYPE_METEOR, { 0, 10 }, 50, "meteor");
		GameObject& meteor_2 = Play::GetGameObject(id_meteor_2);
		meteor_2.velocity = { 2 , 2 };
		meteor_2.rotation = 107;

		gameState.agentState = STATE_APPEAR;
	}
	
}

int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}