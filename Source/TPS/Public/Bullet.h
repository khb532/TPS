#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UPlayerFireComponent;

UCLASS()
class TPS_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MoveComp;

	UPROPERTY(EditAnywhere, Category = "Settings")
	// float LifeTime = 0.f;
	float DestroyTime = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float speed = 10000.f;

	void SetActiveBullet(bool bActive);

	UPROPERTY()
	class UPlayerFireComponent* OwnerFireComponent = nullptr;

	void SetOwningFireComponent(class UPlayerFireComponent* InComponent);

	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
};
