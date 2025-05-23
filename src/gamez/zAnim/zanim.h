#pragma once
#include <string>

#include "gamez/zNode/znode.h"
#include "gamez/zMath/zmath.h"
#include "gamez/zReader/zrdr.h"
#include "gamez/zValve/zvalve.h"
#include "gamez/zSound/zsnd.h"

#define ANIMTYPE_UNKNOWN01 1
#define ANIMTYPE_CONDITION 2

class CZAnim;
class CZAnimEx;
class CZAnimMain;
class CZAnimHealth;
class CBlendable;
class CAnimsByFP;

class CSched_Manager;

class Particle;

namespace zar
{
	class CZAR;
}

namespace zdb
{
	class CTexHandle;
}

extern CZAnimMain ZAnim;

extern u32* seq_offset_stack;
extern u32 seq_offset_stack_size;

extern char* _local_wildcard_str;
extern char* _input_str_ptr;
extern u32 _digit_count;

static bool CmdAddNode(zdb::CNode node);
static bool CmdRemoveNode(zdb::CNode node);

enum class AnimType
{
	animTypeUndefined,
	animTypeRef,
	animTypeStand,
	animTypeSneak,
	animTypeSkitter,
	animTypeRun,
	animTypeSneakBack,
	animTypeSkitterBack,
	animTypeDie,
	animTypeJumpLaunch,
	animTypeJumpFall,
	animTypeJumpLand,
	animTypeJumpLandHard,
	animTypeStrafeRight,
	animTypeStrafeLeft,
	animTypeStrafeRightFast,
	animTypeStrafeLeftFast,
	animTypeStep,
	animTypeStand2Crouch,
	animTypeCrouch,
	animTypeCrouch2Prone,
	animTypeCrouchWalk,
	animTypeCrouchStep,
	animTypeCrouchStrafeR,
	animTypeCrouchStrafeRFast,
	animTypeCrouchWalkBack,
	animTypeStand2Prone,
	animTypeProne,
	animTypeProneCrawl,
	animTypeProneTurn,
	animTypePeekL,
	animTypePeekR,
	animTypeCrouchPeekL,
	animTypeCrouchPeekR,
	animTypeStand2PeekU,
	animtypeCrouch2PeekU,
	animTypeStand2Hang,
	animTypeHang,
	animTypeHang2Climb,
	animTypeHandJumpDown,
	animTypeLedge2HangBw,
	animTypeHopdownBw,
	animTypeFalling,
	animTypeLandSoft,
	animTypeLandHanging,
	animTypeClimbCrate,
	animTypeHopdownForward,
	animTypeStand2Ladder,
	animTypeClimbLadder,
	animTypeClimbOffLadder,
	animTypeFallForward01,
	animTypeGetupForward,
	animTypeThrowGrenade,
	animTypeUnderhandToss,
	animTypeFlinchRArm,
	animTypeFlinchRLeg,
	animTypeRLimp,
	animTypeRifle2Pistol,
	animTypeCrouchRifle2Pistol,
	animTypeProneRifle2Pistol,
	animTypeStand2Binocular,
	animTypeCrouch2Binocular,
	animTypeProne2Binocular,
	animTypeRestraintsApply,
	animTypeRestraintsReceive,
	animTypePutOnGround,
	animTypePutOnWall,
	animTypeOpenDoor,
	animTypeGetFromGround,
	animTypeGetFromSurface,
	animTypeRunR,
	animTypeRunL,
	animType_OnWall,
	animType_OnWallSmoke,
	animType_Stand2OnWall,
	animType_StandStretch,
	animType_Slouch,
	animType_SlouchSmoke,
	animTypeStab,
	animTypeStab_P,
	animTypeStabbed,
	animTypeCQB,
	animTypeCQB_P,
	animTypeCQBVictimFallBackwards,
	animTypeCQBVictimFallForwards,
	animTypeHeadLooks,
	animTypeHeadLooksProne,
	animTypeStand_P,
	animTypeCrouch_P,
	animTypeProne_P,
	animTypeHang_P,
	animTypeClimbLadder_P,
	animTypeSneak_P,
	animTypeSkitter_P,
	animTypeRun_P,
	animTypeSneakBack_P,
	animTypeSkitterBack_P,
	animTypeDie_P,
	animTypeStrafeRight_P,
	animTypeStrafeLeft_P,
	animTypeStrafeRightFast_P,
	animTypeStrafeLeftFast_P,
	animTypeStep_P,
	animTypeStand2Crouch_P,
	animTypeCrouch2Prone_P,
	animTypeCrouchWalk_P,
	animTypeCrouchStep_P,
	animTypeCrouchStrafeR_P,
	animTypeCrouchWalkBack_P,
	animTypeProneCrawl_P,
	animTypePeekL_P,
	animTypePeekR_P,
	animTypeCrouchPeekL_P,
	animTypeCrouchPeekR_P,
	animTypeStand2PeekU_P,
	animTypeCrouch2PeekU_P,
	animTypeStand2Hang_P,
	animTypeHang2Climb_P,
	animTypeHangJumpDown_P,
	animTypeLedge2HangBw_P,
	animTypeHopdownBw_P,
	animTypeFalling_P,
	animTypeLandSoft_P,
	animTypeLandHanging_P,
	animTypeClimbCrate_P,
	animTypeHopdownForward_P,
	animTypeStand2Ladder_P,
	animTypeClimbOffLadder_P,
	animTypeFallForward01_P,
	animTypeGetupForward01_P,
	animTypeRLimp_P,
	animTypeThrowGrenade_P,
	animTypeUnderhandToss_P,
	animTypeStand2Binocular_P,
	animTypeCrouch2Binocular_P,
	animTypeProne2Binocular_P,
	animTypeRestraintsApply_P,
	animTypePutOnGround_P,
	animTypePutOnWall_P,
	animTypeOpenDoor_P,
	animTypeGetFromGround_P,
	animTypeGetFromSurface_P,
	animTypeRunR_P,
	animTypeRunL_P
};

