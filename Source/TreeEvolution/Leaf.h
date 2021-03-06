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

	void duplicate(ALeaf* spawnedLeaf);

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float rotationChance = .005f;

	void mutate();

	int32 attachedToIndex = 0;
	float branchOffset;
	FVector offsetVector;

	UPrimitiveComponent* getComponentWithName(FString name);
	FVector getCenter();

	void updateLocation(FVector branchLoc);

private:
	FRandomStream random;

	
	
};
