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
	STATE_BOSS_CASTING,
	STATE_BOSS_FIREBALL,
	STATE_BOSS_CHASE,
	STATE_BOSS_CLEAVE,
	STATE_BOSS_HIT,
	STATE_BOSS_SMASH,
	STATE_BOSS_SUMMON,
	STATE_TEST_IDLE

};

struct GameState {

	int attackCooldown = 0;
	int bossIdleCooldown = 0;
	int castingCooldown = 0;
	int fireBallCooldown = 0;
	int fireBallsCreated = 0;
	int catTargetPositionX = 0;
	int catTargetPositionY = 0;
	int cleaveCooldown = 0;
	int hitBossCooldown = 5;
	int bossHealth = 0; //you already have boss health that's why you had a bug
	int playerHealth = 0; //if boss hit with fireball or minion remove quarter of heart, if boss hit with cleave remove half of heart
	int phase = 0;
	int minionsCreated = 0;
	int minionCooldown = 0;
	int minionMoveCooldown = 0;
	int bossChaseCooldown = 0;

	CatState catState = STATE_APPEAR;
	BossState bossState = STATE_BOSS_APPEAR;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_CAT,
	TYPE_BOSS,
	TYPE_FIREBALL,
	TYPE_SWORD,
	TYPE_BOSS_HIT,
	TYPE_MINION,
	TYPE_MINION_SPAWN
};

void UpdateCat();
void UpdateBoss();
void UpdateFireball();
void UpdateSuccessfulHit();
void DrawObjectXFlipped(GameObject& obj);
void UpdateMinion();
void UpdateMinionSpawn();



void MainGameEntry(PLAY_IGNORE_COMMAND_LINE) {
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::MoveSpriteOrigin("cat_idle", 50, 60);
	Play::MoveSpriteOrigin("cat_run", 50, 60);
	Play::MoveSpriteOrigin("cat_attack", 50, 60);
	Play::MoveSpriteOrigin("boss_idle", 145, 120);
	Play::MoveSpriteOrigin("boss_spell", 145, 120);
	Play::MoveSpriteOrigin("fireball", 15, 15);
	Play::MoveSpriteOrigin("explosion", 15, 15);
	Play::MoveSpriteOrigin("boss_walk", 145, 120);
	Play::MoveSpriteOrigin("boss_cleave", 145, 120);
	Play::MoveSpriteOrigin("boss_smash", 145, 120);
	Play::MoveSpriteOrigin("minion_move", 140, 150);
	Play::MoveSpriteOrigin("minion_spawn", 50, 70);
	Play::MoveSpriteOrigin("minion_death", 140, 150);
	Play::LoadBackground("Data\\Backgrounds\\dungeonbackground.png");
	//Play::StartAudioLoop( "battle_theme" );

	int id_cat = Play::CreateGameObject(TYPE_CAT, { 500, 500 }, 50, "cat_idle");
	GameObject& cat = Play::GetGameObject(id_cat);

	int id_boss = Play::CreateGameObject(TYPE_BOSS, { 750, 250 }, 120, "boss_idle");

}