struct REFPT_INFO
{
	CPnt3D m_goal_pos;
	bool m_valid;
	CPnt3D m_goal_norm;
};

struct ANIM_CALLBACK
{
	f32 m_tfrac;
	void(*m_func)();
	void(*m_userdata)();
};

struct ANIMSET_PARAMS
{
	f32 m_stand_max_fwd;
	f32 m_stand_max_back;
	f32 m_stand_max_rt;
	f32 m_stand_max_left;

	f32 m_crouch_max_fwd;
	f32 m_crouch_max_back;
	f32 m_crouch_max_rt;
	f32 m_crouch_max_left;

	f32 m_prone_max_fwd;
	f32 m_prone_max_back;
	f32 m_prone_max_rt;
	f32 m_prone_max_left;

	f32 m_ladder_updn;
};

struct ANIMTYPE_GROUP
{
	AnimType m_rifle_type;
	AnimType m_pistol_type;
	AnimType m_rifle_fp_type;
	AnimType m_pistol_fp_type;
};

struct BLENDABLE_ANIM
{
	f32 m_trans0;
	f32 m_trans1;
	AnimType m_animtype;
};

struct zanim_cmd_index
{
	u8 cmd;
	u8 set;
};

struct zanim_cmd_val
{
	zanim_cmd_index index;
	u16 val;
};

struct zanim_cmd
{
	const char* name;
	zanim_cmd_val cmd_val;
	void*(*parser)();
	void*(*begin)();
	void*(*tick)();
	void*(*end)();
};

struct _zanim_cmd_hdr
{
	u32 data_type : 16;
	u32 quad_align : 1;
	u32 timeless : 1;
	u32 data_size : 14;
};

struct _zanim_cmd_set
{
	char* name;
	s32 count;
	zanim_cmd* cmd_list;
};

struct _zanim_main_params
{
	s32 m_version;
	f32 m_gravity;
	u32 m_search_external_nodes : 1;
	u32 m_unused : 31;
	s32 m_UserActionAnimIndex;
};

struct _zanim_anim_params
{
	u8 m_name_index;
	u8 m_root_node_index;

	bool m_paused;
	u8 m_state;
	
	u32 m_activation : 2;
	u32 m_network_anim : 1;
	u32 node_search_scope : 2;

	u32 m_auto_copy_anim : 1;
	u32 m_auto_add_to_world : 1;
	u32 m_create_instance : 1;
	u32 m_copy_instance : 1;
	u32 m_clone_anim : 1;
	u32 m_create_root : 1;
	s32 m_max_anim_copies : 6;
	s32 m_unused : 15;

