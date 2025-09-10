// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* fpgun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniper;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_tps;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_run;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_lookup;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_fire;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_grenadegun;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipergun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipermode;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class ABullet> BulletClass;
	
	float WalkSpeed = 200.f; // uproperty 없다면 쓰레기값 초기화
	
	UPROPERTY(EditAnywhere, Category = "Setting")
	float RunSpeed = 600.f;

	FVector Direction = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 MagSize = 20;
	
	UPROPERTY()
	TArray<class ABullet*> Mag;

	UPROPERTY()
	class UUserWidget* p_SniperUI;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ui")
	TSubclassOf<class UUserWidget> SniperUIClass;

	UPROPERTY()
	class UUserWidget* p_Crosshair;

	UPROPERTY(EditDefaultsOnly, Category = "Ui")
	TSubclassOf<class UUserWidget> CrosshairUIClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* bulletvfx;
	
	bool bUsingGrenade = false;
	// 
	void MoveInput(const struct FInputActionValue& value);
	void RunInput(const struct FInputActionValue& value);
	void TurnInput(const struct FInputActionValue& value);
	void LookUpInput(const struct FInputActionValue& value);
	void JumpInput(const struct FInputActionValue& value);
	void FireInput(const struct FInputActionValue& value);
	void ChangeToGrenadeGun(const struct FInputActionValue& value);
	void ChangeToSniperGun(const struct FInputActionValue& value);
	void SniperModeInput(const struct FInputActionValue& value);



	
};
