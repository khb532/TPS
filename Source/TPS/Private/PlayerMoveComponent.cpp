#include "PlayerMoveComponent.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "InputAction.h"


UPlayerMoveComponent::UPlayerMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiamove(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSMove.IA_TPSMove'"));
	if (tmpiamove.Succeeded())
		ia_move = tmpiamove.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiarun(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSRun.IA_TPSRun'"));
	if (tmpiarun.Succeeded())
		ia_run = tmpiarun.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiaturn(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSTurn.IA_TPSTurn'"));
	if (tmpiaturn.Succeeded())
		ia_turn = tmpiaturn.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpialookup(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSLookup.IA_TPSLookup'"));
	if (tmpialookup.Succeeded())
		ia_lookup = tmpialookup.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiajump(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSJump.IA_TPSJump'"));
	if (tmpiajump.Succeeded())
		ia_jump = tmpiajump.Object;
}


void UPlayerMoveComponent::BeginPlay()
{
    Super::BeginPlay();

    // Base에서 설정된 Owner(ACharacter*) 사용. 없으면 직접 보완.
    if (!Owner)
    {
        Owner = Cast<ACharacter>(GetOwner());
    }

    if (Owner)
    {
        movement = Owner->GetCharacterMovement();
    }
    if (!movement && GetOwner())
    {
        movement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
    }

    if (movement)
    {
        movement->MaxWalkSpeed = WalkSpeed;
    }
}


void UPlayerMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Owner)
    {
        return;
    }

    Direction = FTransform(Owner->GetControlRotation()).TransformVector(Direction);
    
    Direction.Normalize();
    /*SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime, true);*/
    Owner->AddMovementInput(Direction);
    Direction = FVector::ZeroVector;
}

void UPlayerMoveComponent::SetInputBinding(class UEnhancedInputComponent* playerinput)
{
	Super::SetInputBinding(playerinput);
	
	playerinput->BindAction(ia_move, ETriggerEvent::Triggered, this, &UPlayerMoveComponent::MoveInput);

	playerinput->BindAction(ia_turn, ETriggerEvent::Triggered, this, &UPlayerMoveComponent::TurnInput);
	playerinput->BindAction(ia_lookup, ETriggerEvent::Triggered, this, &UPlayerMoveComponent::LookUpInput);
	playerinput->BindAction(ia_jump, ETriggerEvent::Started, this, &UPlayerMoveComponent::JumpInput);
	playerinput->BindAction(ia_run, ETriggerEvent::Started, this, &UPlayerMoveComponent::RunInput);
	playerinput->BindAction(ia_run, ETriggerEvent::Completed, this, &UPlayerMoveComponent::RunInput);
}

void UPlayerMoveComponent::MoveInput(const struct FInputActionValue& value)
{
	FVector2D axis = value.Get<FVector2D>();
	Direction.X = axis.X;
	Direction.Y = axis.Y;
}

void UPlayerMoveComponent::RunInput(const struct FInputActionValue& value)
{
    bool isPressed = value.Get<bool>();
    if (!movement)
    {
        return;
    }

    movement->MaxWalkSpeed = isPressed ? RunSpeed : WalkSpeed;
        
}

void UPlayerMoveComponent::TurnInput(const struct FInputActionValue& value)
{
    if (Owner)
    {
        Owner->AddControllerYawInput(value.Get<float>());
    }
}

void UPlayerMoveComponent::LookUpInput(const struct FInputActionValue& value)
{
    if (Owner)
    {
        Owner->AddControllerPitchInput(value.Get<float>());
    }
}

void UPlayerMoveComponent::JumpInput(const struct FInputActionValue& value)
{
    if (Owner)
    {
        Owner->Jump();
    }
}
