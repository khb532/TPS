// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class TPS_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, Category = "SPawn")
	float MinTime = 1.f;
	UPROPERTY(EditAnywhere, Category = "SPawn")
	float MaxTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "SPawn")
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category = "SPawn")
	TSubclassOf<class AEnemy> EnemyFactory;

	FTimerHandle TimerHandle;

	void CreateEnemy(void);
	
	void FindSpawnPoints(void);
};
