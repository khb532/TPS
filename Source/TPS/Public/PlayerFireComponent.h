#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerFireComponent.generated.h"

class ATPSPlayer;
class ABullet;
class UCameraComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_API UPlayerFireComponent : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UPlayerFireComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetInputBinding(class UEnhancedInputComponent* playerinput) override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_fire;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_grenadegun;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipergun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_snipermode;


	void FireInput(const struct FInputActionValue& value);
	void ChangeToGrenadeGun(const struct FInputActionValue& value);
	void ChangeToSniperGun(const struct FInputActionValue& value);
	void SniperModeInput(const struct FInputActionValue& value);

	void ReturnBulletToPool(class ABullet* Bullet);

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
	
private:
	void InitializeWeaponReferences();
	void InitializeFireResources();
	void InitializeWidgets(class APlayerController* PlayerController);
	void InitializeBulletPool();

	UPROPERTY()
	ATPSPlayer* CachedPlayer = nullptr;

	UPROPERTY()
	USkeletalMeshComponent* FPGun = nullptr;

	UPROPERTY()
	UStaticMeshComponent* Sniper = nullptr;

	UPROPERTY()
	UCameraComponent* Camera = nullptr;

};
