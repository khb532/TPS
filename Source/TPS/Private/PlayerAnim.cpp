#include "PlayerAnim.h"

#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

UPlayerAnim::UPlayerAnim()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> tmp(TEXT("/Script/Engine.AnimMontage'/Game/Animation/My_Fire_Rifle_Ironsights_Montage.My_Fire_Rifle_Ironsights_Montage'"));
	if (tmp.Succeeded())
	{
		AttackMontage = tmp.Object;
	}
}

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

void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(AttackMontage);
}
