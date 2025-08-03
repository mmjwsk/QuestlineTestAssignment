#include "Core/Game/GameModeGame.h"

void AGameModeGame::HandlePlayerDeath(AController* DeadController)
{
	if (!DeadController) return;

	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGameModeGame::RespawnPlayer, DeadController);

	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, RespawnDelay, false);
}

void AGameModeGame::RespawnPlayer(AController* Controller)
{
	if (!Controller) return;

	// Remove existing pawn if still present
	if (APawn* Pawn = Controller->GetPawn())
	{
		Pawn->Destroy();
	}

	auto* SpawnPoint = FindPlayerStart(Controller);
	ensureMsgf(SpawnPoint, TEXT("Couldn't find Spawn Point to respawn Player!"));

	RestartPlayerAtPlayerStart(Controller, SpawnPoint);
}
