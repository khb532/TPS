#include "EnemyFSM.h"

#include "AIController.h"
#include "Enemy.h"
#include "EnemyAnim.h"
#include "NavigationSystem.h"
#include "TPS/TPS.h"
#include "TPSPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"


UEnemyFSM::UEnemyFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 소유자 찾기
	me = Cast<AEnemy>(GetOwner());

	// AIController
	ai = Cast<AAIController>(me->GetController());

	
	// 타겟 찾기
	target = Cast<ATPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass()));

	// 애니메이션 블루프린트
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
}


void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType,
                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 현재 상태 출력
	FString stateStr = UEnum::GetValueAsString(_state);
	// PRINTLOG(TEXT("%s"), *stateStr);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, stateStr);
	
	switch (_state)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}

	if (HPBar)
	{
		FVector Direction = target->GetActorLocation() -
		me->GetActorLocation();
		Direction.Z = 0.f;
		//HPBar->SetRelativeRotation(Direction);
	}
}

bool UEnemyFSM::FindRandomPosInNavMesh(const FVector& pos, const float radius, FVector& destination)
{	// Get Random Position in NavMesh
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool bResult = ns->GetRandomReachablePointInRadius(pos, radius, loc);
	destination = loc.Location;
	return bResult;
}

// 일정시간이 지나면 상태를 이동으로 전환하고 싶다.
// 필요속성 : 대기시간, 경과시간
void UEnemyFSM::IdleState()
{
	// 일정시간이 지나면 상태를 이동으로 전환하고 싶다.
	// 1. 시간이 흘러야한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 경과시간이 대기시간을 초과하면
	if (currentTime > idleDelayTime)
	{
		// 3. 상태를 이동으로 전환한다.
		_state = EEnemyState::Move;
		currentTime = 0;
		// 4. 애니메이션 상태도 업데이트
		anim->_state = _state;
		FindRandomPosInNavMesh(me->GetActorLocation(), 500, RandomPos);
	}
}

// 타겟쪽으로 이동하고 싶다.
// 타겟이 공격범위 안에 들어오면 상태를 공격으로 전환하고 싶다.
void UEnemyFSM::MoveState()
{
	// 플레이어가 없으면??
	if (IsValid(target) == false)
	{
		return;		// 아무처리하지 않는다.
	}
	// p = p0 + vt
	FVector direction = target->GetActorLocation() - me->GetActorLocation();

	// 1. 타겟과의 거리가 필요하다.
	float distance = direction.Size();
	direction.Normalize();

	// /*CharMove*/ me->AddMovementInput(direction);
	// /*NoNavMesh*/ai->MoveToLocation(target->GetActorLocation());
	// /*NavMesh + Invok + GetRandomPos*/
	// if CanReach to RandomPos
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	// Create Query, 
	FPathFindingQuery query;
	FAIMoveRequest request;
	request.SetAcceptanceRadius(3.f);
	request.SetGoalLocation(target->GetActorLocation());
	ai->BuildPathfindingQuery(request, query);
	auto result = ns->FindPathSync(query);

	if (result.IsSuccessful())
	{
		ai->MoveToLocation(target->GetActorLocation());
	}
	else
	{
		EPathFollowingRequestResult::Type res = ai->MoveToLocation(RandomPos);
		
		if (res == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			FindRandomPosInNavMesh(me->GetActorLocation(), 500, RandomPos);
		}
	}
	
	// 타겟이 공격범위 안에 들어오면 상태를 공격으로 전환하고 싶다.
	// 2. 타겟과의 거리가 공격범위 안에 들어왔으니까.
	if (distance < attackRange)
	{
		// 3. 상태를 공격으로 전환
		_state = EEnemyState::Attack;
		anim->_state = _state;
		currentTime = attackDelayTime;
		ai->StopMovement();
	}
}

// 일정시간에 한번씩 공격하고 싶다.
// 타겟이 공격범위를 벗어 났다면 상태를 이동으로 전환하고 싶다.
void UEnemyFSM::AttackState()
{
	// 시간이 흘러야한다. -> 공격대기시간 만큼 기다린다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	
	if (currentTime > attackDelayTime)
	{
		// 공격
		currentTime = 0;
		if (target)
			target->OnHitEvent();
		anim->bAttackPlay = true;
	}

	// 타겟을 바라보게 하자.
	FVector dir = target->GetActorLocation() - me->GetActorLocation();
	FRotator rot = UKismetMathLibrary::MakeRotFromZX(me->GetActorUpVector(), dir);
	me->SetActorRotation(rot);
	
	// 거리
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		_state = EEnemyState::Move;
		anim->_state = _state;
		FindRandomPosInNavMesh(me->GetActorLocation(), 500, RandomPos);
	}
}

// 일정시간 기다렸다가 상태를 대기로 전환
void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		currentTime = 0;
		_state = EEnemyState::Idle;
		anim->_state = _state;
	}

	float percent = GetWorld()->DeltaTimeSeconds * 10;// 얼마나 빨리
	FVector P = FMath::Lerp(me->GetActorLocation(), knockbackPos, percent);
	// 원충돌
	float dist = FVector::Dist(P, me->GetActorLocation());
	if (dist < 10)
	{
		P = me->GetActorLocation();
	}
	else
	{
		me->SetActorLocation(P, true);
	}
}

// 아래로 내려가도 되면
// 아래로 사라지도록 하고 싶다.
// 안보이면 제거하고 싶다.
void UEnemyFSM::DieState()
{
	if (bDieProcessing == false)
	{
		return;
	}
	me->SetActorEnableCollision(false);
	me->SetActorLocation(me->GetActorLocation() + (-me->GetActorUpVector() * 100 * GetWorld()->DeltaTimeSeconds));
	
	if (me->GetActorLocation().Z < -80)
		me->Destroy();
}

void UEnemyFSM::OnDamageProcess(FVector hitDirection)
{
	ai->StopMovement();
	me->OnHit();
	// 체력
	hp--;

	// 경과시간 초기화
	currentTime = 0;
	
	// 체력이 남아있으면 피격상태
	if (hp > 0)
	{
		_state = EEnemyState::Damage;

		// 피격 애니메이션 재생
		int index = FMath::RandRange(0, 1);
		FName sectionName(FString::Printf(TEXT("Damage%d"), index));
		anim->PlayDamageAnim(sectionName);
		
		// 넉백 처리
		// P = P0 + v
		hitDirection.Z = 0;
		FVector force = hitDirection * knockbackPower;
		knockbackPos = me->GetActorLocation() + force;

	}
	// 그렇지 않으면 죽음
	else
	{
		me->SetActorEnableCollision(false);
		_state = EEnemyState::Die;
		anim->PlayDamageAnim(TEXT("Die"));
	}

	anim->_state = _state;
}
