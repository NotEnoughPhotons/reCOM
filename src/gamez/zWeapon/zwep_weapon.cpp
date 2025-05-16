#include "zweapon.h"

CZWeapon::CZWeapon()
{
	// AI parameter setup
	m_ai_params.m_min_range = 50.0f;
	m_ai_params.m_max_range = 1000.0f;
	m_ai_params.m_version = AI_PARAMS::version;
	m_ai_params.m_custom = false;
	m_ai_params.m_hard_min = false;

	m_name = 0;
	m_description = 0;

	m_maxfiremode = FIREMODE::SAFETY;

	m_ammocap = 0;

	m_soundradius = 0.0f;
	m_soundradiusSq = 0.0f;

	m_encumb = ENCUMBRANCE::LIGHTLY_ENCUMBERED;

	m_effectiverange = 0.0f;
	m_maxrange = 0.0f;
	
	m_texname = 0;
	m_icontexname = 0;
	m_associatedgearname = 0;
	m_modelname = 0;
	
	m_icontex = NULL;

	m_ID = EQUIP_ITEM::EQUIP_NONE;

	m_nummags = 0;

	m_specialmaterialanimname = 0;
	m_defaultspecialanimname = 0;
	m_fireanimname = 0;
	m_hitanimname = 0;

	m_hitanim = NULL;
	m_specialmaterialanim = NULL;
	m_defaultspecialanim = NULL;
	m_zoomedfireanim = NULL;
	m_fireanim = NULL;

	m_texture = NULL;

	m_firewait = 0.1f;

	m_decalsetname = 0;
	// TODO: Implement CZDecalSet
	// m_decalset = NULL;

	m_muzzlevel = 0.0f;
	m_impactradius = 0.0f;

	m_reloadsoundname = 0;
	m_reloadsound = NULL;
	m_reloadtime = 0.0f;

	m_reloadAfterShot = false;

	m_hasFiremodes[0] = true;
	m_hasFiremodes[1] = false;
	m_hasFiremodes[2] = false;
	m_hasFiremodes[3] = false;
	m_hasFiremodes[4] = false;
}

void CZWeapon::AddLegalAmmo(CZAmmo* ammo)
{

}

// 	RVA: 0x339A60 -> SCUS_971.34
bool CZWeapon::HasFireMode(s32 id) const
{
	return id >= 5 || m_hasFiremodes[id];
}

void CZWeapon::ClearAnims()
{
	if (m_hitanim != NULL)
	{
		zfree(m_hitanim);
		m_hitanim = NULL;
	}

	m_fireanim = NULL;
	m_zoomedfireanim = NULL;
	m_defaultspecialanim = NULL;
	
	if (m_specialmaterialanim != NULL)
	{
		zfree(m_specialmaterialanim);
		m_specialmaterialanim = NULL;
	}
}

void CZWeapon::ClearTimer()
{
	m_timer_pCurval = 0;
	m_timer_bTimerRegistered = false;
	m_timer_bTimerCleared = true;
	m_timer_maxval = -1.0f;
	m_timer_bIncrTimer = 1;
}

void CZWeapon::Close()
{
	// m_flyout_machine->ReleaseAllActive()
}

void CZWeapon::Fire(CZProjectile& projectile)
{
	zdb::CNode* instance = projectile.GetInstance();

	if (instance && instance->m_parent != zdb::CWorld::m_world)
		zdb::CWorld::m_world->AddChild(instance);

	projectile.PreFireProjectile();

	if (!projectile.FireValidityCheck())
	{
		// m_pProjectileList.RemoveProjectile(projectile);
	}
	else
	{
		
	}
}

//	RVA: 0x339A30 -> SCUS_971.34
//  RVA: 0x31EEB0 -> SCUS_972.05
void CZWeapon::SetHasFireMode(FIREMODE newMode)
{
	m_hasFiremodes[static_cast<s32>(newMode)] = true;
	
	if (m_maxfiremode < newMode)
		m_maxfiremode = newMode;
}

// TODO: Reimplement the zoom function properly
f32 CZFTSWeapon::GetZoomRange(u32 range_level) const
{
	f32 range = 0.0f;
	
	if (m_firstZoomLevel == 0 || range <= m_thirdZoomLevel)
	{
		range = 500.0f;
	}
	else
	{

	}

	return range;
}