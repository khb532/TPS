#include "PlayerBaseComponent.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.h"


UPlayerBaseComponent::UPlayerBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!Owner)
		Owner = Cast<ACharacter>(GetOwner());

	// Delegate Bind
	if (Owner)
	{
		ATPSPlayer* p_Owner = Cast<ATPSPlayer>(Owner);
		p_Owner->OnInputBindingDelegate.AddUObject(this, &UPlayerBaseComponent::SetInputBinding);
	}
}


