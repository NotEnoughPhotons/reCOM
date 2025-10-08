#include <cstdint>
#include <iostream>

#include "zai.h"

void AI_PARAMS::Parse(_zrdr* reader)
{

}

CAiState* CAiState::Create(AI_STATE state)
{
	CAiState* aiState;

	switch (state)
	{
	case STATE_UNKNOWN:
		break;
	case STATE_SCRIPT:
		aiState = new CAiSScript();
		aiState->Reset(STATE_SCRIPT);
		break;
	case STATE_HOLD:
		aiState = new CAiSHold();
		aiState->Reset(STATE_HOLD);
		break;
	case STATE_AVOID:
		aiState = new CAiSAvoid();
		aiState->Reset(STATE_AVOID);
		break;
	case STATE_DEPLOY:
		aiState = new CAiSDeploy();
		aiState->Reset(STATE_DEPLOY);
		break;
	case STATE_LASING:
		break;
	case STATE_SURRENDER:
		aiState = new CAiSSurrender();
		aiState->Reset(STATE_SURRENDER);
		break;
	case STATE_RESTRAINED:
		aiState = new CAiSRestrained();
		aiState->Reset(STATE_RESTRAINED);
		break;
	case STATE_GOTO:
		aiState = new CAiSGoto();
		aiState->Reset(STATE_GOTO);
		break;
	case STATE_FOLLOW:
		aiState = new CAiSFollow();
		aiState->Reset(STATE_FOLLOW);
		break;
	case STATE_BREACH:
		aiState = new CAiSBreach();
		aiState->Reset(STATE_BREACH);
		break;
	case STATE_ACTION:
		aiState = new CAiSAction();
		aiState->Reset(STATE_ACTION);
		break;
	case STATE_PURSUE:
		aiState = new CAiSPursue();
		aiState->Reset(STATE_PURSUE);
		break;
	case STATE_ANIMATE:
		aiState = new CAiSAnimate();
		aiState->Reset(STATE_ANIMATE);
		break;
	case STATE_CQB:
		aiState = new CAiSCQBAttack();
		aiState->Reset(STATE_CQB);
		break;
	case STATE_FLEE:
		aiState = new CAiSFlee();
		aiState->Reset(STATE_FLEE);
		break;
	case STATE_PATHRECO:
		aiState = new CAiSPathRecover();
		aiState->Reset(STATE_PATHRECO);
		break;
	case STATE_RESCUED:
		aiState = new CAiSRescued();
		break;
	case STATE_SUSPENDED:
		aiState = new CAiSSuspended();
		break;
	case STATE_SHOTAT:
		aiState = new CAiSShotAt();
		break;
	case STATE_RELOAD:
		// aiState = new CAiSReload();
		break;
	case STATE_NOAMMO:
		aiState = new CAiSNoAmmo();
		break;
	case STATE_PICKUP:
		// aiState = new CAiSPickup();

		break;
	case STATE_HOSTAGE:
		aiState = new CAiSHostage();
		break;
	case STATE_CORPSE:
		// aiState = new CAiSCorpse();

		break;
	case STATE_EVENT:
		aiState = new CAiSEvent();
		break;
	case STATE_TARGETING:
		aiState = new CAiSTarget();
		break;
	case STATE_GRENADE:
		aiState = new CAiSGrenade();
		break;
	case STATE_STUNNED:
		// aiState = new CAiSStunned();
		break;
	case STATE_RUSH:
		aiState = new CAiSRush();
		break;
	default:
		break;
	}

	return NULL;
}

void CAiState::Reset(AI_STATE state)
{
	m_bitfield = m_bitfield & 0b11111101;
	m_state = state;
	m_field8_0x14 = 0;
	m_bitfield = m_bitfield & 0b11111110;
	m_field2_0x8 = 0;
	m_field1_0x4 = 0;
	m_bitfield = m_bitfield & 0b11111011;
	m_bitfield = m_bitfield & 0b00000111;
}