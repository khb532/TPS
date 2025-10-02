#include "PlayerFireComponent.h"

#include "Bullet.h"
#include "EnemyFSM.h"
#include "EnhancedInputComponent.h"
#include "PlayerAnim.h"
#include "TPSPlayer.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


UPlayerFireComponent::UPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiafire(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSFire.IA_TPSFire'"));
	if ( tmpiafire.Succeeded())
		ia_fire = tmpiafire.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiagren(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSGrenadeGun.IA_TPSGrenadeGun'"));
	if ( tmpiagren.Succeeded())
		ia_grenadegun = tmpiagren.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiasniper(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSSniperGun.IA_TPSSniperGun'"));
	if ( tmpiasniper.Succeeded())
		ia_snipergun = tmpiasniper.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> tmpiasnimode(TEXT("/Script/EnhancedInput.InputAction'/Game/TPSInput/IA_TPSSniperMode.IA_TPSSniperMode'"));
	if ( tmpiasnimode.Succeeded())
		ia_snipermode = tmpiasnimode.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tmpvfx(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tmpvfx.Succeeded())
		bulletvfx = tmpvfx.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> tmpsfx(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tmpsfx.Succeeded())
	firesound = tmpsfx.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> SniperWidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_SniperUI.WBP_SniperUI_C'"));
	if (SniperWidgetFinder.Succeeded())
		SniperUIClass = SniperWidgetFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairWidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_CrosshairUI.WBP_CrosshairUI_C'"));
	if (CrosshairWidgetFinder.Succeeded())
		CrosshairUIClass = CrosshairWidgetFinder.Class;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShakeFinder(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_CameraShake.BP_CameraShake_C'"));
	if (CameraShakeFinder.Succeeded())
		fireshakeclass = CameraShakeFinder.Class;
}


void UPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Owner)
	{
		Owner = Cast<ACharacter>(GetOwner());
	}

	CachedPlayer = Cast<ATPSPlayer>(GetOwner());

	InitializeWeaponReferences();
	InitializeFireResources();
	InitializeBulletPool();

	APlayerController* PlayerController = Owner ? Cast<APlayerController>(Owner->GetController()) : nullptr;
	InitializeWidgets(PlayerController);

	ChangeToSniperGun(FInputActionValue());
}


void UPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerFireComponent::SetInputBinding(class UEnhancedInputComponent* playerinput)
{
	Super::SetInputBinding(playerinput);

	playerinput->BindAction(ia_fire, ETriggerEvent::Started, this, &UPlayerFireComponent::FireInput);
	playerinput->BindAction(ia_grenadegun, ETriggerEvent::Started, this, &UPlayerFireComponent::ChangeToGrenadeGun);
	playerinput->BindAction(ia_snipergun, ETriggerEvent::Started, this, &UPlayerFireComponent::ChangeToSniperGun);
	playerinput->BindAction(ia_snipermode, ETriggerEvent::Started, this, &UPlayerFireComponent::SniperModeInput);
	playerinput->BindAction(ia_snipermode, ETriggerEvent::Completed, this, &UPlayerFireComponent::SniperModeInput);
}

void UPlayerFireComponent::FireInput(const struct FInputActionValue& value)
{
	if (!Owner)
	{
		Owner = Cast<ACharacter>(GetOwner());
	}

	if (!Owner)
	{
		return;
	}

	UWorld* World = GetWorld();

	// Ensure cached references are valid
	if (!FPGun || !Camera)
	{
		InitializeWeaponReferences();
	}

	// Anim play
	auto anim = Cast<UPlayerAnim>(Owner->GetMesh()->GetAnimInstance());
	if (anim)
	{
		anim->PlayAttackAnim();
	}

	APlayerController* Controller = Cast<APlayerController>(Owner->GetController());
	if (!Controller && World)
	{
		Controller = World->GetFirstPlayerController();
	}
	if (Controller && Controller->PlayerCameraManager && fireshakeclass)
	{
		Controller->PlayerCameraManager->StartCameraShake(fireshakeclass);
	}

	if (firesound && World)
	{
		UGameplayStatics::PlaySound2D(World, firesound);
	}

	if (!FPGun)
	{
		return;
	}

	const FTransform FireTransform = FPGun->GetSocketTransform(TEXT("FirePosition"));

	if (bUsingGrenade)
	{
		if (Mag.Num() > 0 && Mag[0])
		{
			ABullet* Bullet = Mag[0];
			Mag.RemoveAt(0);
			Bullet->SetActorTransform(FireTransform);
			Bullet->SetActiveBullet(true);
		}
	}
	else
	{
		if (!Camera)
		{
			return;
		}

		const FVector StartPos = Camera->GetComponentLocation();
		const FVector EndPos = StartPos + Camera->GetForwardVector() * 10000.0f;

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		if (World && World->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			if (bulletvfx)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, bulletvfx, HitResult.Location);
			}

			if (HitResult.GetComponent() && HitResult.GetComponent()->IsSimulatingPhysics())
			{
				HitResult.GetComponent()->AddImpulseAtLocation(HitResult.ImpactPoint * 1000.0f, HitResult.Location);
			}

			if (AActor* HitActor = HitResult.GetActor())
			{
				auto EnemyFSM = Cast<UEnemyFSM>(HitActor->GetDefaultSubobjectByName(TEXT("FSM")));
				if (EnemyFSM && Camera)
				{
					EnemyFSM->OnDamageProcess(Camera->GetForwardVector());
				}
			}
		}
	}
}	

