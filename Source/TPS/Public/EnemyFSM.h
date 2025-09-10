// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle = 0 UMETA(DisplayName = "Idle"),
	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack"),
	Damage UMETA(DisplayName = "Damage"),
	Die UMETA(DisplayName = "Die"),
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	float CurTime;
	float IdleDelayTime;
	float ARange = 200.f;

	UPROPERTY(EditAnywhere, Category = "FSM")
	float DamageDelay = 2.f;

	UPROPERTY(EditAnywhere, Category = "FSM")
	float AttackDelay = 2.f;
	
	UPROPERTY()
	class ATPSPlayer* p_Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
	EEnemyState _state = EEnemyState::Idle;

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	int32 MaxHP = 3;
	int32 HP;

	UPROPERTY(EditAnywhere, Category = "FSM")
	float knockbackpow = 100.f;

	float lalpha;

	UPROPERTY()
	FVector KnockPos;
	FVector CurLocation;

	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();
	
	void OnDamageProcess(FVector ImpNormal);

	
private:
	UPROPERTY()
	class AEnemy* Owner;
};
