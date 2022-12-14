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
	STATE_SWORD_ATTACK,
	STATE_MAGIC_ATTACK
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
	STATE_BOSS_DEAD
};

enum PlayingState {
	STATE_START_SCREEN = 0,
	STATE_MENU_SCREEN,
	STATE_PLAY_SCREEN,
	STATE_GAME_OVER,
	STATE_WIN
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
	int bossHealth = 0; 
	int playerHealth = 0; 
	int phase = 0;
	int minionsCreated = 0;
	int minionCooldown = 0;
	int minionMoveCooldown = 0;
	int beenHitCounter = 0;
	bool hitBoxCreated = false;
	int hitByMinionTimer = 0;
	int magicBallsCreated = 0;
	bool gameStarted = false;
	bool titleFontWritten = false;
	int lastPosXToDraw = 0;//to inform the length of the boss' health bar so it dynamically changes relative to amount of health
	

	PlayingState playingState = STATE_START_SCREEN;
	CatState catState = STATE_APPEAR;
	BossState bossState = STATE_BOSS_APPEAR;
	
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_CAT,
	TYPE_BOSS,
	TYPE_HEART,
	TYPE_FIREBALL,
	TYPE_SWORD,
	TYPE_BOSS_HIT,
	TYPE_MINION,
	TYPE_MINION_SPAWN,
	TYPE_SMASH_HIT_BOX,
	TYPE_RIGHT_MAGIC_FIREBALL,
	TYPE_LEFT_MAGIC_FIREBALL,
	TYPE_UI_PANEL,
	TYPE_BOSS_HEALTH,
	TYPE_STAR
	
};

void UpdateCat();
void UpdateBoss();
void UpdateHealth();
void UpdateFireball();
void UpdateMagicFireball();
void UpdateSuccessfulHit();
void DrawObjectXFlipped(GameObject& obj);
void UpdateMinion();
void UpdateMinionSpawn();
void BossDead();
void UpdateGame();

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
	Play::MoveSpriteOrigin("boss_dead", 145, 120);
	Play::MoveSpriteOrigin("cat_magic", 50, 60);
	Play::MoveSpriteOrigin("magic_ball", 65, 70);
	Play::MoveSpriteOrigin("ui_panel", 25, 20);
	Play::LoadBackground("Data\\Backgrounds\\dungeonbackground.png");

	Play::CreateGameObject(TYPE_CAT, { 500, 500 }, 50, "cat_idle");
	Play::CreateGameObject(TYPE_BOSS, { 750, 250 }, 120, "boss_idle");
	int heart_id = Play::CreateGameObject(TYPE_HEART, { 50 , 25 }, 0, "full_health");
	GameObject& heart = Play::GetGameObject(heart_id);
	heart.scale = 4.0f;
	int ui_panel_id = Play::CreateGameObject(TYPE_UI_PANEL, { 640, 360 }, 0, "ui_panel");
	GameObject& ui_panel = Play::GetGameObject(ui_panel_id);
	ui_panel.scale = 16.0f;
}

