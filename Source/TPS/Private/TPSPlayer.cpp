#include "TPSPlayer.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAnim.h"
#include "InputMappingContext.h"
#include "PlayerMoveComponent.h"
#include "PlayerFireComponent.h"
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

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> tmpimc(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/TPSInput/IMC_TPS.IMC_TPS'"));
	if (tmpimc.Succeeded())
		imc_tps = tmpimc.Object;
	
	
	// MoveComp
	MoveComp = CreateDefaultSubobject<UPlayerMoveComponent>(TEXT("MoveComp"));
	// FireComp
	FireComp = CreateDefaultSubobject<UPlayerFireComponent>(TEXT("FireComp"));
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
		OnInputBindingDelegate.Broadcast(playerinput);
		// MoveComp->SetInputBinding(playerinput);
		// FireComp->SetInputBinding(playerinput);
	}
}

void ATPSPlayer::OnHitEvent()
{
	PRINTLOG(TEXT("Damaged"));
	Hp--;
	MakeHpPercent();
	if (Hp <= 0)
		PRINTLOG(TEXT("Die"));
}

void ATPSPlayer::MakeHpPercent_Implementation()
{
	percent = Hp / MaxHp;
}
