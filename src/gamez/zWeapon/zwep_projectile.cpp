#include "zweapon.h"
#include "zwep_projectilelist.h"
#include "gamez/zNetwork/znet.h"

void CZProjectile::Reset()
{
	if (m_pickup != NULL)
	{
		// m_pickup->Destroy();
		m_pickup = NULL;
	}

	m_heading = 0.0f;
	m_pitch = 0.0f;
	m_roll = 0.0f;

	m_done = false;
	m_isfirsthit = true;
	m_isfirsthit = true;
	m_useflyout = true;
	m_lastHitWasPerson = true;
	m_lastHitWasVolumetric = true;
	m_echo = true;

	m_intersecthandle = -1;
	m_animinstance = NULL;
	m_shottype = PROJECTILE_TYPE::NORMAL;
	m_state = PROJECTILE_STATE::PROJECTILE_EXPIRED;

	m_currange = 0.0f;
	m_shooterID = 0;

	m_time = 9999999.0f;
	m_removaltime = 9999999.0f;

	m_framestartpos = CPnt3D::zero;
	m_frameendpos = CPnt3D::zero;
	m_startpos = CPnt3D::zero;

	m_instance->SetModel(NULL);

	if (m_weapon != NULL)
	{
		// m_currange = m_weapon->GetMaxRange();

		if (m_currange == 0.0f)
		{
			m_shottype = m_shottype;
		}
		else
		{
			m_shottype = PROJECTILE_TYPE::ONE_FRAME;
		}
	}

	if (m_ammo != NULL)
	{
		m_ammo = m_ammo;
	}
}

zdb::CNode* CZProjectile::GetInstance() const
{
	return m_instance;
}

f32 CZProjectile::ResolveDamage(f32* health, f32* armor, f32 damage, f32 multiplier)
{
	f32 armorpct = damage - *armor * (multiplier / 10.0f);
	f32 healthpct = damage - armorpct;

	if (armorpct <= 0.0f)
	{
		armorpct = 0.0f;
		healthpct = damage - 0.0f;
	}

	healthpct = *armor - healthpct / 4.0f;
	*armor = healthpct;

	if (healthpct <= 0.0f)
		*armor = 0.0f;

	healthpct = *health;
	*health = healthpct - armorpct;

	if (healthpct - armorpct <= 0.0f)
		*health = 0.0f;

	return *health;
}

bool CZProjectile::FireValidityCheck()
{
	// Created projectiles are validated by the server only.
	if (theNetwork.m_bNetwork)
		return false;
	
	if (!m_weapon)
		return false;

	if (!m_weapon->IsDirectFire())
		return false;

	return true;
}

void CZProjectile::PreFireProjectile()
{
	if (m_shottype == PROJECTILE_TYPE::ONE_FRAME)
	{
		// CZWeapon::m_pProjectileList.GetNextDirectFireDI(&m_di, &m_di_handle);
		m_removeASAP = false;
	}
	else
	{
		// CZWeapon::m_pProjectileList.GetNextIndirectFireDI(&m_di, &m_di_handle);
		m_removeASAP = false;
	}
}

bool CZProjectile::SetProjectile(zdb::CNode* owner, CZWeapon* weapon, CZAmmo* ammo,
	const CPnt3D& startpos, CPnt3D& velscale, CPnt3D& vel,
	s32 id, zdb::CModel* model, f32 time, f32 removal_time)
{
	m_weapon = weapon;

	if (m_weapon)
	{
		m_currange = m_weapon->GetMaxRange();
	}

	if (m_weapon->m_ID < EQUIP_ITEM::EQUIP_SMG)
	{
		m_useflyout = false;
	}
	else
	{
		m_useflyout = true;
	}

	m_ammo = ammo;

	m_framestartpos = startpos;
	m_frameendpos = startpos;

	m_intersecthandle = -1;

	velscale.Scale(m_weapon->GetMuzzleVelocity(), &m_vel);
	m_vel.Add(&vel, &m_vel);

	if (model)
	{
		m_instance->SetModel(model);
	}

	m_owner = owner;
	m_time = time;
	m_removaltime = removal_time;
	m_pickup = NULL;

	m_done = false;

	return true;
}

bool CZProjectileList::GetNextDirectFireDI(zdb::DiIntersect** di, s32* free_handle)
{
	if (m_directFireDI[0])
	{
		if (m_direct_firstfree < 60)
		{
			*di = m_directFireDI[m_direct_firstfree];
			*free_handle = m_direct_firstfree;

			m_directFireDIUsed[m_direct_firstfree] = true;

			while (m_direct_firstfree < 60 && m_directFireDIUsed[m_direct_firstfree])
				m_direct_firstfree++;
		}

		return true;
	}

	return false;
}

bool CZProjectileList::GetNextIndirectFireDI(zdb::DiIntersect** di, s32* free_handle)
{
	if (m_indirectFireDI[0])
	{
		if (m_indirect_firstfree < 60)
		{
			*di = m_indirectFireDI[m_indirect_firstfree];
			*free_handle = m_indirect_firstfree;

			m_indirectFireDIUsed[m_indirect_firstfree] = true;

			while (m_indirect_firstfree < 60 && m_indirectFireDIUsed[m_indirect_firstfree])
				m_indirect_firstfree++;
		}

		return true;
	}

	return false;
}