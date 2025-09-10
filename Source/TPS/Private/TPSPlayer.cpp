// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"

#include "Bullet.h"
#include "EnemyFSM.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "TPS/TPS.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if (tmpMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tmpMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f,0.f, -87.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f,-90.f,0.f));
	}
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.f, 60.f));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	fpgun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPGun"));
	fpgun->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpfp(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (tmpfp.Succeeded())
	{
		fpgun->SetSkeletalMesh(tmpfp.Object);
		fpgun->SetRelativeLocation(FVector(40.f,0.f,90.f));
	}
	
	sniper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper"));
	sniper->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpsniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper11.sniper11'"));
	ConstructorHelpers::FObjectFinder<UMaterial> tmpmat(TEXT("/Script/Engine.Material'/Game/SniperGun/sniper1.sniper1'"));
	if (tmpsniper.Succeeded() && tmpmat.Succeeded())
	{
		sniper->SetStaticMesh(tmpsniper.Object);
		sniper->SetRelativeLocation(FVector(-40.f,0.f,90.f));
		sniper->SetRelativeScale3D(FVector(0.2f));
		sniper->SetMaterial(0, tmpmat.Object);
	}
	
	JumpMaxCount = 2;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tmpvfx(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tmpvfx.Succeeded())
		bulletvfx = tmpvfx.Object;

	// Crosshair UI Constructor
	static ConstructorHelpers::FClassFinder<UUserWidget> _sniperwidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_sniperwidget.Succeeded())
		SniperUIClass = _sniperwidget.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> _crosswidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_CrosshairUI.WBP_CrosshairUI_C'"));
	if (_crosswidget.Succeeded())
		CrosshairUIClass = _crosswidget.Class;
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// input mapping context alloc
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
			subsys->AddMappingContext(imc_tps, 0);
	}

	// Object Pool
	Mag.Empty();
	for (int i = 0; i < MagSize; i++)
	{
		ABullet * pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass);
		Mag.Add(pBullet);
		
	}
	
	p_SniperUI = CreateWidget(pc, SniperUIClass);
	p_Crosshair = CreateWidget(pc, CrosshairUIClass);
	p_Crosshair->AddToViewport();
	
	// sniper active
	ChangeToSniperGun(FInputActionValue());

	// Velocity initialize
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Direction = FTransform(GetControlRotation()).TransformVector(Direction);
	
	Direction.Normalize();
	/*SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime, true);*/
	AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* playerinput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerinput)
	{
		playerinput->BindAction(ia_move, ETriggerEvent::Triggered, this, &ATPSPlayer::MoveInput);
		playerinput->BindAction(ia_turn, ETriggerEvent::Triggered, this, &ATPSPlayer::TurnInput);
		playerinput->BindAction(ia_lookup, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUpInput);
		playerinput->BindAction(ia_jump, ETriggerEvent::Started, this, &ATPSPlayer::JumpInput);
		playerinput->BindAction(ia_fire, ETriggerEvent::Started, this, &ATPSPlayer::FireInput);
		playerinput->BindAction(ia_grenadegun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		playerinput->BindAction(ia_snipergun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		playerinput->BindAction(ia_snipermode, ETriggerEvent::Started, this, &ATPSPlayer::SniperModeInput);
		playerinput->BindAction(ia_snipermode, ETriggerEvent::Completed, this, &ATPSPlayer::SniperModeInput);
		playerinput->BindAction(ia_run, ETriggerEvent::Started, this, &ATPSPlayer::RunInput);
		playerinput->BindAction(ia_run, ETriggerEvent::Completed, this, &ATPSPlayer::RunInput);
	}

}

void ATPSPlayer::MoveInput(const struct FInputActionValue& value)
{
	FVector2D axis = value.Get<FVector2D>();
	Direction.X = axis.X;
	Direction.Y = axis.Y;
}

void ATPSPlayer::RunInput(const struct FInputActionValue& value)
{
	bool isPressed = value.Get<bool>();
	if (isPressed)
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		
}

void ATPSPlayer::TurnInput(const struct FInputActionValue& value)
{
	AddControllerYawInput(value.Get<float>());
}

void ATPSPlayer::LookUpInput(const struct FInputActionValue& value)
{
	AddControllerPitchInput(value.Get<float>());
}

void ATPSPlayer::JumpInput(const struct FInputActionValue& value)
{
	Jump();
}

void ATPSPlayer::FireInput(const struct FInputActionValue& value)
{
	FTransform firepos = fpgun->GetSocketTransform(TEXT("FirePosition"));

	if(bUsingGrenade)
	{
		if (Mag.Num() > 0)
		{
			Mag[0]->SetActorTransform(firepos);
			Mag[0]->SetActiveBullet(true);
			Mag.RemoveAt(0);
		}
	}
	else 
	{
		// line trace : x, y
		// trace by channel / profile / objecttype
		FVector startpos = Camera->GetComponentLocation();
		FVector endpos = startpos + Camera->GetForwardVector() * 10000.0f;

		FHitResult HitResult;
		FCollisionQueryParams param;	// linetrace parameter
		param.AddIgnoredActor(this);	// ignore player
		bool b_Hit = GetWorld()->LineTraceSingleByChannel(HitResult, startpos, endpos, ECollisionChannel::ECC_Visibility, param);

		if (b_Hit)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletvfx, HitResult.Location);
			if (HitResult.GetComponent()->IsSimulatingPhysics())
			{
				HitResult.GetComponent()->AddImpulseAtLocation(HitResult.ImpactPoint*1000, HitResult.Location);
			}
			if (b_Hit)
			{
				// UActorComponent* enemy = HitResult.GetActor()->GetComponentByClass(UEnemyFSM::StaticClass());
				auto enemy2 = Cast<UEnemyFSM>(HitResult.GetActor()->GetDefaultSubobjectByName(TEXT("FSM")));
				if (enemy2)
				{
					enemy2->OnDamageProcess(Camera->GetForwardVector());
				}
			}
		}
	}
}	

void ATPSPlayer::ChangeToGrenadeGun(const struct FInputActionValue& value)
{
	fpgun->SetVisibility(true);
	sniper->SetVisibility(false);
	bUsingGrenade = true;
}

void ATPSPlayer::ChangeToSniperGun(const struct FInputActionValue& value)
{
	fpgun->SetVisibility(false);
	sniper->SetVisibility(true);
	bUsingGrenade = false;
}

void ATPSPlayer::SniperModeInput(const struct FInputActionValue& value)
{
	if (bUsingGrenade)
		return;

	bool b_Input = value.Get<bool>();
	if (b_Input)
	{
		p_SniperUI->AddToViewport();
		p_Crosshair->RemoveFromParent();
		Camera->SetFieldOfView(45);
	}
	else
	{
		p_SniperUI->RemoveFromParent();
		p_Crosshair->AddToViewport();
		Camera->SetFieldOfView(90);
	}
}
