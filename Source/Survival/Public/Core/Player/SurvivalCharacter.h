#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SurvivalCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
// REFACTOR NOTE: Removed the struct Fwd-declaration and replaced it w/ include - it is not a good practice to fwd-declare USTRUCTs
// as it can sometimes lead to difficult-to-trace bugs so it's best avoided altogether.
//struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(Abstract)
class ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

	// REFACTOR NOTE: Replaced instances of T* w/ TObjectPtr<T> - in UE5 it's best practice to use TObjectPtr for UPROPERTY-exposed
	// pointers due to additional functionalities and benefits for the reflection system
	
	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> MouseLookAction;
	
public:
	ASurvivalCharacter();

protected:

	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