	f32 m_timer;
	f32 m_priority;

	u32 m_damage_seq_offset : 16;
	u32 m_activation_seq_offset : 16;
	u32 m_execution_seq_offset : 16;
	u32 m_cleanup_seq_offset : 16;
};

struct _zanim_si_script
{
	u16 script_pathname_index;
	u16 script_object_name_index;
	bool spline_interp;
	s32 total_frame_count;
	s32 script_data_size;
	char* script_data;
};

struct _zanim_call_anim_ref
{
	u8 name_index;
	u8 local_name_index;

	bool stop_on_exit;

	CZAnim* base_anim;
	CZAnim* executing_anim;
};

struct _zanim_node_ref
{
	u32 parent_index : 8;
	u32 name_index : 11;
	u32 node_search : 3;

	bool is_light;
	bool created_here;
	bool stop_on_exit;
	bool on_world_lightlist;
	bool save_translation;
	bool save_rotation;
	bool save_active;
	bool save_parent;
	bool is_local_light;
	bool set_inactive;

	zdb::CNode* node_ptr;
};

struct _zanim_particle
{
	s32 name_index;
	Particle* particle;
};

struct _zanim_tex_ref
{
	s32 name_index;
	zdb::CTexHandle* texh;
};

struct _zanim_valve_ref
{
	s32 name_index;
	CValve* valve;
};

struct _zanim_sound
{
	s16 name_index;
	bool stop_on_exit;
	CSnd* sound_handle;
	CSndInstance* sound_instance;
};

struct _zsequence
{
	u32 name_index : 16;
	s32 loop_counter : 16;
	u32 paused : 1;
	
	u32 activation : 7;
	u32 state : 8;
	u32 cmd_state : 8;

	u32 m_IF_NestLevel : 8;

	s8* cmd_pc;
	s32 seq_data_size;

	f32 seq_timer;
	f32 cmd_timer;
	f32 loop_timer;
};

struct ZAnimNetworkPacket
{
	s32 packet_size;
	u16 anim_set_index;
	u16 anim_index;
};

struct AnimSet
{
	ANIMSET_PARAMS m_aparams;

	bool m_ready;
	std::string m_name;

	std::vector<u32> m_animIDs;
	std::vector<AnimType> m_basetypes;
	std::vector<CZAnim> m_included;

	CAnimsByFP* m_animgps;

	CBlendable* m_move_anims;
	CBlendable* m_moveback_anims;
	CBlendable* m_strafeL_anims;
	CBlendable* m_strafeR_anims;
	CBlendable* m_crouchmove_anims;
	CBlendable* m_crouchmoveback_anims;
	CBlendable* m_crouchstrafeL_anims;
	CBlendable* m_crouchstrafeR_anims;
};

struct SequenceData
{
	f32 m_fraction;
	CPnt4D m_data;
};

class Sequence : std::vector<SequenceData*> {};

class CAnimsByFP
{
private:
	ANIMTYPE_GROUP m_animgroups;
};

class CBlendable
{
	BLENDABLE_ANIM m_blendable[4];
};

class CActiveAnimList
{

};

class CZAnim : public _zanim_anim_params
{
public:
	void AddNewNodeRef();
	void OnWeaponHitInit();
	void GetNode();

	// CZAnimArgList* m_arg_list;
	// CZAnimNameIndexTable m_nameindex_table;

	CZAnimEx* m_animEx;
	CZAnimHealth* m_health;

	u32 m_invalid : 1;
	u32 m_added_to_world_internally : 1;
	u32 m_debug : 1;
	u32 m_animset_index : 8;
	u32 m_node_ref_count : 8;
	u32 m_tex_ref_count : 8;
	u32 m_particle_count : 8;
	u32 m_sound_ref_count : 8;
	u32 m_valve_ref_count : 8;
	u32 m_call_anim_ref_count : 8;

	zdb::CNode* m_instance_node;

	_zanim_node_ref* m_node_ref_list;
	_zanim_tex_ref* m_tex_ref_list;
	_zanim_valve_ref* m_valve_ref_list;
	_zanim_particle* m_particle_list;
	_zanim_sound* m_sound_ref_list;
	_zanim_call_anim_ref* m_call_anim_ref_list;
	_zanim_cmd_hdr** m_user_cmd;

