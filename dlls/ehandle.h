/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

// Safe way to point to CBaseEntities who may die between frames.
template <typename T = CBaseEntity>
class EntityHandle
{
public:
	EntityHandle() : m_edict(nullptr), m_serialnumber(0) {}
	EntityHandle(const EntityHandle<T> &other);

	// Cast to base class
	// NOTE: this is a unsafe method
	template <typename I>
	I *Get() const
	{
		return (I *)((T *)(this));
	}

	edict_t *Get() const;
	edict_t *Set(edict_t *pEdict);

	bool IsValid() const;
	int GetSerialNumber() const;

	bool operator==(T *pEntity) const;
	operator int() const;
	operator T *() const;

	T *operator=(T *pEntity);
	T *operator->();

	// Copy the ehandle.
	EntityHandle<T>& operator=(const EntityHandle<T> &other);

private:
	edict_t *m_edict;
	int m_serialnumber;
};

// Short alias
using EHandle = EntityHandle<>;
using EHANDLE = EHandle;

// Inlines
template <typename T>
inline bool FNullEnt(EntityHandle<T> &hent)
{
	return (!hent || FNullEnt(OFFSET(hent.Get())));
}

// Copy constructor
template <typename T>
EntityHandle<T>::EntityHandle(const EntityHandle<T> &other)
{
	m_edict = other.m_edict;
	m_serialnumber = other.m_serialnumber;
}

template <typename T>
inline edict_t *EntityHandle<T>::Get() const
{
	if (!m_edict || m_edict->serialnumber != m_serialnumber || m_edict->free)
	{
		return nullptr;
	}

	return m_edict;
}

template <typename T>
inline edict_t *EntityHandle<T>::Set(edict_t *pEdict)
{
	m_edict = pEdict;
	if (pEdict)
	{
		m_serialnumber = pEdict->serialnumber;
	}

	return pEdict;
}

// Returns whether this handle is valid.
template <typename T>
inline bool EntityHandle<T>::IsValid() const
{
	edict_t *pEdict = Get();
	if (!pEdict)
	{
		return false;
	}

	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pEdict);
	if (!pEntity)
	{
		return false;
	}

	return true;
}

// CBaseEntity serial number.
// Used to determine if the entity is still valid.
template <typename T>
inline int EntityHandle<T>::GetSerialNumber() const
{
	return m_serialnumber;
}

template <typename T>
inline bool EntityHandle<T>::operator==(T *pEntity) const
{
	assert(("EntityHandle<T>::operator==:  got a nullptr pointer!", pEntity != nullptr));

	if (m_serialnumber != pEntity->edict()->serialnumber)
	{
		return false;
	}

	return m_edict == pEntity->edict();
}

template <typename T>
inline EntityHandle<T>::operator int() const
{
	return Get() != nullptr;
}

// Gets the Entity this handle refers to.
// Returns null if invalid.
template <typename T>
inline EntityHandle<T>::operator T *() const
{
	return (T *)GET_PRIVATE(Get());
}

// Assigns the given entity to this handle.
template <typename T>
inline T *EntityHandle<T>::operator=(T *pEntity)
{
	if (pEntity)
	{
		m_edict = ENT(pEntity->pev);
		if (m_edict)
		{
			m_serialnumber = m_edict->serialnumber;
		}
	}
	else
	{
		m_edict = nullptr;
		m_serialnumber = 0;
	}

	return pEntity;
}

template <typename T>
inline T *EntityHandle<T>::operator->()
{
	edict_t *pEdict = Get();
	assert(("EntityHandle<T>::operator->:  pointer is nullptr!", pEdict != nullptr));

	T *pEntity = (T *)GET_PRIVATE(pEdict);
	assert(("EntityHandle<T>::operator->:  pvPrivateData is nullptr!", pEntity != nullptr));
	return pEntity;
}

// Makes this handle refer to the same entity as the given handle.
template <typename T>
inline EntityHandle<T>& EntityHandle<T>::operator=(const EntityHandle<T> &other)
{
	m_edict = other.m_edict;
	m_serialnumber = other.m_serialnumber;

	return (*this);
}
