#include "node_action.h"

const char* CNodeAction::GetActionType(ACTION_TYPE type) const
{
	switch (type)
	{
	case ACTION_TYPE::ACT_DOOR:
		return "DOOR";
	case ACTION_TYPE::ACT_WINDOW:
		return "WINDOW";
	case ACTION_TYPE::ACT_SWITCH:
		return "SWITCH";
	case ACTION_TYPE::ACT_DEFUSE:
		return "DEFUSE";
	case ACTION_TYPE::ACT_MPBOMB:
		return "MPBOMB";
	case ACTION_TYPE::ACT_BREACH:
		return "BREACH";
	case ACTION_TYPE::ACT_SATCHEL_CHARGE:
		return "SATCHEL";
	default:
		return "UNKNOWN";
	}
}

const char* CNodeAction::GetUIString()
{
	switch (m_actionType)
	{
	case ACTION_TYPE::ACT_DOOR:
		if (m_uiVars.size() != 0)
			return "UIVAR_ACT_DOOR";
		else
			return "OPEN DOOR";
	case ACTION_TYPE::ACT_WINDOW:
		if (m_uiVars.size() != 0)
			return "UIVAR_ACT_WINDOW";
		else
			return "OPEN WINDOW";
	case ACTION_TYPE::ACT_SWITCH:
		return "UIVAR_ACT_SWITCH";
	case ACTION_TYPE::ACT_DEFUSE:
		return "UIVAR_ACT_DEFUSE";
	case ACTION_TYPE::ACT_MPBOMB:
		return "UIVAR_ACT_MPBOMB";
	case ACTION_TYPE::ACT_BREACH:
		return "UIVAR_ACT_BREACH";
	case ACTION_TYPE::ACT_SATCHEL_CHARGE:
		return "UIVAR_ACT_SATCHEL_CHARGE";
	}
}