void UPlayerFireComponent::ChangeToGrenadeGun(const struct FInputActionValue& value)
{
	if (!FPGun || !Sniper)
	{
		InitializeWeaponReferences();
	}

	if (FPGun)
	{
		FPGun->SetVisibility(true);
	}
	if (Sniper)
	{
		Sniper->SetVisibility(false);
	}

	bUsingGrenade = true;
	CachedPlayer->ChangeWeapon(bUsingGrenade);
}

void UPlayerFireComponent::ChangeToSniperGun(const struct FInputActionValue& value)
{
	if (!FPGun || !Sniper)
	{
		InitializeWeaponReferences();
	}

	if (FPGun)
	{
		FPGun->SetVisibility(false);
	}
	if (Sniper)
	{
		Sniper->SetVisibility(true);
	}

	bUsingGrenade = false;
	CachedPlayer->ChangeWeapon(bUsingGrenade);
}

void UPlayerFireComponent::SniperModeInput(const struct FInputActionValue& value)
{
	if (bUsingGrenade)
	{
		return;
	}

	bool b_Input = value.Get<bool>();
	if (b_Input)
	{
		if (p_SniperUI)
		{
			p_SniperUI->AddToViewport();
		}
		if (p_Crosshair)
		{
			p_Crosshair->RemoveFromParent();
		}
		if (Camera)
		{
			Camera->SetFieldOfView(45);
		}
	}
	else
	{
		if (p_SniperUI)
		{
			p_SniperUI->RemoveFromParent();
		}
		if (p_Crosshair)
		{
			p_Crosshair->AddToViewport();
		}
		if (Camera)
		{
			Camera->SetFieldOfView(90);
		}
	}
}

void UPlayerFireComponent::ReturnBulletToPool(ABullet* Bullet)
{
	if (!Bullet)
	{
		return;
	}

	if (!Mag.Contains(Bullet))
	{
		Mag.Add(Bullet);
	}
}

void UPlayerFireComponent::InitializeWeaponReferences()
{
	if (!CachedPlayer)
	{
		CachedPlayer = Cast<ATPSPlayer>(GetOwner());
	}

	FPGun = CachedPlayer ? CachedPlayer->fpgun : nullptr;
	Sniper = CachedPlayer ? CachedPlayer->sniper : nullptr;
	Camera = CachedPlayer ? CachedPlayer->Camera : (Owner ? Owner->FindComponentByClass<UCameraComponent>() : nullptr);
}

void UPlayerFireComponent::InitializeFireResources()
{
	bUsingGrenade = false;
}

void UPlayerFireComponent::InitializeWidgets(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	if (!p_SniperUI && SniperUIClass)
	{
		p_SniperUI = CreateWidget<UUserWidget>(PlayerController, SniperUIClass);
	}

	if (!p_Crosshair && CrosshairUIClass)
	{
		p_Crosshair = CreateWidget<UUserWidget>(PlayerController, CrosshairUIClass);
	}

	if (p_Crosshair && !p_Crosshair->IsInViewport())
	{
		p_Crosshair->AddToViewport();
	}
}

void UPlayerFireComponent::InitializeBulletPool()
{
	Mag.Empty();

	UWorld* World = GetWorld();
	
	if (!World || !BulletClass)
	{
		return;
	}

	for (int32 Index = 0; Index < MagSize; ++Index)
	{
		ABullet* NewBullet = World->SpawnActor<ABullet>(BulletClass);

		if (!NewBullet)
		{
			continue;
		}

		NewBullet->SetOwner(GetOwner());
		NewBullet->SetOwningFireComponent(this);
		NewBullet->SetActiveBullet(false);
		ReturnBulletToPool(NewBullet);
	}
}
