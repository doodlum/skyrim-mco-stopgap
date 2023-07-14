#include "MCO.h"

void MCO::ProcessBlock()
{
	if (auto player = RE::PlayerCharacter::GetSingleton()) {
		if (player->AsActorState()->actorState2.wantBlocking) {
			bool MCO_IsInRecovery = false;
			if (player->GetGraphVariableBool("MCO_IsInRecovery", MCO_IsInRecovery) && MCO_IsInRecovery)
			{
				logger::debug("MCO_EndAnimation wantBlocking");
				player->NotifyAnimationGraph("MCO_EndAnimation");
				player->NotifyAnimationGraph("blockStart");
			}
		}
	}
}
