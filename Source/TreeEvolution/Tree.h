// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Branch.h"
#include "GameFramework/Actor.h"
#include "Tree.generated.h"



UCLASS()
class TREEEVOLUTION_API ATree : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATree();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "General")
		ATree* duplicate(FVector location);

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateCost();

	UFUNCTION(BlueprintCallable, Category = "Mutation")
		void mutate();

	void spawnRandomBranch();

	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float zDist = 1500;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float maxSpread = 1000;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float numberRays = 100;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		bool debugLine = false;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float spawnMutationChance = .05f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float removeMutationChance = .01f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float hitRewardMultiplier = 50000;

	void addBranch(ABranch* b);

	void annihilate();

	float currentValue = 0;

private:
	FRandomStream random;
	TSubclassOf<class ABranch> Branch_BP;
	TSubclassOf<class ATree> Tree_BP;
	TArray<ABranch*> branches;
};

