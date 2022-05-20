#pragma once

#include "CoreMinimal.h"
#include "TestFire/Actors/PracticeTarget.h"
#include "MovingTarget.generated.h"


UENUM(BlueprintType)
enum class ETarget : uint8
{
	First	UMETA(DisplayName = "First Target"),
	Second	UMETA(DisplayName = "Second Target")
};

UCLASS()
class TESTFIRE_API AMovingTarget : public APracticeTarget
{
	GENERATED_BODY()

public:
	AMovingTarget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathing")
		float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathing")
		ETarget StartingTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pathing", meta = (AllowPrivateAccess))
		class UBoxComponent* Target1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pathing", meta =  (AllowPrivateAccess))
		class UBoxComponent* Target2;

	FVector Target1Location;
	FVector Target2Location;
	FVector CurrentLocationTarget;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
