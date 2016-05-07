// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Leaf.h"
#include "GameFramework/Actor.h"
#include "Branch.generated.h"


const int NOT_PLACED = -1;

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

	FVector getPositionOnBranch(float offset);
	UPrimitiveComponent* getComponentWithName(FString name);
	FVector getBegin();
	FVector getEnd();
	FVector getCenter();

	UPrimitiveComponent* getEndComponent();

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float rotationChance = .005f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float cost = 0.5;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		bool useOverlap = true;


	float calculateCost();

	void displace(FVector loc, FRotator rot, FVector origin);

	bool mutate();
	bool overlapsProps();

	int32 placedOn = NOT_PLACED;
	int32 myIndex = NOT_PLACED;

	FVector treeOffset;

private:
	FRandomStream random;
	FVector begin;
	FVector end;
};