bool MainGameUpdate(float elapsedTime) {
	Play::DrawBackground();
	UpdateCat();
	UpdateBoss();
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

void UpdateCat() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	switch (gameState.catState) {

	case STATE_APPEAR:
		gameState.catState = STATE_IDLE;
		gameState.bossHealth = 500;
		gameState.phase = 1;
		break;

	case STATE_IDLE:
		Play::SetSprite(cat, "cat_idle", 0.2f);
		cat.velocity = { 0, 0 };
		cat.scale = 2.0f;
		gameState.attackCooldown--;
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_UP) || Play::KeyDown(VK_DOWN)) {
			gameState.catState = STATE_RUN;
		}
		if (Play::KeyPressed('A') && (gameState.attackCooldown <= 0)) {
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
		if (Play::KeyDown('A') && gameState.attackCooldown <= 0) {
			Play::SetSprite(cat, "cat_attack", 0.2f);
			if (Play::IsColliding(boss, cat)) {
				Play::PlayAudio("hit");
				if (cat.right_facing == true) {
					Play::CreateGameObject(TYPE_BOSS_HIT, { cat.pos.x + 50, cat.pos.y - 50 }, 5, "successful_attack");
						gameState.bossHealth -= 50; //fixed issue, add this after code review
						if (gameState.bossHealth <= 250) {
							gameState.phase = 2;
						}
				}
				else if (cat.right_facing == false) {
					Play::CreateGameObject(TYPE_BOSS_HIT, { cat.pos.x - 120, cat.pos.y - 50 }, 5, "successful_attack");
						gameState.bossHealth -= 50;
						if (gameState.bossHealth <= 250) {
							gameState.phase = 2;
						}
				}

			}
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
	UpdateMinion();
	UpdateMinionSpawn();
	DrawObjectXFlipped(cat);
	Play::UpdateGameObject(cat);
}

void UpdateBoss() {
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);

	switch (gameState.bossState) {
	case STATE_BOSS_APPEAR:
		gameState.bossState = STATE_BOSS_IDLE;
		gameState.bossIdleCooldown = 200;
		break;

	case STATE_BOSS_IDLE:
		Play::SetSprite(boss, "boss_idle", 0.12f);
		gameState.bossIdleCooldown--;

		if (gameState.bossIdleCooldown <= 0) {
			gameState.fireBallsCreated = 0;
			gameState.bossState = STATE_BOSS_CASTING; //change this to an if statement for when you implement phases 
			gameState.castingCooldown = 30;
			gameState.fireBallCooldown = 25;
			gameState.minionCooldown = 30;
		}

		break;
	case STATE_BOSS_CASTING:
		Play::SetSprite(boss, "boss_spell", 0.12f);
		boss.velocity = { 0, 0 };
		gameState.minionsCreated = 0;
		gameState.castingCooldown--;
		if (gameState.castingCooldown <= 0) {
			if (gameState.phase == 1) {
				gameState.bossState = STATE_BOSS_FIREBALL;
			}
			if (gameState.phase == 2) {
				gameState.bossState = STATE_BOSS_SUMMON;
			}
		}
		break;

	case STATE_BOSS_FIREBALL:
		gameState.fireBallCooldown--;
		if (gameState.fireBallCooldown <= 0) {
			gameState.catTargetPositionX = cat.pos.x;
			gameState.catTargetPositionY = cat.pos.y;
			if (boss.right_facing == true) {
				int id_fireball = Play::CreateGameObject(TYPE_FIREBALL, { boss.pos.x + 150, boss.pos.y - 40 }, 10, "fireball");
				GameObject& fireball = Play::GetGameObject(id_fireball);
				Play::PointGameObject(fireball, 3.0f, gameState.catTargetPositionX, gameState.catTargetPositionY);
				gameState.fireBallsCreated++;
				gameState.fireBallCooldown = 25;
			}
			else if (boss.right_facing == false) {
				int id_fireball = Play::CreateGameObject(TYPE_FIREBALL, { boss.pos.x - 150, boss.pos.y - 40 }, 10, "fireball");
				GameObject& fireball = Play::GetGameObject(id_fireball);
				Play::PointGameObject(fireball, 3.0f, gameState.catTargetPositionX, gameState.catTargetPositionY);
				gameState.fireBallsCreated++;
				gameState.fireBallCooldown = 35;
			}

		}
		if (gameState.fireBallsCreated == 6) {
			gameState.bossState = STATE_BOSS_CHASE;
			if (Play::IsColliding(boss, cat)) {
				gameState.bossState = STATE_BOSS_CLEAVE;

			}
		}


		break;

	case STATE_BOSS_CHASE:
		Play::SetSprite(boss, "boss_walk", 0.25f);
		Play::PointGameObject(boss, 1.5f, cat.pos.x, cat.pos.y);
		if (Play::IsColliding(boss, cat)) {


			gameState.cleaveCooldown = 25;
			if (gameState.phase == 1) {
				gameState.bossState = STATE_BOSS_CLEAVE; //randomise attacks, so put if randomiser is below 10 due cleave, if between 10 and 20 do spell if phase 1 etc...
			} //FIX THIS  so cleaves also in phase 1, but after a timer boss starts casting
			else if (gameState.phase == 2) {
				gameState.bossState = STATE_BOSS_CASTING;
			}
		}
		break;

	case STATE_BOSS_CLEAVE:
		// include warning sound effect
		boss.velocity = { 0, 0 };
		gameState.cleaveCooldown--;
		if (gameState.cleaveCooldown <= 0) {
			Play::SetSprite(boss, "boss_cleave", 0.25f);

			if (boss.frame == 10) {//&& inivisible game object is colliding with cat then reduce cat's health and play hit sounds and animation 
				if (boss.right_facing == true) {
					Play::CreateGameObject(TYPE_SWORD, { boss.pos.x + 170, boss.pos.y + 40 }, 50, "");
				}
				else if (boss.right_facing == false) {
					Play::CreateGameObject(TYPE_SWORD, { boss.pos.x - 170, boss.pos.y + 40 }, 50, "");
				}
			}
			if (boss.frame == 15) {
				Play::DestroyGameObjectsByType(TYPE_SWORD);
				gameState.bossIdleCooldown = 50;
				gameState.bossState = STATE_BOSS_IDLE;

			}
		}
		break;

	case STATE_BOSS_SMASH:
		Play::SetSprite(boss, "boss_smash", 0.1f);
		if (boss.frame == 1) {
			boss.velocity = { 0,0 };
			gameState.catTargetPositionX = cat.pos.x;
			gameState.catTargetPositionY = cat.pos.y;
		}

		if (boss.frame >= 5) {
			if ((boss.pos.x <= gameState.catTargetPositionX + 10) && (boss.pos.y <= gameState.catTargetPositionY +10)) {
				boss.velocity = { 0, 0 }; 
			}
			else if ((boss.pos.x != gameState.catTargetPositionX) || (boss.pos.y != gameState.catTargetPositionY)) {

				Play::PointGameObject(boss, 15, gameState.catTargetPositionX, gameState.catTargetPositionY);
			}
		}

		if (boss.frame == 18) {
			boss.velocity = { 0, 0 };
			gameState.bossChaseCooldown = 120;
			gameState.bossState = STATE_TEST_IDLE; 
		}
		break;

	case STATE_BOSS_SUMMON:
		gameState.minionCooldown--;
		if (gameState.minionCooldown <= 0) {
			int minion_id = Play::CreateGameObject(TYPE_MINION, { Play::RandomRollRange(50, 1100), Play::RandomRollRange(50, 680) }, 62, "minion_move");
			GameObject& minion = Play::GetGameObject(minion_id);
			Play::CreateGameObject(TYPE_MINION_SPAWN, minion.pos, 5, "minion_spawn");
			gameState.minionsCreated++;
			gameState.minionCooldown = 25;
			gameState.minionMoveCooldown = 45;
		}

		if (gameState.minionsCreated == 5) {
			gameState.bossState = STATE_BOSS_SMASH;
		}

		break;

	case STATE_TEST_IDLE:
		Play::SetSprite(boss, "boss_idle", 0.12f);
		boss.velocity = { 0,0 };
		break;
	}

	
	DrawObjectXFlipped(boss);
	Play::UpdateGameObject(boss);
	UpdateFireball();
	UpdateSuccessfulHit();

}

