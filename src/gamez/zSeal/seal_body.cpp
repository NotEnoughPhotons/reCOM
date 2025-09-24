#include "zseal.h"

bool CZSealBody::DamageBody(f32 damage, f32 multiplier)
{
	s8 retidx = 0;

	if (m_body_health <= 0.0f || damage == 0.0f)
		return false;

	CZProjectile::ResolveDamage(&m_body_health, &m_armor[3], damage, multiplier);

	if (m_body_health <= m_body_healthMax * 0.1)
	{
		retidx = 4;
	}
	else
	{
		retidx = 3;

		if (m_body_healthMax * 0.3f < m_body_health)
		{
			if (m_body_health <= m_body_healthMax * 0.7f)
			{
				retidx = 2;
			}
			else
			{
				retidx = 0;

				if (m_body_health <= m_body_healthMax * 0.9f)
				{
					retidx = 1;
				}
			}
		}
			
		if (retidx != 4 && retidx == 3 || retidx == 2)
			m_retmods += 3.0f;

		// AnimType damageAnim = FindRandomDamageAnim(DAMAGE_LOCATION::BODY);
		SEAL_STANCE stance = GetStance(false);
		// WeaponHitAnim(damageAnim, stance, false);
		return true;
	}

	return false;
}