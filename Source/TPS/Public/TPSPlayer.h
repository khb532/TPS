#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerBaseComponent* MoveComp;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

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
	class UInputAction* ia_fire;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_grenadegun;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipergun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipermode;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class ABullet> BulletClass;
	
	

	

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire")
	TSubclassOf<class UCameraShakeBase> fireshakeclass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire")
	class USoundBase* firesound;
	
	bool bUsingGrenade = false;
	// 
	
	
	void FireInput(const struct FInputActionValue& value);
	void ChangeToGrenadeGun(const struct FInputActionValue& value);
	void ChangeToSniperGun(const struct FInputActionValue& value);
	void SniperModeInput(const struct FInputActionValue& value);



	
};