void SetUpFlipMatrix(bool face_right, Matrix2D& flipMat, Vector2D pos) {
	flipMat = MatrixIdentity();
	if (face_right == false) {
		flipMat.row[0].x = -2.0f;
		flipMat.row[1].y = 2.0f;
		flipMat.row[2].x = pos.x;
		flipMat.row[2].y = pos.y;
	}
	else {
		flipMat.row[0].x = 2.0f;
		flipMat.row[1].y = 2.0f;
		flipMat.row[2].x = pos.x;
		flipMat.row[2].y = pos.y;
	}
}



void DrawObjectXFlipped(GameObject& obj) {
	Matrix2D flipMat = MatrixIdentity();

	if (obj.velocity.x < 0) {
		obj.right_facing = false;
	}
	else if (obj.velocity.x > 0) {	
		obj.right_facing = true;
	}
	
	SetUpFlipMatrix(obj.right_facing, flipMat, obj.pos);
	Play::DrawSpriteTransformed(obj.spriteId, flipMat, obj.frame);
}

void UpdateFireball() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	std::vector<int>fireball_list = Play::CollectGameObjectIDsByType(TYPE_FIREBALL);

	for (int fireball_id : fireball_list) {
		GameObject& fireball = Play::GetGameObject(fireball_id);
		fireball.scale = 2.0f;
		fireball.animSpeed = 2.0f;
		Play::UpdateGameObject(fireball);
		Play::DrawObjectRotated(fireball); //find a way to make it appear with 0 transparency and then full transparency, this is used to update scale

		if (Play::IsColliding(fireball, cat)) {
			Play::SetSprite(fireball, "explosion", 0.2f);
			if (fireball.frame == 10) {
				Play::DestroyGameObject(fireball_id);
			}
		}
	}
}

