// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Leaf.h"
#include "GameFramework/Actor.h"
#include "Branch.generated.h"

UCLASS()
class TREEEVOLUTION_API ABranch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABranch();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void mutate();

	void annihilate();

	void spawnRandomLeaf();
	void spawnRandomBranch();
	FTransform getRandomPositionOnBranch(int offset);

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float branchMutationChance = .1f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafMutationChance = .6f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float cost = 1;

	ABranch* duplicate(FVector originalLocation, FVector newLocation);

	float calculateCost();

	void addBranch(ABranch* b);
	void addLeaf(ALeaf* l);


private:
	FRandomStream random;
	TSubclassOf<class ALeaf> Leaf_BP;
	TSubclassOf<class ABranch> Branch_BP;
	TArray<ALeaf*> leafs;
	TArray<ABranch*> branches;
};
