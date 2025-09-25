#ifndef ZINPUT_H
#define ZINPUT_H
#include "gamez/zSystem/zsys.h"

class CInput;
class CPad;
class CKeyboard;

extern float pad_dt;

enum KEY_STATE
{
	KEY_UP,
	KEY_FALLING,
	KEY_DOWN,
	KEY_RISING
};

enum class PAD_BUTTON
{
	PAD_START,
	PAD_SELECT,
	PAD_RIGHT,
	PAD_LEFT,
	PAD_UP,
	PAD_DOWN,
	PAD_TRIANGLE,
	PAD_CIRCLE,
	PAD_SQUARE,
	PAD_CROSS,
	PAD_TRIGGER_R1,
	PAD_TRIGGER_R2,
	PAD_TRIGGER_L1,
	PAD_TRIGGER_L2,
	PAD_RIGHT_STICK,
	PAD_LEFT_STICK,
	PAD_MAX_BUTTONS
};

s32 InitKeyreadStuff();

class CStickType
{
	friend class CPad;
private:
	f32 m_minStickVelocity;
	f32 m_absoluteDeadZone;
	f32 m_velocityDeadZone;
	f32 m_valueForConstant;
	f32 m_valueForN;

	bool m_velocitySeparate;
	bool m_deadZoneSeparate;

	f32 m_minZeroVelocityTime;

	bool m_normalize;
	bool m_dilate;
};

class CInput
{
public:
	// Disallow construction of a purely static class
	CInput() = delete;

	static void Init();
	static void Uninit();
	static void Tick(f32 dT);
	static CPad* CreatePad(u32 slot);
	static void DeletePad(u32 slot);
	static void Flush();
	static int OpenPadIO();
	static void ClosePadIO();
	static bool m_init;

	static CPad* m_pads[2];
	static CKeyboard* m_keyboard;
};

class CPad
{
public:
	CPad() { }
	CPad(u32 port, u32 slot);

	void SetButtonState(PAD_BUTTON button, KEY_STATE state);
	
	void Flush();
	
	void Tick(f32 delta);

	bool GetTwoButtons(PAD_BUTTON button1, PAD_BUTTON button2);
	bool IsOpen();

	s32 m_port;
	s32 m_slot;
	s32 m_open;

	u8 m_ActData;
	bool m_IsAnalog;

	KEY_STATE m_state[16];
	u8 m_singleClick[16];
	u8 m_doubleClick[16];
	f32 m_click[16];

	long128* m_DmaBuf;

	u8 m_InData[32];
	u32 m_Data;
	u32 m_AnalogLeft[2];
	u32 m_AnalogRight[2];

	s32 m_Phase;
	s32 m_State;
	s32 m_Id;
	s32 m_ExId;
	s32 m_actuator;
	s32 m_NumActuators;

	f32 m_LeftStick[2];
	f32 m_RightStick[2];
	bool m_swapSticks;

	f32 m_LeftStickDilated;
	f32 m_RightStickDilated;
	
	bool m_DataIsGood;

	u8 m_ActAlign[6];

	s32 m_RotationVibration;
	s32 m_RotationSpeed;
	s32 m_ControlNum;
	s32 m_Consuption;

	u8 m_lastStickRaw[4];
	f32 m_lastStickVal[4];
	f32 m_maxStick[4];
	f32 m_minstick[4];
	f32 m_deadTime[4];

	CStickType m_stickType[2];
};

class CKeyboard
{
public:
	void Tick(f32 delta);
};
#endif