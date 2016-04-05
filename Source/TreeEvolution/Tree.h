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
		ATree* duplicate(ATree* tree, FVector location, bool hidden);

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHits();
	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateCost();

	UFUNCTION(BlueprintCallable, Category = "Mutation")
		void mutate();

	FTransform GetRandomPosition();

	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float zDist = 500;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float maxSpread = 300;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float numberRays = 15;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		bool debugLine = false;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float spawnMutationChance = .1f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float removeMutationChance = .1f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float displacementChance = .1f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float hitRewardMultiplier = 800;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 maxBranches = 5;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 maxLeafs = 20;

	

	void addBranch(ABranch* b);

	void annihilate();

	float currentValue = 0;
	int32 currentBranches = 0;
	int32 currentLeafs = 0;

private:
	FRandomStream random;
	TSubclassOf<class ABranch> Branch_BP;
	//TSubclassOf<class ATree> Tree_BP;
	TArray<ABranch*> branches;

	
};

