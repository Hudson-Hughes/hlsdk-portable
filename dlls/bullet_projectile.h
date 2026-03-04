#ifndef BULLET_PROJECTILE_H
#define BULLET_PROJECTILE_H

#define BULLET_LIFETIME 3.0f

class CBulletProjectile : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT BulletTouch( CBaseEntity *pOther );
	void EXPORT BulletThink( void );

	int m_iBulletType;
	int m_iTrail;

	static CBulletProjectile *BulletCreate( Vector vecOrigin, Vector vecDir, float flDamage, int iBulletType, edict_t *pentOwner );
};

#endif
