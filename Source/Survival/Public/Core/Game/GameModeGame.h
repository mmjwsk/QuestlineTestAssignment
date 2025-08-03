#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeGame.generated.h"


UCLASS()
class SURVIVAL_API AGameModeGame : public AGameModeBase
{
	GENERATED_BODY()

public:
	void HandlePlayerDeath(AController* DeadController);
	void RespawnPlayer(AController* Controller);

protected:
	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 5.f;
};
