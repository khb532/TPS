// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"
#include "EngineUtils.h"


// Sets default values
AEnemyManager::AEnemyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	FindSpawnPoints();
	CreateEnemy();
}

void AEnemyManager::CreateEnemy()
{
	int index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	if (SpawnPoints[index] != nullptr)
	{
		auto transform = SpawnPoints[index]->GetActorTransform();
		GetWorld()->SpawnActor<AEnemy>(EnemyFactory, transform);
	}
	
	float createtime = FMath::RandRange(MinTime, MaxTime);
	GetWorldTimerManager().SetTimer(TimerHandle,this, &AEnemyManager::CreateEnemy, createtime, false);
}

void AEnemyManager::FindSpawnPoints(void)
{
	for (TActorIterator<AActor> it (GetWorld()); it; ++it)
	{
		// Name : SpawnPoint >> TArray<>SpawnPoints
		auto spawn = *it;
		if (spawn->GetActorNameOrLabel().Contains(TEXT("BP_SpawnPoint")))
		{
			SpawnPoints.Add(spawn);
		}
	}
}