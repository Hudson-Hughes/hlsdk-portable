#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "decals.h"
#include "bullet_projectile.h"
#include "game.h"

LINK_ENTITY_TO_CLASS( bullet_projectile, CBulletProjectile )

CBulletProjectile *CBulletProjectile::BulletCreate( Vector vecOrigin, Vector vecDir, float flDamage, int iBulletType, edict_t *pentOwner )
{
	CBulletProjectile *pBullet = GetClassPtr( (CBulletProjectile *)NULL );
	pBullet->pev->classname = MAKE_STRING( "bullet_projectile" );
	pBullet->Spawn();

	UTIL_SetOrigin( pBullet->pev, vecOrigin );
	pBullet->pev->velocity = vecDir.Normalize() * flex_bullet_speed.value;
	pBullet->pev->angles = UTIL_VecToAngles( pBullet->pev->velocity );
	pBullet->pev->owner = pentOwner;
	pBullet->pev->dmg = flDamage;
	pBullet->m_iBulletType = iBulletType;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( pBullet->entindex() );
		WRITE_SHORT( pBullet->m_iTrail );
		WRITE_BYTE( 3 );  // life in 0.1s
		WRITE_BYTE( 1 );  // width
		WRITE_BYTE( 255 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 200 );
		WRITE_BYTE( 200 ); // brightness
	MESSAGE_END();

	return pBullet;
}

void CBulletProjectile::Spawn( void )
{
	Precache();

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL( ENT( pev ), "models/shell.mdl" );
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 0;

	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	SetTouch( &CBulletProjectile::BulletTouch );
	SetThink( &CBulletProjectile::BulletThink );
	pev->nextthink = gpGlobals->time + BULLET_LIFETIME;
}

void CBulletProjectile::Precache( void )
{
	PRECACHE_MODEL( "models/shell.mdl" );
	m_iTrail = PRECACHE_MODEL( "sprites/smoke.spr" );
}

void CBulletProjectile::BulletTouch( CBaseEntity *pOther )
{
	SetTouch( NULL );
	SetThink( NULL );

	if( pOther->edict() == pev->owner )
		return;

	if( pOther->pev->takedamage )
	{
		TraceResult tr = UTIL_GetGlobalTrace();
		entvars_t *pevOwner = pev->owner ? VARS( pev->owner ) : pev;

		ClearMultiDamage();
		pOther->TraceAttack( pevOwner, pev->dmg, pev->velocity.Normalize(), &tr,
			DMG_BULLET | ( ( pev->dmg > 16 ) ? DMG_ALWAYSGIB : DMG_NEVERGIB ) );
		ApplyMultiDamage( pev, pevOwner );
	}
	else
	{
		TraceResult tr = UTIL_GetGlobalTrace();

		Vector vecSrc = pev->origin;
		Vector vecEnd = vecSrc + pev->velocity.Normalize() * 32.0f;
		TEXTURETYPE_PlaySound( &tr, vecSrc, vecEnd, m_iBulletType );
		DecalGunshot( &tr, m_iBulletType );
	}

	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time;
}

void CBulletProjectile::BulletThink( void )
{
	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time;
}
