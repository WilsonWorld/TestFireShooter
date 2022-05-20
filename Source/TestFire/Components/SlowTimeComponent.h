#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlowTimeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTFIRE_API USlowTimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly)
		float CurrentSlowPoints;

	UPROPERTY(EditDefaultsOnly)
		float MaxSlowPoints;

	UPROPERTY(EditDefaultsOnly)
		float SlowPointChangeRate;

	USlowTimeComponent();

	UFUNCTION()
		void OnSlowTimeStart();

	UFUNCTION()
		void OnSlowTimeEnd();

protected:
	void IncreaseSlowPoints();
	void DecreaseSlowPoints();

	virtual void BeginPlay() override;

private:
	FTimerHandle SlowTimeHandle;
	bool bTimeIsSlow;

	void ClearSlowTimer();
};
