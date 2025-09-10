// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	float Speed = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	float Direction = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAnim")
	bool isInAir = false;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