bool MainGameUpdate(float elapsedTime) {
	Play::DrawBackground();
	UpdateCat();
	UpdateBoss();
	UpdateHealth();
	UpdateGame();
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

void UpdateGame() {
	GameObject& ui_panel = Play::GetGameObjectByType(TYPE_UI_PANEL);
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	std::vector<int>minion_list = Play::CollectGameObjectIDsByType(TYPE_MINION);
	std::vector<int>fireball_list = Play::CollectGameObjectIDsByType(TYPE_FIREBALL);
	std::vector<int>right_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_RIGHT_MAGIC_FIREBALL);
	std::vector<int>left_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_LEFT_MAGIC_FIREBALL);
	int segment_id = Play::GetSpriteId("health_bar_segment");

	switch (gameState.playingState){
		case STATE_START_SCREEN:
			Play::DrawObjectRotated(ui_panel);
			Play::DrawFontText("69px", "BOSS BATTLE", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 400 }, Play::CENTRE);
			Play::DrawFontText("28px", "press spacebar to start", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 200 }, Play::CENTRE);
			
			if (Play::KeyPressed(VK_SPACE)) {
				gameState.gameStarted = true;
				Play::StartAudioLoop("battle_theme");
				gameState.playingState = STATE_PLAY_SCREEN;
				gameState.lastPosXToDraw = 925; 
			}
			break;

		case STATE_PLAY_SCREEN:
			Play::DrawFontText("28px", "Move: arrows", { DISPLAY_WIDTH  - 1250, DISPLAY_HEIGHT - 200 }, Play::LEFT);
			Play::DrawFontText("28px", "Sword : A", { DISPLAY_WIDTH - 1250, DISPLAY_HEIGHT - 150 }, Play::LEFT);
			Play::DrawFontText("28px", "Magic: spacebar", { DISPLAY_WIDTH - 1250, DISPLAY_HEIGHT - 100 }, Play::LEFT);
			Play::DrawRect({ 340, 650 }, { 940, 700 }, Play::cWhite);
			for (int x = 345; x < gameState.lastPosXToDraw; x++) { 	
				Play::DrawSprite(segment_id, { x, 660 }, 0);
			
			}
			break;

		case STATE_GAME_OVER:
			Play::DrawObjectRotated(ui_panel);
			Play::DrawFontText("69px", "GAME OVER", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 400 }, Play::CENTRE);
			Play::DrawFontText("28px", "press space to restart", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 200 }, Play::CENTRE);
			cat.velocity = { 0,0 };
			cat.pos = { 500, 500 };
			boss.velocity = { 0,0 };
			boss.pos = { 720, 250 };
			Play::DestroyGameObjectsByType(TYPE_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_RIGHT_MAGIC_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_LEFT_MAGIC_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_MINION);
			gameState.catState = STATE_APPEAR;
			gameState.bossState = STATE_BOSS_APPEAR;
			gameState.gameStarted = false;
			if (Play::KeyPressed(VK_SPACE)) {
				gameState.gameStarted = true;
				gameState.lastPosXToDraw = 925;
				Play::StartAudioLoop("battle_theme");
				gameState.catState = STATE_IDLE;
				gameState.bossState = STATE_BOSS_IDLE;
				gameState.playingState = STATE_PLAY_SCREEN;
			}
			break;

		case STATE_WIN:
			Play::DrawObjectRotated(ui_panel);
			Play::DrawFontText("69px", "BOSS DEAD", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 400 }, Play::CENTRE);
			Play::DrawFontText("28px", "press space to play again", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 200 }, Play::CENTRE);
			cat.velocity = { 0,0 };
			cat.pos = { 500, 500 };
			boss.velocity = { 0,0 };
			boss.pos = { 720, 250 };
			Play::DestroyGameObjectsByType(TYPE_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_RIGHT_MAGIC_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_LEFT_MAGIC_FIREBALL);
			Play::DestroyGameObjectsByType(TYPE_MINION);
			gameState.catState = STATE_APPEAR;
			gameState.bossState = STATE_BOSS_APPEAR;
			gameState.gameStarted = false;
			if (Play::KeyPressed(VK_SPACE)) {
				gameState.gameStarted = true;
				gameState.lastPosXToDraw = 925;
				Play::StartAudioLoop("battle_theme");
				gameState.catState = STATE_IDLE;
				gameState.bossState = STATE_BOSS_IDLE;
				gameState.playingState = STATE_PLAY_SCREEN;
			}	
			break;
	}
}