	char* m_seq_data;
	s32 m_seq_data_size;

	CZAnim* m_sibling_anim;
};

class CZAnimEx : public zdb::CNodeEx
{
public:
	void AddAnimEx();
private:
	CZAnim* m_anim;

	zdb::CNode* m_nodeEx;
	zdb::CNode* m_root_node;

	f32 m_shield;
	f32 m_armor;
	f32 m_armor_max;
	f32 m_health;
	f32 m_health_max;
};

class CZAnimExVector : public std::vector<CZAnimEx*>
{
	
};

class CZAnimZAR : public zar::CZAR {};

class CZAnimNameTable
{
public:

};

class CZAnimSet
{
public:
	void GetName();

	bool m_WasStarted;
	bool m_IsStarted;
	bool m_IsAvailable;

	char* m_name;
	CZAnimNameTable m_name_table;

	s32 m_si_script_count;
	_zanim_si_script* m_si_script;

	CZAnimExVector* m_AnimExList;

	s32 m_anim_count;
	s32 m_anim_max;
	CZAnim* m_anim_list;
	// CZAnimSetSave* m_node_states;
	CSched_Manager* m_task_scheduler;
};

class CZAnimNameIndexTable
{
public:
	void GetName();
};

class CZAnimMain : public _zanim_main_params
{
public:
	char* SplitName(char* name, char** splitname);

	void Open();
	bool Close();
	bool Unload();
	bool InitCommands();

	void CmdNext();
	void CmdNext(s32 program_counter);
	void CmdSet(s32 program_counter);

	bool CmdValid() const;
	
	_zanim_cmd_hdr* AddCmd(const char* name, _zanim_cmd_hdr*(*parser)(_zrdr*), void(*begin)(_zanim_cmd_hdr*), bool(*tick)(_zanim_cmd_hdr*, f32*), void(*end)(_zanim_cmd_hdr*));
	_zanim_cmd_hdr* AnimParseExpression(_zanim_cmd_hdr* header, _zrdr* reader);

	_zanim_cmd_set* GetCmdEntry(char* name, bool param_2);
	_zanim_cmd_set* GetCmdSet(char* name, bool param_2);
	
	static bool m_LoadFromZAR;

	bool m_IsOpen;
	bool m_IsStarted;

	CZAnimNameTable m_name_table;
	CZAnimSet* m_animset_list;

	s32 m_animset_count;
	s32 m_cmdset_count;
	s32 m_cmdset_index;
	s32 m_cmdlist_index;

	_zanim_cmd_set* m_cmdset_list;

	CZAnimSet* m_CurAnimSet;
	CZAnim* m_CurAnim;
	_zsequence* m_CurSeq;
	bool m_anim_complete;

	bool m_debug;

	CZAnim* m_UserActionAnim;

	u8 m_Lod;
	void(*m_network_activation_callback)();
	bool m_block_activation_callback;

	_zanim_cmd_hdr m_cmd_if;
	_zanim_cmd_hdr m_cmd_elseif;
	_zanim_cmd_hdr m_cmd_else;
	_zanim_cmd_hdr m_cmd_endif;
	_zanim_cmd_hdr m_cmd_endwhile;
	_zanim_cmd_hdr m_cmd_ExprOperator;

	f32 m_exp[41];
	f32 m_inv_exp[41];

	ZAnimNetworkPacket* m_NetworkPacket;
	s32 m_NetworkPacketMaxSize;

	s32 m_RunningAnimCount;
	s32 m_CurrentFrame;
};

class CZAnimExpression
{
public:
	static bool IsOperator(const char* operation);
	static _zanim_cmd_hdr* NewCmd(const char* operation);
	static bool ProcExpr(f32* dT);
};

class CActiveAnimPool
{

};

class CZBodyAnimBlend
{
public:
	void SetFractionalTime(f32 time);
private:
	CPnt3D m_dpos;
	float m_quitT;
	CPnt3D m_drot;
	float m_tfrac;

	REFPT_INFO* m_refpt_info;

	// CZBodyPart* m_action_node;
	// CZBodyPart* m_result_parent;

	char m_flags;
	char m_ctrl;
	char m_state;

	AnimType m_animType;

	CActiveAnimList* m_anims;
	ANIM_CALLBACK m_callback;
	// CHoming m_homing;
};

