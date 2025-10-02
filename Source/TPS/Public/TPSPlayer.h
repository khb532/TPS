#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"



DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, UEnhancedInputComponent*);

class UEnhancedInputComponent;
class UPlayerBaseComponent;
class UPlayerFireComponent;

UCLASS()
class TPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSPlayer();

	FInputBindingDelegate OnInputBindingDelegate;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void OnHitEvent();

	UFUNCTION(BlueprintNativeEvent)
	void MakeHpPercent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeWeapon(bool IsGrenade);
	
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerBaseComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerFireComponent* FireComp;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP")
	float MaxHp = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	float Hp = MaxHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	float percent = 0.f;
	
};
