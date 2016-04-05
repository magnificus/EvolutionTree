// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Leaf.generated.h"

UCLASS()
class TREEEVOLUTION_API ALeaf : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALeaf();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float cost = 1;

//	ALeaf* duplicate(ALeaf* spawnedLeaf, FVector originalLocation, FVector newLocation);

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafMutationChance = .005f;

	void mutate();
	void mutateSelf();
	void annihilate();

private:
	//TSubclassOf<class ALeaf> Leaf_BP;
	FRandomStream random;
	
};