class CZSIObject
{
private:
	f32* m_positionData;
	CQuat* m_rotations;
	u32 m_inUse : 1;
	u32 m_constPos : 1;
	u32 m_constRot : 1;
	u32 m_padding : 21;
	u32 m_body_part_id : 8;
	char* m_pName;
};

class CZSIScript
{
	friend class CZSIScriptCommon;
protected:
	s32 m_isLoaded : 1;
	s32 m_looped : 1;
	f32 m_tf;
	f32 m_tScale;
	f32 m_multiplier;
	CZSIScriptCommon* m_pCommon;
};

class CZSIScriptCommon
{
	friend class CZSIObject;
private:
	char* m_name;
	s16 m_nFrames;
	s16 m_nObjects;
	CZSIObject* m_pObjects;
	s16 m_refCount;
	s16 m_objects_ready;
	f32 m_distance;
	f32 m_thr_scale;
	f32 m_blendtime;
	f32 m_terrnorm0;
	f32 m_terrnorm1;
	f32 m_fire_val;
	f32 m_interrupt_val;
	std::vector<CZSIObject*> m_objects_for_body;
};

class CSealAnim
{
public:
	CSealAnim();
	~CSealAnim();
};

class CSequence
{
private:
	f32 m_upT;
	f32 m_holdT;
	f32 m_downT;
	f32 m_currT;
};

_zanim_cmd_hdr* zAnimLoad_If(_zrdr*);
_zanim_cmd_hdr* zAnimLoad_Else(_zrdr*);
_zanim_cmd_hdr* zAnimLoad_ElseIf(_zrdr*);
_zanim_cmd_hdr* zAnimLoad_EndIf(_zrdr*);
_zanim_cmd_hdr* zAnimLoadWhile(_zrdr*);
_zanim_cmd_hdr* zAnimLoadEndWhile(_zrdr*);
_zanim_cmd_hdr* zAnimLoadBreak(_zrdr*);
_zanim_cmd_hdr* zAnimLoadWait(_zrdr*);
_zanim_cmd_hdr* zAnimLoadLoop(_zrdr*);

