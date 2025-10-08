#ifndef NODE_ACTION_H
#define NODE_ACTION_H
#include "znode.h"

class CNodeAction : public zdb::CNodeEx
{
public:
	enum ACTION_TYPE
	{
		ACT_DOOR,
		ACT_WINDOW,
		ACT_SWITCH,
		ACT_DEFUSE,
		ACT_MPBOMB,
		ACT_BREACH,
		ACT_SATCHEL_CHARGE,
		ACT_UNKNOWN
	};

	// CNodeAction(zdb::CNode* node, CZAnim* animToPlay, CValve* actionValve, zdb::CTexHandle* handle);
	// ~CNodeAction();

	static void Open(zdb::CWorld* world, const char* readerName);
	static void Close();

	// CNodeAction* FindActionByValve(const CValve* valve) const;
	CNodeAction* GetAction(bool isBase) const;
	CNodeAction* GetActionById(s32 id) const;
	const char* GetActionType(ACTION_TYPE type) const;
	CNodeAction* GetClosestAction(const CPnt3D* position, f32 radius) const;

	// bool ExecuteAction(CZSealBody* seal, unsigned int actionFlags) const;

	template<class T>
	bool OnAction(zdb::CNode* node, T* type) const;
private:
	static std::vector<CNodeAction> m_actions;

	CNodeAction** actionList;

	zdb::CNode* node;
	// CZAnim** animations;
	// std::vector<CZAnim*> animVector;
	// CZAnim* actionAnimation;
	CValve* m_valve;
	zdb::CTexHandle* m_actionBitmap;

	s32 m_type;
	u32 m_flags;
};
#endif