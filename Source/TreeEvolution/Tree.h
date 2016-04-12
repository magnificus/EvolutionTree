// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Branch.h"
#include "Leaf.h"
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
		void duplicate(ATree* tree, FVector location);

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float hemisphereHits();

	UFUNCTION(BlueprintCallable, Category = "Mutation")
		void mutate();

	FTransform GetRandomPosition();

	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float zDist = 500;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float maxSpread = 600;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float numberRays = 30;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		bool debugLine = false;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float displacementChance = .01f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float hitRewardMultiplier = 800;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafChangeBranchChance = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numBranches = 5;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numLeafs = 20;

	
	void init();

	void initRandomLeaf();
	void initRandomBranch();

	void addBranch(ABranch* b);
	void addLeaf(ABranch* b, ALeaf* l);

	void annihilate();

	float currentValue = 0;

	TArray<ABranch*> branches;
	TArray<ALeaf*> leafs;

private:
	FRandomStream random;
	TSubclassOf<class ABranch> Branch_BP;
	TSubclassOf<class ALeaf> Leaf_BP;	
};

