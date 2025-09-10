// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Pawn -> Player
	auto OwnerPawn = TryGetPawnOwner();
	ATPSPlayer* PlayerPawn = nullptr;
	if (OwnerPawn != nullptr)
		PlayerPawn = Cast<ATPSPlayer>(OwnerPawn);

	if (PlayerPawn != nullptr)
	{
		FVector Velocity = PlayerPawn->GetVelocity();
		FVector Forward = PlayerPawn->GetActorForwardVector();
		Speed = FVector::DotProduct(Velocity, Forward);

		FVector Right = PlayerPawn->GetActorRightVector();
		Direction = FVector::DotProduct(Velocity, Right);
		
		UCharacterMovementComponent* cmp = PlayerPawn->GetCharacterMovement();
		isInAir = cmp->IsFalling();
	}

}
