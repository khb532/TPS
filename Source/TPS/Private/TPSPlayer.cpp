#include "TPSPlayer.h"

#include "Bullet.h"
#include "EnemyFSM.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAnim.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PlayerMoveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "TPS/TPS.h"


ATPSPlayer::ATPSPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
    if (tmpMesh.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(tmpMesh.Object);
        GetMesh()->SetRelativeLocation(FVector(0.f,0.f, -87.f));
        GetMesh()->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

        // Prefer AnimBlueprint (ABP_Player) if available; fallback to native class
        static ConstructorHelpers::FClassFinder<UAnimInstance> TmpABP(
            TEXT("/Game/Blueprints/ABP_Player.ABP_Player_C"));
        if (TmpABP.Succeeded())
        {
            GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            GetMesh()->SetAnimInstanceClass(TmpABP.Class);
        }
        else
        {
            GetMesh()->SetAnimInstanceClass(UPlayerAnim::StaticClass());
        }
    }
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.f, 60.f));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	fpgun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPGun"));
	fpgun->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpfp(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (tmpfp.Succeeded())
	{
		fpgun->SetSkeletalMesh(tmpfp.Object);
		fpgun->SetRelativeLocation(FVector(2.780775f,1.505752f,-2.f));
		fpgun->SetRelativeRotation(FRotator(4.980925,14.980998,-0.436880));
	}
	
	sniper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper"));
	sniper->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpsniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper11.sniper11'"));
	ConstructorHelpers::FObjectFinder<UMaterial> tmpmat(TEXT("/Script/Engine.Material'/Game/SniperGun/sniper1.sniper1'"));
	if (tmpsniper.Succeeded() && tmpmat.Succeeded())
	{
		sniper->SetStaticMesh(tmpsniper.Object);
		sniper->SetRelativeLocation(FVector(-8.391204,29.543566,5.901428));
		sniper->SetRelativeScale3D(FVector(0.150000,0.150000,0.150000));
		sniper->SetRelativeRotation(FRotator(4.980925,14.563120,-5.019002)); 
		sniper->SetMaterial(0, tmpmat.Object);
	}
	
	JumpMaxCount = 2;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tmpvfx(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tmpvfx.Succeeded())
		bulletvfx = tmpvfx.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> tmpsfx(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tmpsfx.Succeeded())
		firesound = tmpsfx.Object;

	// Crosshair UI Constructor
	static ConstructorHelpers::FClassFinder<UUserWidget> _sniperwidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_sniperwidget.Succeeded())
		SniperUIClass = _sniperwidget.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> _crosswidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_CrosshairUI.WBP_CrosshairUI_C'"));
	if (_crosswidget.Succeeded())
		CrosshairUIClass = _crosswidget.Class;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> tmpshake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_CameraShake.BP_CameraShake_C'"));
	if (tmpshake.Succeeded())
		fireshakeclass = tmpshake.Class;
	
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> tmpimc(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/TPSInput/IMC_TPS.IMC_TPS'"));
	if (tmpimc.Succeeded())
		imc_tps = tmpimc.Object;
	
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

	// MoveComp
	MoveComp = CreateDefaultSubobject<UPlayerMoveComponent>(TEXT("MoveComp"));;
	
}

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

	
	
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* playerinput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerinput)
	{
		MoveComp->SetInputBinding(playerinput);
		
		
		playerinput->BindAction(ia_fire, ETriggerEvent::Started, this, &ATPSPlayer::FireInput);
		playerinput->BindAction(ia_grenadegun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		playerinput->BindAction(ia_snipergun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		playerinput->BindAction(ia_snipermode, ETriggerEvent::Started, this, &ATPSPlayer::SniperModeInput);
		playerinput->BindAction(ia_snipermode, ETriggerEvent::Completed, this, &ATPSPlayer::SniperModeInput);
		
		
	}

}





void ATPSPlayer::FireInput(const struct FInputActionValue& value)
{
	//Anim play
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (anim)
	{
		anim->PlayAttackAnim();
	}
	// Camera Shake
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(fireshakeclass);

	// sfx
	UGameplayStatics::PlaySound2D(GetWorld(), firesound);
	
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