void UpdateCat() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	switch (gameState.catState) {

		case STATE_APPEAR:

			gameState.playerHealth = 4;
			gameState.bossHealth = 3000; 
			gameState.phase = 1;
			if (gameState.gameStarted == true) {
				gameState.catState = STATE_IDLE;
			}
			break;

		case STATE_IDLE:
			Play::SetSprite(cat, "cat_idle", 0.2f);
			cat.velocity = { 0, 0 };
			cat.scale = 2.0f;
			if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_UP) || Play::KeyDown(VK_DOWN)) {
				gameState.catState = STATE_RUN;
			}
			if (Play::KeyPressed('A')) {
				gameState.catState = STATE_SWORD_ATTACK;
			}

			if (Play::KeyPressed(VK_SPACE)) {
				gameState.catState = STATE_MAGIC_ATTACK;
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
			if (Play::IsLeavingDisplayArea(cat)) {
				cat.pos = cat.oldPos;
			}
			break;
		case STATE_MAGIC_ATTACK:
			Play::SetSprite(cat, "cat_magic", 0.2f);
			gameState.magicBallsCreated++;
			if (gameState.magicBallsCreated == 1) {
				if (cat.right_facing == true) {
					Play::CreateGameObject(TYPE_RIGHT_MAGIC_FIREBALL, { cat.pos.x + 30, cat.pos.y }, 10, "magic_ball");
					Play::PlayAudio("magic");
				}
				else if (cat.right_facing == false) {
					Play::CreateGameObject(TYPE_LEFT_MAGIC_FIREBALL, { cat.pos.x - 30, cat.pos.y }, 10, "magic_ball");
					Play::PlayAudio("magic");
				}

			}
			if (cat.frame == 5) {
				gameState.magicBallsCreated = 0;
				gameState.catState = STATE_IDLE;
			}

			break;
		case STATE_SWORD_ATTACK:
			Play::SetSprite(cat, "cat_attack", 0.2f);
			if (Play::IsColliding(boss, cat)) {
				if (cat.right_facing == true) {
					if (cat.frame == 0) {
						Play::CreateGameObject(TYPE_BOSS_HIT, { cat.pos.x + 50, cat.pos.y - 50 }, 5, "successful_attack"); 
						Play::PlayAudio("enemy_hurt");
						gameState.bossHealth -= 50;
						gameState.lastPosXToDraw -= 10;
					}
					if (gameState.bossHealth <= 1500) {
						gameState.phase = 2;
					}
				}
				else if (cat.right_facing == false) {
					if (cat.frame == 0) {
						Play::CreateGameObject(TYPE_BOSS_HIT, { cat.pos.x - 120, cat.pos.y - 50 }, 5, "successful_attack"); 
						Play::PlayAudio("enemy_hurt");
						gameState.bossHealth -= 50;
						gameState.lastPosXToDraw -= 10;
					}
					if (gameState.bossHealth <= 1500) {
						gameState.phase = 2;
					}
				}

			}
			if (cat.frame == 4) {
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
		gameState.bossIdleCooldown = 200; 

		if (gameState.gameStarted == true) {
			gameState.bossState = STATE_BOSS_IDLE;
		}
		break;

	case STATE_BOSS_IDLE:
		Play::DestroyGameObjectsByType(TYPE_SMASH_HIT_BOX);
		Play::SetSprite(boss, "boss_idle", 0.12f);
		gameState.bossIdleCooldown--;

		if (gameState.bossIdleCooldown <= 0) {
			gameState.fireBallsCreated = 0;
			gameState.castingCooldown = 30;
			gameState.fireBallCooldown = 25;
			gameState.minionCooldown = 30;
			if (gameState.phase == 1) {
				gameState.bossState = STATE_BOSS_CASTING;
			}
			else if ((gameState.phase == 2) && (boss.has_cleaved_phase_two == true) && (gameState.minionsCreated > 0)) {
				gameState.bossState = STATE_BOSS_SMASH;
				boss.has_cleaved_phase_two = false;
			}
			else if ((gameState.phase == 2) && (boss.has_cleaved_phase_two == true) && (gameState.minionsCreated <= 0)) {
				gameState.bossState = STATE_BOSS_CASTING;
				boss.has_cleaved_phase_two = false;

			}
			else if ((gameState.phase == 2) && (boss.has_cleaved_phase_two == false)){
				gameState.bossState = STATE_BOSS_CHASE;
			}
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
				Play::PlayAudio("hit");
				Play::PointGameObject(fireball, 3.0f, gameState.catTargetPositionX, gameState.catTargetPositionY);
				gameState.fireBallsCreated++;
				gameState.fireBallCooldown = 25;
			}
			else if (boss.right_facing == false) {
				int id_fireball = Play::CreateGameObject(TYPE_FIREBALL, { boss.pos.x - 150, boss.pos.y - 40 }, 10, "fireball");
				GameObject& fireball = Play::GetGameObject(id_fireball);
				Play::PlayAudio("hit");
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
			gameState.bossState = STATE_BOSS_CLEAVE; 
		}
		break;

	case STATE_BOSS_CLEAVE:
		boss.velocity = { 0, 0 };
		gameState.cleaveCooldown--;
		if (gameState.cleaveCooldown <= 0) {
			Play::SetSprite(boss, "boss_cleave", 0.25f);

			if (boss.frame == 10) { 
				if (boss.right_facing == true) {
					int sword_id = Play::CreateGameObject(TYPE_SWORD, { boss.pos.x + 170, boss.pos.y + 40 }, 80, "");
					Play::PlayAudio("cleave");
					GameObject& sword = Play::GetGameObject(sword_id);
					if (Play::IsColliding(cat, sword)) {
						cat.cat_been_hit = true;
					}
				}
				else if (boss.right_facing == false) {
					int sword_id = Play::CreateGameObject(TYPE_SWORD, { boss.pos.x - 170, boss.pos.y + 40 }, 80, "");
					Play::PlayAudio("cleave");
					GameObject& sword = Play::GetGameObject(sword_id);
					if (Play::IsColliding(cat, sword)) {
						cat.cat_been_hit = true;
					}
				}
				
			}
			if (boss.frame == 15) {
				Play::DestroyGameObjectsByType(TYPE_SWORD);
				if (gameState.phase == 1) {
					gameState.bossIdleCooldown = 200;
				}
				else if (gameState.phase == 2) {
					gameState.bossIdleCooldown = 160;
				}
				if (cat.cat_been_hit == true) {
					gameState.playerHealth--;
					cat.cat_been_hit = false;
				}
				if (gameState.phase == 2) {
					boss.has_cleaved_phase_two = true;
				}
				gameState.bossState = STATE_BOSS_IDLE;

			}
		}
		break;

	case STATE_BOSS_SMASH:
		Play::SetSprite(boss, "boss_smash", 0.1f);
		if (boss.frame == 0) {
			boss.velocity = { 0,0 };
			gameState.catTargetPositionX = cat.pos.x;
			gameState.catTargetPositionY = cat.pos.y;
		}

		if (boss.frame >= 5) {
			if ((boss.pos.x <= gameState.catTargetPositionX + 10) && (boss.pos.y <= gameState.catTargetPositionY +10)) {
				boss.velocity = { 0, 0 }; 
				if (gameState.hitBoxCreated == false) {
					Play::CreateGameObject(TYPE_SMASH_HIT_BOX, { boss.pos.x, boss.pos.y + 50 }, 100, " ");
					gameState.hitBoxCreated = true;
				}
			}
			else if ((boss.pos.x != gameState.catTargetPositionX) || (boss.pos.y != gameState.catTargetPositionY)) {

				Play::PointGameObject(boss, 15, gameState.catTargetPositionX, gameState.catTargetPositionY);
			}
		}

		if (boss.frame == 10) {
			Play::PlayAudio("smash");
		}

		if (boss.frame == 18) {
			std::vector<int>vHitBoxes = Play::CollectGameObjectIDsByType(TYPE_SMASH_HIT_BOX);
			for (int id_hit_box : vHitBoxes) {
				GameObject& hit_box = Play::GetGameObject(id_hit_box);
				if (Play::IsColliding(cat, hit_box)) {
					cat.cat_been_hit = true;
					if (cat.cat_been_hit == true) {
						gameState.playerHealth--;
						cat.cat_been_hit = false;
					}
				}
			}
			gameState.hitBoxCreated = false;
			gameState.bossIdleCooldown = 120;
			gameState.bossState = STATE_BOSS_IDLE; 
		}
		break;

	case STATE_BOSS_SUMMON:
		gameState.minionCooldown--;
		if (gameState.minionCooldown <= 0) {
			int minion_id = Play::CreateGameObject(TYPE_MINION, { Play::RandomRollRange(50, 1100), Play::RandomRollRange(50, 680) }, 40, "minion_move");
			GameObject& minion = Play::GetGameObject(minion_id);
			Play::CreateGameObject(TYPE_MINION_SPAWN, minion.pos, 5, "minion_spawn");
			Play::PlayAudio("hit");
			gameState.minionsCreated++;
			gameState.minionCooldown = 25;
			gameState.minionMoveCooldown = 45;
		}

		if (gameState.minionsCreated == 3) {
			gameState.bossState = STATE_BOSS_SMASH;
		}

		break;

	case STATE_BOSS_DEAD:
		if (boss.frame == 0) {
			Play::StopAudioLoop("battle_theme");
			Play::PlayAudio("boss_death");
		}
		Play::SetSprite(boss, "boss_dead", 0.1f);
		boss.velocity = { 0,0 };
		if (boss.frame == 22) {
			boss.animSpeed = 0;
			gameState.playingState = STATE_WIN;
		}
		break;
	}

	if ((gameState.bossState == STATE_BOSS_DEAD) && (boss.frame == 22)) {
		Play::DrawObjectTransparent(boss, 0.0f);
	}
	else {
		DrawObjectXFlipped(boss);
	}
	Play::UpdateGameObject(boss);
	BossDead();
	UpdateFireball();
	UpdateMagicFireball();
	UpdateSuccessfulHit();
}


