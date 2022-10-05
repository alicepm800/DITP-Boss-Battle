#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#define PLAY_ADD_GAMEOBJECT_MEMBERS 
#include "Play.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

enum CatState
{
	STATE_IDLE = 0,
	STATE_RUN

};

struct GameState {

	float gameTime = 0;
	int spriteId = 0;
	
	CatState catState = STATE_IDLE;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_KNIGHT
};

void UpdateKnight();
void HandlePlayerControls();


void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ){
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins;
	Play::LoadBackground( "Data\\Backgrounds\\dungeonbackground.png" );
	//Play::StartAudioLoop( "steady_piece_1" );

	int id_cat = Play::CreateGameObject(TYPE_KNIGHT, { 500, 500 }, 50, "cat_idle");
	GameObject& cat = Play::GetGameObject(id_cat);
	
}


bool MainGameUpdate( float elapsedTime ){
	Play::DrawBackground();
	UpdateKnight();
	
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

void UpdateKnight() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_KNIGHT);
	switch (gameState.catState) {

	case STATE_IDLE:
		Play::SetSprite(cat, "cat_idle", 0.2f);
		cat.velocity = { 0, 0 };
		cat.scale = 2.0f;
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT)) {
			gameState.catState = STATE_RUN;
		}
		break;

	case STATE_RUN:
		if (Play::KeyDown(VK_LEFT)) {
			Play::SetSprite(cat, "cat_run", 0.2f);
			cat.velocity = { -2, 0 };
		}
		else if (Play::KeyDown(VK_RIGHT)) {
			Play::SetSprite(cat, "cat_run", 0.2f);
			cat.velocity = { 2, 0 };
		}
		else gameState.catState = STATE_IDLE;
			
	}
	Play::UpdateGameObject(cat);
	Play::DrawObjectRotated(cat);
}

void HandlePlayerControls() {

}


int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}