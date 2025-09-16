// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMoveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_API UPlayerMoveComponent : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerMoveComponent();
	
	float WalkSpeed = 200.f; // uproperty 없다면 쓰레기값 초기화
	
	UPROPERTY(EditAnywhere, Category = "Setting")
	float RunSpeed = 600.f;

	UPROPERTY()
	class UCharacterMovementComponent* movement;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_run;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_lookup;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_jump;
	
private:
	FVector Direction = FVector::ZeroVector;



	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SetInputBinding(class UEnhancedInputComponent* playerinput) override;

	void MoveInput(const struct FInputActionValue& value);
	void RunInput(const struct FInputActionValue& value);
	void TurnInput(const struct FInputActionValue& value);
	void LookUpInput(const struct FInputActionValue& value);
	void JumpInput(const struct FInputActionValue& value);
};
