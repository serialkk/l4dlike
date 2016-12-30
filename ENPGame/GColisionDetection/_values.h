#pragma once
#define G_DEFINE_MAX_BASIC_ZOMBIE  5
#define G_DEFINE_CHAR_Y_POS_OFFSET 40.0f
#define G_DEFINE_SCORE_BASIC 100
#define G_DEFINE_DAMAGE_SHOTGUN_TO_PLAYER 10
#define G_DEFINE_DAMAGE_SHOTGUN_TO_ZOMBIE 100

#define G_MACRO_DEBUG_STR_INTERVAL 30


#define G_DEFINE_ANI_ZOMB_DIE L"ZOMBIE_DIE"
#define G_DEFINE_ANI_ZOMB_ATT L"ZOMBIE_ATTACK"
#define G_DEFINE_ANI_ZOMB_WLK L"ZOMBIE_WALK"
#define G_DEFINE_ANI_ZOMB_IDL L"ZOMBIE_IDLE"

#define G_DEFINE_UI_PLAYER1_HEALTH	2
#define G_DEFINE_UI_PLAYER2_HEALTH	4
#define G_DEFINE_UI_PLAYER1_BULLET	3
#define G_DEFINE_UI_PLAYER2_BULLET	5
#define G_DEFINE_UI_CROSSHAIR		6	
#define G_DEFINE_UI_CHATTING		13
#define G_DEFINE_UI_CHATTING_P1_IMG 14
#define G_DEFINE_UI_CHATTING_P2_IMG 15
#define G_DEFINE_UI_CHATTING_SPACE	16
#define G_DEFINE_UI_GAMEOVER		17

#define G_DEFINE_OBB_LAB D3DXVECTOR3(-95.0f, 0.0f, -85.0f), D3DXVECTOR3(93.0f, 435.0f, 125.0f)
#define G_DEFINE_OBB_CAR D3DXVECTOR3(-160.0f, 0.0f, -440.0f), D3DXVECTOR3(160.0f, 300.0f, 440.0f)
#define G_DEFINE_OBB_DROPSHIP_LAND D3DXVECTOR3(-42.0f, 0.0f, -75.0f), D3DXVECTOR3(42.0f, 60.0f, 115.0f)

#define G_DEFINE_HERO_1_POS_X -3258.0f
#define G_DEFINE_HERO_1_POS_Y 60.0f
#define G_DEFINE_HERO_1_POS_Z -4050.0f

enum G_OBJECT {
	G_OBJ_LAB = 0,
	G_OBJ_DROPSHIP,
	G_OBJ_CAR1,
	G_OBJ_CAR2,

	G_OBJ_LAB1,
	G_OBJ_LAB2,
	G_OBJ_LAB3,
	G_OBJ_LAB4,

	G_OBJ_LAB5,
	G_OBJ_LAB6,
	G_OBJ_LAB7,
	G_OBJ_LAB8,
	G_OBJ_LAB9,
	G_OBJ_LAB10,
	G_OBJ_LAB11,
	G_OBJ_LAB12,
	G_OBJ_LAB13,
	G_OBJ_LAB14,

	G_OBJ_LAB15,
	G_OBJ_LAB16,
	G_OBJ_LAB17,
	G_OBJ_LAB18,
	G_OBJ_LAB19,
	G_OBJ_LAB20,
	G_OBJ_LAB21,
	G_OBJ_LAB22,
	G_OBJ_LAB23,
	G_OBJ_LAB24,
	G_OBJ_LAB25,

	G_OBJ_LAB26,
	G_OBJ_LAB27,
	G_OBJ_LAB28,

	G_OBJ_CNT
};

enum G_WALL
{
	G_BB_WALL1 = 0,
	G_BB_WALL2,
	G_BB_WALL3,
	G_BB_WALL4,
	G_BB_CNT
};

enum G_ZOMB_ST {
	G_ZOMB_ST_WALK =0,
	G_ZOMB_ST_IDLE,
	G_ZOMB_ST_ATTACK,
	G_ZOMB_ST_DEAD,
	G_ZOMB_ST_CNT
};