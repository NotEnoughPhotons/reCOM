#pragma once
#include <list>

#include "gamez/zReader/zrdr.h"
#include "gamez/zUtil/zutil.h"

class CValve;
class CValvePool;

enum OP_TYPE
{
	OP_NONE,
	OP_CMP_EQUAL,
	OP_CMP_NOTEQUAL,
	OP_CMP_GTHAN,
	OP_CMP_LTHAN,
	OP_CMP_GTHANEQ,
	OP_CMP_LTHANEQ,
	OP_EQUAL,
	OP_INCREMENT,
	OP_DECREMENT
};

/// <summary>
/// Determines the pooling type for the valve.
/// <para> ValveType::PERM - Lasts the entire lifetime of the level. Will pool if value reaches threshold. </para>
/// <para> ValveType::TEMP - Loads once then unloads when done. </para>
/// <para> ValveType::PERSIST - Lasts the entire lifetime of the game, and will pool if the value reaches threshold.</para>
/// </summary>
enum class VALVE_TYPE
{
	VTYPE_NONE,
	VTYPE_PERM,
	VTYPE_TEMP,
	VTYPE_PERSIST
};

enum class VALVE_STATE
{
	VSTATE_DORMANT,
	VSTATE_TEMPORARY,
	VSTATE_PERMANENT,
	VSTATE_PERSISTENT
};

class CValvePool;

extern CValvePool valvePool;

class CValveCBList
{
public:
	std::list<void(*)(CValve*, void*)> m_list;
};

/// <summary>
/// A class that acts as a "signal" for events to occur in the game.
/// An example is the valve "mission_failure", that when fired, will run callbacks to handle mission failure.
/// </summary>
class CValve
{
public:
	CValve(const char* name, u32 value, VALVE_TYPE type);
	
	static void Init() { }

	static bool Open(const char* name, VALVE_TYPE type);
	static void Close();
	static void Reset();

	static bool Parse(_zrdr* file, VALVE_TYPE type);
	// static _zanim_cmd_hdr* CmdParseCmp(_zrdr* reader);
	// static bool CmdTickCmp(_zanim_cmd_hdr* header, float* delta);

	static CValve* Create(const char* name, VALVE_TYPE type);
	static CValve* Create(const char* name, s32 count, VALVE_TYPE type);
	static void Destroy(CValve* valve);

	static CValve* GetByName(const char* name);

	static void RegisterCommands();
	
	static std::list<CValve*> m_list;

	void AssignName(const char* name);
	void FreeName();

	void* AddCallback(size_t size, void(*callback)(CValve*, void*), void* buffer);
	void MakeCallbacks(VALVE_STATE state) {}
	void DeleteCallbacks();

	bool Set(s32 value);

	OP_TYPE ParseOperator(const char* op);

	char* m_name;
	u32 m_value : 16;
	u32 m_type : 8;
	u32 m_name_pooled : 1;
	u32 m_pooled : 1;
	u32 m_unused : 6;
	CValveCBList m_callbacks;
};

class CValvePool : public std::vector<CValve*>
{
public:
	CValve* Acquire(const char* name, VALVE_TYPE type);

	bool m_has_name;
	char* m_pool_name;
	u32 m_pool_length;
	u32 m_max_char_len;
};