void UpdateHealth() {
	GameObject& heart = Play::GetGameObjectByType(TYPE_HEART);
	if (gameState.playingState == STATE_PLAY_SCREEN) {
		Play::DrawObjectRotated(heart, 1.0f);
	}
	
	if (gameState.playerHealth == 4) {
		Play::SetSprite(heart, "full_health", 0.0f);
	}

	if (gameState.playerHealth == 3) {
		Play::SetSprite(heart, "three_quarters_health", 0.0f);
	}

	if (gameState.playerHealth == 2) {
		Play::SetSprite(heart, "half_health", 0.0f);
	}

	if (gameState.playerHealth == 1) {
		Play::SetSprite(heart, "quarter_health", 0.0f);
	}	

	if (gameState.playerHealth == 0) {
		Play::StopAudioLoop("battle_theme");
		gameState.playingState = STATE_GAME_OVER;
	}
}

void BossDead() {
	if (gameState.bossHealth <= 100) {
		gameState.bossState = STATE_BOSS_DEAD;
	}
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
	std::vector<int>fireball_list = Play::CollectGameObjectIDsByType(TYPE_FIREBALL);

	for (int fireball_id : fireball_list) {
		GameObject& fireball = Play::GetGameObject(fireball_id);
		fireball.scale = 2.0f;
		fireball.animSpeed = 2.0f;
		Play::UpdateGameObject(fireball);
		Play::DrawObjectRotated(fireball); 

		if (Play::IsColliding(fireball, cat)) {
			Play::SetSprite(fireball, "explosion", 0.2f); 
			if (fireball.frame == 0) {
				gameState.playerHealth--;
				fireball.cat_been_hit = true;
			}
		}

		if (((fireball.cat_been_hit == true) && (fireball.frame == 9)) 
			|| (!Play::IsVisible(fireball) )) { 
				Play::DestroyGameObject(fireball_id);	
		}
	}
}


