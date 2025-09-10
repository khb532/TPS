// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include <gsl/pointers>

#include "Enemy.h"
#include "TPSPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPS/TPS.h"


// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<AEnemy>(GetOwner());
	p_Player = Cast<ATPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()));
	HP = MaxHP;
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FString stateString = UEnum::GetValueAsString(_state);
	// PRINTLOG(TEXT("%s"), *stateString);
	GEngine->AddOnScreenDebugMessage(0, 0, FColor::Cyan, stateString, false);
	
	switch (_state)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
		
	case EEnemyState::Move:
		MoveState();
		break;
		
	case EEnemyState::Attack:
		AttackState();
		break;
		
	case EEnemyState::Damage:
		DamageState();
		break;
		
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	CurTime += GetWorld()->DeltaTimeSeconds;
	if (CurTime > IdleDelayTime)
	{
		_state = EEnemyState::Move;
		CurTime = 0;
	}
}

void UEnemyFSM::MoveState()
{
	if (!ensure(p_Player != nullptr))
		return;

	FVector Direction = p_Player->GetActorLocation() - Owner->GetActorLocation();
	float distance = Direction.Size();	// 
	Direction.Normalize();
	Owner->AddMovementInput(Direction);
	if (distance < ARange)
		_state = EEnemyState::Attack;
}

void UEnemyFSM::AttackState()
{	// 일정시간마다 공격
	CurTime += GetWorld()->DeltaTimeSeconds;
	if (CurTime > AttackDelay)
	{
		CurTime = 0.f;
		PRINTLOG(TEXT("Attack"));
	}
	// Out of Range ~: Chase
	FVector Direction = p_Player->GetActorLocation() - Owner->GetActorLocation();
	float distance = Direction.Size();
	if (distance > ARange)
		_state = EEnemyState::Move;
	
	Owner->SetActorRotation(UKismetMathLibrary::MakeRotFromZX(Owner->GetActorUpVector(), Direction));
}

void UEnemyFSM::DamageState()
{
	// 넉백
	lalpha += GetWorld()->DeltaTimeSeconds;
	// FVector lerp = FMath::Lerp(HitLocation, KnockPos - HitLocation, FMath::Clamp(lalpha, 0, 1));
	FVector lerp = FMath::Lerp(CurLocation, KnockPos, lalpha);
	float distance = FVector::Dist(Owner->GetActorLocation(), KnockPos);
	if (lalpha > 1.f || distance < KINDA_SMALL_NUMBER)
	{
		Owner->SetActorLocation(KnockPos);
		lalpha = 0.f;
		
		_state = EEnemyState::Idle;
	}
	Owner->SetActorLocation(lerp, true);
	
	/*CurTime += GetWorld()->DeltaTimeSeconds;
	if (CurTime > DamageDelay)
	{
		lalpha = 0.f;
		CurTime = 0.f;
	}*/
}

void UEnemyFSM::DieState()
{
	lalpha += GetWorld()->DeltaTimeSeconds;
	FVector Dest = FVector(CurLocation.X, CurLocation.Y, -10.f);
	FVector lerp = FMath::Lerp(CurLocation, Dest, lalpha);
	Owner->SetActorLocation(lerp);

	float distance = FVector::Dist(Owner->GetActorLocation(), Dest);

	if (distance < KINDA_SMALL_NUMBER || lalpha > 2.f)
	{
		lerp = Owner->GetActorLocation();
		lalpha = 0.f;
		Owner->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess(FVector ImpNormal)
{
	HP--;
	CurLocation = Owner->GetActorLocation();
	if(HP > 0)
	{
		ImpNormal.Z = 0.f;
		FVector force = ImpNormal * knockbackpow;
		KnockPos = Owner->GetActorLocation() + force;
		_state = EEnemyState::Damage;
	}
	else
	{
		_state = EEnemyState::Die;
	}
}
