#ifndef BUILDING_EXT_H
#define BUILDING_EXT_H

#include <CCINIClass.h>
#include <BuildingClass.h>
#include <TechnoClass.h>
#include <set>

#include "PrismForwarding.h"
#include "../BuildingType/Body.h"
#include "../_Container.hpp"
#include "../../Ares.h"

#ifdef DEBUGBUILD
#include "../../Misc/Debug.h"
#endif

class BuildingExt
{
public:
	typedef BuildingClass TT;

	class ExtData : public Extension<TT>
	{
	private:

	public:
		HouseClass* OwnerBeforeRaid; //!< Contains the house which owned this building prior to it being raided and turned over to the raiding party.
		bool isCurrentlyRaided; //!< Whether this building is currently occupied by someone not the actual owner of the structure.
		bool ignoreNextEVA; //!< This is used when returning raided buildings, to decide whether to play EVA announcements about building capture.

		bool FreeUnits_Done; //!< Prevent free units and aircraft to be created multiple times. Set when the free units have been granted.
		bool AboutToChronoshift; //!< This building is going to be shifted. It should not be attacked with temporal weapons now. Otherwise it would disappear.

		bool InfiltratedBy(HouseClass *Enterer);
		BuildingExtras::cPrismForwarding PrismForwarding;

		std::set<TechnoClass *> RegisteredJammers; //!< Set of Radar Jammers which have registered themselves to be in range of this building. (Related to issue #305)

	public:
		ExtData(TT* const OwnerObject) : Extension<TT>(OwnerObject),
			OwnerBeforeRaid(NULL), isCurrentlyRaided(false), ignoreNextEVA(false), PrismForwarding(), FreeUnits_Done(false), AboutToChronoshift(false)
			{ };

		virtual ~ExtData() {
			BuildingTypeExtras::cPrismForwarding::RemoveFromNetwork(this->AttachedToObject, true);
		}

		virtual void SaveToStream(AresByteStream &pStm);

		virtual void LoadFromStream(AresByteStream &pStm, size_t Size, size_t &Offset);

		virtual void InvalidatePointer(void *ptr) {
			AnnounceInvalidPointer(this->OwnerBeforeRaid, ptr);
			RegisteredJammers.erase(reinterpret_cast<TechnoClass *>(ptr));
			AnnounceInvalidPointer(this->PrismForwarding.SupportTarget, ptr);
			AnnounceInvalidPointer(this->PrismForwarding.Senders, ptr);
		}

		// related to Advanced Rubble
		bool RubbleYell(bool beingRepaired = false); // This function triggers back and forth between rubble states.
		void KickOutOfRubble();

		// related to trench traversal
		bool canTraverseTo(BuildingClass* targetBuilding); // Returns true if people can move from the current building to the target building, otherwise false.
		void doTraverseTo(BuildingClass* targetBuilding); // This function moves as many occupants as possible from the current to the target building.
		bool sameTrench(BuildingClass* targetBuilding); // Checks if both buildings are of the same trench kind.

		// related to linkable buildings
		bool isLinkable(); //!< Returns true if this is a structure that can be linked to other structures, like a wall, fence, or trench. This is used to quick-check if the game has to look for linkable buildings in the first place. \sa canLinkTo()
		bool canLinkTo(BuildingClass* targetBuilding); //!< Checks if the current building can link to the given target building. \param targetBuilding the building to check for compatibility. \return true if the two buildings can be linked. \sa isLinkable()

		// related to raidable buildings
		void evalRaidStatus(); //!< Checks if the building is empty but still marked as raided, and returns the building to its previous owner, if so.

		void UpdateFirewall();
		void ImmolateVictims();
		void ImmolateVictim(ObjectClass * Victim);

		bool ReverseEngineer(TechnoClass * Victim); //!< Returns true if Victim wasn't buildable and now should be

		void KickOutClones(TechnoClass * Production);
	};

	static Container<BuildingExt> ExtMap;

	static DWORD GetFirewallFlags(BuildingClass *pThis);

	//static void ExtendFirewall(BuildingClass *pThis, CellStruct Center, HouseClass *Owner); // replaced by generic buildLines
	static void buildLines(BuildingClass*, CellStruct, HouseClass*); // Does the actual line-building, using isLinkable() and canLinkTo().

	static void UpdateDisplayTo(BuildingClass *pThis);

	static signed int GetImageFrameIndex(BuildingClass *pThis);

	static void KickOutHospitalArmory(BuildingClass *pThis);

	static CellStruct *TempFoundationData1;
	static CellStruct *TempFoundationData2;

	static DWORD FoundationLength(CellStruct * StartCell);

	static void Cleanup();
};

#endif