_zanim_cmd_hdr* zAnimLoadObjectAddChild(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectDeleteChild(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectMotion(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectMotionFromTo(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectMotionSIScript(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectMotionSIScriptForObject(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectTranslateState(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectRotateState(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectActiveState(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectOpacityFromTo(_zrdr*);
_zanim_cmd_hdr* zAnimLoadObjectBlendMode(_zrdr*);

_zanim_cmd_hdr* zAnimLoadNodeActive(_zrdr*);
_zanim_cmd_hdr* zAnimLoadNodeRendered(_zrdr*);
_zanim_cmd_hdr* zAnimLoadLight(_zrdr*);

_zanim_cmd_hdr* zAnimLoadAnimHealthTest(_zrdr*);
_zanim_cmd_hdr* zAnimLoadAnimLODTest(_zrdr*);
_zanim_cmd_hdr* zAnimLoadInvalidateAnim(_zrdr*);
_zanim_cmd_hdr* zAnimLoadCallAnim(_zrdr*);
_zanim_cmd_hdr* zAnimLoadCallSequence(_zrdr*);
_zanim_cmd_hdr* zAnimLoadCamera(_zrdr*);
_zanim_cmd_hdr* zAnimLoadDebug(_zrdr*);
_zanim_cmd_hdr* zAnimLoadDestructor(_zrdr*);
_zanim_cmd_hdr* zAnimLoadFail(_zrdr*);
_zanim_cmd_hdr* zAnimLoadFireWeapon(_zrdr*);
_zanim_cmd_hdr* zAnimLoadMessage(_zrdr*);
_zanim_cmd_hdr* zAnimLoadMotionSIScript(_zrdr*);
_zanim_cmd_hdr* zAnimLoadParticleSource(_zrdr*);
_zanim_cmd_hdr* zAnimLoadPauseAnim(_zrdr*);
_zanim_cmd_hdr* zAnimLoadRandomWeight(_zrdr*);
_zanim_cmd_hdr* zAnimLoadRangeTest(_zrdr*);
_zanim_cmd_hdr* zAnimLoadResumeAnim(_zrdr*);
_zanim_cmd_hdr* zAnimLoadSound(_zrdr*);
_zanim_cmd_hdr* zAnimLoadStopAnim(_zrdr*);
_zanim_cmd_hdr* zAnimLoadStopSequence(_zrdr*);
_zanim_cmd_hdr* zAnimLoadTimer(_zrdr*);

_zanim_cmd_hdr* zAnimLoadUIMenu(_zrdr*);
_zanim_cmd_hdr* zAnimUIAppMenu(_zrdr*);
_zanim_cmd_hdr* zAnimLoadUIAppMenu(_zrdr*);

void zAnimCallAnimBegin(_zanim_cmd_hdr*);
void zAnimCameraBegin(_zanim_cmd_hdr*);
void zAnimMultiObjectMotionScriptBegin(_zanim_cmd_hdr*);
void zAnimWaitBegin(_zanim_cmd_hdr*);
void zAnimObjectOpacityFromToBegin(_zanim_cmd_hdr*);
void zAnimObjectMotionBegin(_zanim_cmd_hdr*);
void zAnimObjectMotionFromToBegin(_zanim_cmd_hdr*);

bool zAnimCallAnimTick(_zanim_cmd_hdr*, f32*);
bool zAnimCallSequenceTick(_zanim_cmd_hdr*, f32*);
bool zAnimCameraTick(_zanim_cmd_hdr*, f32*);
bool zAnimBreakTick(_zanim_cmd_hdr*, f32*);
bool zAnimIfTick(_zanim_cmd_hdr*, f32*);
bool zAnimEndIfTick(_zanim_cmd_hdr*, f32*);
bool zAnimGoToEndIf(_zanim_cmd_hdr*, f32*);
bool zAnimFireWeaponTick(_zanim_cmd_hdr*, f32*);
bool zAnimDebugTick(_zanim_cmd_hdr*, f32*);
bool zAnimDestructorTick(_zanim_cmd_hdr*, f32*);
bool zAnimEndWhileTick(_zanim_cmd_hdr*, f32*);
bool zAnimFailTick(_zanim_cmd_hdr*, f32*);
bool zAnimHealthTestTick(_zanim_cmd_hdr*, f32*);
bool zAnimInvalidateAnimTick(_zanim_cmd_hdr*, f32*);
bool zAnimLightTick(_zanim_cmd_hdr*, f32*);
bool zAnimLODTestTick(_zanim_cmd_hdr*, f32*);
bool zAnimLoopTick(_zanim_cmd_hdr*, f32*);
bool zAnimMessageTick(_zanim_cmd_hdr*, f32*);
bool zAnimMultiObjectMotionScriptTick(_zanim_cmd_hdr*, f32*);
bool zAnimNodeActiveTick(_zanim_cmd_hdr*, f32*);
bool zAnimNodeRenderedTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectActiveStateTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectAddChildTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectBlendModeTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectDeleteChildTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectMotionFromToTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectMotionScriptTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectMotionTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectOpacityFromToTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectRotateStateTick(_zanim_cmd_hdr*, f32*);
bool zAnimObjectTranslateStateTick(_zanim_cmd_hdr*, f32*);
bool zAnimParticleSourceTick(_zanim_cmd_hdr*, f32*);
bool zAnimPauseAnimTick(_zanim_cmd_hdr*, f32*);
bool zAnimQuadAlignTick(_zanim_cmd_hdr*, f32*);
bool zAnimRandomWeightTick(_zanim_cmd_hdr*, f32*);
bool zAnimRangeTestTick(_zanim_cmd_hdr*, f32*);
bool zAnimResumeAnimTick(_zanim_cmd_hdr*, f32*);
bool zAnimSoundTick(_zanim_cmd_hdr*, f32*);
bool zAnimStopAnimTick(_zanim_cmd_hdr*, f32*);
bool zAnimStopSequenceTick(_zanim_cmd_hdr*, f32*);
bool zAnimTimerTick(_zanim_cmd_hdr*, f32*);
bool zAnimUIAppMenuTick(_zanim_cmd_hdr*, f32*);
bool zAnimUIMenuTick(_zanim_cmd_hdr*, f32*);
bool zAnimWaitTick(_zanim_cmd_hdr*, f32*);
bool zAnimWhileTick(_zanim_cmd_hdr*, f32*);