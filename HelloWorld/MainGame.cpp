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
	STATE_RUN

};

struct GameState {

	float gameTime = 0;
	int spriteId = 0;
	
	CatState catState = STATE_APPEAR;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_CAT
};

void UpdateCat();
void DrawObjectXFlipped(GameObject& obj);



void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ){
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins;
	Play::MoveSpriteOrigin("cat_idle", 50, 60);
	Play::MoveSpriteOrigin("cat_run", 50, 60);
	Play::LoadBackground( "Data\\Backgrounds\\dungeonbackground.png" );
	//Play::StartAudioLoop( "steady_piece_1" );

	int id_cat = Play::CreateGameObject(TYPE_CAT, { 500, 500 }, 50, "cat_idle");
	GameObject& cat = Play::GetGameObject(id_cat);
	
}


bool MainGameUpdate( float elapsedTime ){
	Play::DrawBackground();
	UpdateCat();
	
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
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_UP) || Play::KeyDown(VK_DOWN)) {
			gameState.catState = STATE_RUN;
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
	}
		DrawObjectXFlipped(cat);
		Play::UpdateGameObject(cat);
		
	
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