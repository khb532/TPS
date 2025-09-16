#include "Bullet.h"

#include "TPSPlayer.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetCollisionProfileName(TEXT("BlockAll"));
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetRelativeScale3D(FVector(0.62f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	MoveComp->InitialSpeed = 10000.f;
	MoveComp->MaxSpeed = 10000.f;
	MoveComp->bShouldBounce = true;
	MoveComp->Bounciness = 0.2f;
	// MoveComp->SetUpdatedComponent(); : ex)캐릭터의 별도 컴포넌트에 적용시키고 싶을때

	// InitialLifeSpan = 2.f;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	p_Player = CastChecked<ATPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()));
	
	SetActiveBullet(false);
	
	
	// GetWorldTimerManager().SetTimer(lifetimehandler, this, &ABullet::Destroythis, 2.f);
	
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*LifeTime += DeltaTime;
	if (LifeTime > DestroyTime)
		Destroy();*/
}

void ABullet::SetActiveBullet(bool bActive)
{
	if (bActive)
	{
		Mesh->SetVisibility(true);
		MoveComp->SetComponentTickEnabled(true);
		MoveComp->Velocity = MoveComp->InitialSpeed * GetActorForwardVector();
		Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		auto DieLambda = [this]()->void
		{
			// Destroy();
			SetActiveBullet(false);
			p_Player->Mag.Add(this);
		};
		FTimerHandle handler;
		GetWorldTimerManager().SetTimer(handler, FTimerDelegate::CreateLambda(DieLambda), DestroyTime, false);
	}
	else
	{
		Mesh->SetVisibility(false);
		MoveComp->SetComponentTickEnabled(false);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	}
}

void ABullet::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		MoveComp->InitialSpeed = speed;
		MoveComp->MaxSpeed = speed;
	}
}


