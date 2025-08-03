#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

class UInputMappingContext;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPawnPossessed, APawn*);

UCLASS(Abstract)
class SURVIVAL_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASurvivalPlayerController();

	FOnPawnPossessed OnPossessed;

protected:
	virtual void SetPawn(APawn* InPawn) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	virtual void SetupInputComponent() override;
};
