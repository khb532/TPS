#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

UCLASS()
class TPS_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnim();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	float Speed = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	float Direction = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	bool isInAir = false;

	// Load montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	class UAnimMontage* AttackMontage;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackAnim();
};