void UpdateMagicFireball() {
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	std::vector<int>right_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_RIGHT_MAGIC_FIREBALL);
	std::vector<int>left_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_LEFT_MAGIC_FIREBALL);

	for (int right_magic_ball_id : right_magic_ball_list) {
		GameObject& magic_ball = Play::GetGameObject(right_magic_ball_id);
		magic_ball.animSpeed = 0.2f;
		
		if (magic_ball.frame == 5) {
			magic_ball.frame = 3;
		}

		if (Play::IsColliding(magic_ball, boss)) {
			magic_ball.boss_been_hit = true;
			Play::PlayAudio("enemy_hurt");
		}

		if (magic_ball.boss_been_hit == true) {
			gameState.bossHealth -=  10;
			gameState.lastPosXToDraw -= 2;
			Play::DestroyGameObject(right_magic_ball_id);	
		}
		else if (magic_ball.boss_been_hit == false) {
			magic_ball.velocity = { 4, 0 };
			Play::UpdateGameObject(magic_ball);
			Play::DrawObjectRotated(magic_ball);
		}
		
	}

	for (int left_magic_ball_id : left_magic_ball_list) {
		GameObject& magic_ball = Play::GetGameObject(left_magic_ball_id);
		magic_ball.animSpeed = 0.2f;

		if (magic_ball.frame == 5) {
			magic_ball.frame = 3;
		}
		if (Play::IsColliding(magic_ball, boss)) {
			magic_ball.boss_been_hit = true;
			Play::PlayAudio("enemy_hurt");
		}

		if ((magic_ball.boss_been_hit == true) ) {
			gameState.bossHealth -= 10;
			gameState.lastPosXToDraw -= 2;
			Play::DestroyGameObject(left_magic_ball_id);
		}
		else if (magic_ball.boss_been_hit == false) {
			magic_ball.velocity = { -4, 0 };
			
			Play::UpdateGameObject(magic_ball);
			Play::DrawObjectRotated(magic_ball);
		}
	}
	
}

