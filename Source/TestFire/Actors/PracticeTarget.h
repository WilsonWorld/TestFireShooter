#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PracticeTarget.generated.h"

UCLASS()
class TESTFIRE_API APracticeTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetMesh")
		class UStaticMeshComponent* TargetMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetAudio")
		class USoundBase* BreakingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetHealth")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetValue")
		uint8 PointValue;

	// Sets default values for this actor's properties
	APracticeTarget();

	void IncreasePlayerScore();

protected:
	UFUNCTION()
		void TakeAnyDamage(AActor* damagedActor, float Damage, const class UDamageType* damageType, class AController* InstigatedBy, AActor* damageCauser);

};