void UpdateMinion() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	std::vector<int>minion_list = Play::CollectGameObjectIDsByType(TYPE_MINION);

	for (int minion_id : minion_list) {
		gameState.minionMoveCooldown--;
		GameObject& minion = Play::GetGameObject(minion_id);
		minion.animSpeed = 0.15f;
		minion.scale = 2.0f;

		Play::UpdateGameObject(minion);
		DrawObjectXFlipped(minion);
		

		if (Play::IsColliding(minion, cat) && (gameState.catState == STATE_ATTACK)) {
			Play::SetSprite(minion, "minion_death", 0.25f);
			minion.has_been_attacked = true;
		}
		
		if (minion.has_been_attacked == true) {
			minion.velocity = { 0,0 };
			if (minion.frame == 12) {
				Play::DestroyGameObject(minion_id); //add in reduce transparency over time, when transparency <= 0 destroy game object
			}
		}
		else if(minion.has_been_attacked == false){
			Play::PointGameObject(minion, 1, cat.pos.x, cat.pos.y);
		}
	}
}


void UpdateMinionSpawn() { //had to create separate function for magic spawn effect so animations would keep on udpating, did not update whilst it UpdateMinion()
	std::vector<int>minion_spawn_list = Play::CollectGameObjectIDsByType(TYPE_MINION_SPAWN);

	for (int minion_spawn_id : minion_spawn_list) {
		GameObject& minion_spawn = Play::GetGameObject(minion_spawn_id);
		minion_spawn.animSpeed = 0.2f;
		Play::UpdateGameObject(minion_spawn);
		Play::DrawObject(minion_spawn);
		if (minion_spawn.frame == 10) {
			Play::DestroyGameObject(minion_spawn_id);
		}
	}
}

void UpdateSuccessfulHit() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	std::vector<int>successful_hit_list = Play::CollectGameObjectIDsByType(TYPE_BOSS_HIT);

	for (int successful_hit_id : successful_hit_list) {
		GameObject& successful_hit = Play::GetGameObject(successful_hit_id);
		successful_hit.animSpeed = 0.5f;
		Play::DrawObject(successful_hit);
		Play::UpdateGameObject(successful_hit);

		if (successful_hit.frame == 5) {
			Play::DestroyGameObject(successful_hit_id);
		}
	}
}

int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}