void UpdateMinion() {
	GameObject& cat = Play::GetGameObjectByType(TYPE_CAT);
	GameObject& boss = Play::GetGameObjectByType(TYPE_BOSS);
	std::vector<int>minion_list = Play::CollectGameObjectIDsByType(TYPE_MINION);
	std::vector<int>right_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_RIGHT_MAGIC_FIREBALL);
	std::vector<int>left_magic_ball_list = Play::CollectGameObjectIDsByType(TYPE_LEFT_MAGIC_FIREBALL);

	for (int minion_id : minion_list) {
		gameState.minionMoveCooldown--;
		GameObject& minion = Play::GetGameObject(minion_id);
		minion.animSpeed = 0.15f;
		minion.scale = 2.0f;

		Play::UpdateGameObject(minion);
		DrawObjectXFlipped(minion);
		

		if (Play::IsColliding(minion, cat) && (gameState.catState == STATE_SWORD_ATTACK)) { 
			Play::SetSprite(minion, "minion_death", 0.25f);
			minion.radius == 0;
			minion.has_been_attacked = true;
		}
		else if (Play::IsColliding(minion, cat) && (gameState.catState != STATE_SWORD_ATTACK) && (minion.minion_dead == false)) {
			cat.cat_been_hit = true;
			if (minion.minion_timer_started == false) {
				gameState.hitByMinionTimer = 60;
				minion.minion_timer_started = true;
			}
		}

		if ((cat.cat_been_hit == true) && (minion.minion_timer_started == true)); {
			gameState.hitByMinionTimer--;
			if (gameState.hitByMinionTimer == 59) {
				gameState.playerHealth--;
				cat.cat_been_hit = false;
			}
		}

		if (!Play::IsColliding(minion, cat)) {
			minion.minion_timer_started = false;
		}

		for (int id_right_magic_ball : right_magic_ball_list) {
			GameObject& magic_ball = Play::GetGameObject(id_right_magic_ball);
			if (Play::IsColliding(magic_ball, minion)) {
				minion.minion_dead = true;
				Play::PlayAudio("minion_death");
				Play::SetSprite(minion, "minion_death", 0.25f);
				minion.radius = 0;
				Play::DestroyGameObject(id_right_magic_ball);
				minion.has_been_attacked = true;
			}
		}

		for (int id_left_magic_ball : left_magic_ball_list) {
			GameObject& magic_ball = Play::GetGameObject(id_left_magic_ball);
			if (Play::IsColliding(magic_ball, minion)) {
				minion.minion_dead = true;
				Play::PlayAudio("minion_death");
				minion.has_been_attacked = true;
				Play::SetSprite(minion, "minion_death", 0.25f);
				minion.radius = 0;
				Play::DestroyGameObject(id_left_magic_ball);
				minion.has_been_attacked = true;
			}
		}
		if (minion.has_been_attacked == true) {
			minion.velocity = { 0,0 };
			if (minion.frame == 11) {
				Play::DestroyGameObject(minion_id);
				gameState.minionsCreated--;
			}
		}
		else if(minion.has_been_attacked == false){
			Play::PointGameObject(minion, 1, cat.pos.x, cat.pos.y);
		}
	}
}

void UpdateMinionSpawn() { 
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