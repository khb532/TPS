#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle UMETA(DisplayName = "IdleState"),
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyFSM();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction*
							   ThisTickFunction) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	EEnemyState _state = EEnemyState::Idle;

	// 컴포넌트 소유자 선언
	UPROPERTY()
	class AEnemy* me;

	UPROPERTY()
	class AAIController * ai;

	FVector RandomPos;
	bool FindRandomPosInNavMesh(const FVector& pos, const float radius, FVector& destination);

	// fsm 상태 함수 선언
	// 필요속성 : 대기시간, 경과시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float idleDelayTime = 2;
	float currentTime = 0;
	
	void IdleState();

	// 필요속성 : 타겟
	UPROPERTY()
	class ATPSPlayer* target;

	// 필요속성 : 공격범위
	UPROPERTY(EditAnywhere, Category=FSM)
	float attackRange = 205;
	
	void MoveState();
	// 필요속성 : 공격 대기시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float attackDelayTime = 2.0f;
	
	void AttackState();

	// 피격 대기 시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 2.0f;
	
	void DamageState();
	void DieState();

	// 피격시 호출될 이벤트 함수 -> 콜백
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=FSM)
	int32 MAX_HP = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FSM)
	int32 hp = MAX_HP;

	// 필요속성 : 넉백 파워
	UPROPERTY(EditAnywhere, Category=FSM)
	float knockbackPower = 50;
	FVector knockbackPos;
	void OnDamageProcess(FVector hitDirection);

	// 사용중인 애니메이션 블루프린트
	UPROPERTY()
	class UEnemyAnim* anim;

	// 아래로 내려가도 되면
	bool bDieProcessing = false;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HPBar;
	
};
