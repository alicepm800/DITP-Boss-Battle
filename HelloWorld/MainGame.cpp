#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#define PLAY_ADD_GAMEOBJECT_MEMBERS 
#include "Play.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

enum CatState
{
	STATE_APPEAR = 0,
	STATE_IDLE,
	STATE_RUN,
	STATE_ATTACK

};

enum BossState {
	STATE_BOSS_APPEAR = 0,
	STATE_BOSS_IDLE,
	STATE_BOSS_FOLLOW,
	STATE_PHASE_ONE_ATTACK,
	STATE_PHASE_TWO_ATTACK
};

struct GameState {

	int attackCooldown = 0;
	
	CatState catState = STATE_APPEAR;
	BossState bossState = STATE_BOSS_APPEAR;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_CAT,
	TYPE_BOSS
};

void UpdateCat();
void UpdateBoss();
void DrawObjectXFlipped(GameObject& obj);



void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ){
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	
	Play::MoveSpriteOrigin("cat_idle", 50, 60);
	Play::MoveSpriteOrigin("cat_run", 50, 60);
	Play::MoveSpriteOrigin("cat_attack", 50, 60);
	Play::MoveSpriteOrigin("boss_idle", 145, 120);
	Play::LoadBackground( "Data\\Backgrounds\\dungeonbackground.png" );
	//Play::StartAudioLoop( "steady_piece_1" );

	int id_cat = Play::CreateGameObject(TYPE_CAT, { 500, 500 }, 50, "cat_idle");
	GameObject& cat = Play::GetGameObject(id_cat);

	int id_boss = Play::CreateGameObject(TYPE_BOSS, { 750, 250 }, 80, "boss_idle");
	
}


bool MainGameUpdate( float elapsedTime ){
	Play::DrawBackground();
	UpdateCat();
	UpdateBoss();
	
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

void UpdateCat() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	switch (gameState.catState) {

	case STATE_APPEAR:
		gameState.catState = STATE_IDLE;
		break;

	case STATE_IDLE:
		Play::SetSprite(cat, "cat_idle", 0.2f);
		cat.velocity = { 0, 0 };
		cat.scale = 2.0f;
		gameState.attackCooldown--;
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_UP) || Play::KeyDown(VK_DOWN)) {
			gameState.catState = STATE_RUN;
		}
		if (Play::KeyPressed('B') && (gameState.attackCooldown <= 0)) {
			gameState.catState = STATE_ATTACK;
		}

		break;

	case STATE_RUN:

		if (Play::KeyDown(VK_LEFT)) {
			Play::SetSprite(cat, "cat_run", 0.2f);
			cat.velocity = { -5, 0 };

		}
		else if (Play::KeyDown(VK_RIGHT)) {
			Play::SetSprite(cat, "cat_run", 0.2f);
			cat.velocity = { 5, 0 };
		}
		else if (Play::KeyDown(VK_UP)) {
			cat.velocity = { 0, -5 };
			Play::SetSprite(cat, "cat_run", 0.2f);
		}
		else if (Play::KeyDown(VK_DOWN)) {
			cat.velocity = { 0, 5 };
			Play::SetSprite(cat, "cat_run", 0.2f);

		}
		else {

			gameState.catState = STATE_IDLE;
		}
		break;


	case STATE_ATTACK:	
		if (Play::KeyDown('B') && gameState.attackCooldown <= 0) {
			Play::SetSprite(cat, "cat_attack", 0.2f); 
			cat.has_attacked = true;
			if (cat.frame == 4) {  //put in this statement the number of health decreased if there is a collision 
				gameState.catState = STATE_IDLE;
				gameState.attackCooldown = 8;
			}
		}
		else {
			gameState.attackCooldown = 8;
			gameState.catState = STATE_IDLE;	
			
		}		
		break;
		
	}
	DrawObjectXFlipped(cat);
	Play::UpdateGameObject(cat);	
}

void UpdateBoss() {
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);

//	switch (gameState.bossState) {
	//case STATE_BOSS_APPEAR:

//	}
	Play::SetSprite(boss, "boss_idle", 0.15f);
	DrawObjectXFlipped(boss);
	Play::UpdateGameObject(boss);
	
}



void DrawObjectXFlipped(GameObject& obj) {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	Matrix2D flipMat = MatrixIdentity();
		
	if (Play::KeyDown(VK_LEFT)) {
		flipMat.row[0].x = -2.0f;
		flipMat.row[1].y = 2.0f;
		cat.right_facing = false;
	}
	else if (Play::KeyDown(VK_RIGHT)) {
		flipMat.row[0].x = 2.0f;
		flipMat.row[1].y = 2.0f;
		cat.right_facing = true;	
	}
	flipMat.row[2].x = obj.pos.x;
	flipMat.row[2].y = obj.pos.y;
	//when game object is idle
	if (obj.velocity.x == 0 && obj.velocity.y == 0) {
		if (obj.right_facing == true ){
			flipMat.row[0].x = -2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
		else {
			flipMat.row[0].x = 2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
	}
	//when game object is moving up
	if (obj.velocity.y < 0) {
		if (obj.right_facing == false) {
			flipMat.row[0].x = -2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
		else {
			flipMat.row[0].x = 2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
	}
	//when game object is moving down
	if (obj.velocity.y > 0){
		if (obj.right_facing == false) {
			flipMat.row[0].x = -2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
		else {
			flipMat.row[0].x = 2.0f;
			flipMat.row[1].y = 2.0f;
			flipMat.row[2].x = obj.pos.x;
			flipMat.row[2].y = obj.pos.y;
		}
	}
	Play::DrawSpriteTransformed(obj.spriteId, flipMat, obj.frame);
}


int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}