#include "MCO.h"

bool IsDualWielding(RE::PlayerCharacter* player)
{
	if (auto leftHand = player->GetEquippedObject(true))
	{
		if (auto leftWeapon = leftHand->As<RE::TESObjectWEAP>())
		{
			if ((int)leftWeapon->GetWeaponType() > 0 && (int)leftWeapon->GetWeaponType() < 5)
			{
				return true;
			}
		}
	}
	return !player->GetEquippedObject(false) && !player->GetEquippedObject(true);
}

void MCO::ProcessBlock()
{
	if (auto player = RE::PlayerCharacter::GetSingleton()) {
		bool MCO_IsInRecovery = false;
		if (player->GetGraphVariableBool("MCO_IsInRecovery", MCO_IsInRecovery) && MCO_IsInRecovery)
		{
			if (IsDualWielding(player))
			{
				logger::debug("MCO_EndAnimation dual wield");
				player->NotifyAnimationGraph("MCO_EndAnimation");
			}
			else if (player->AsActorState()->actorState2.wantBlocking) {
				logger::debug("MCO_EndAnimation wantBlocking");
				player->NotifyAnimationGraph("MCO_EndAnimation");
				player->NotifyAnimationGraph("blockStart");
			}
		}
	}
}
