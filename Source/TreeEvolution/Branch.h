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

	FVector getPositionOnBranch(float offset);

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float branchMutationChance = .01f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafMutationChance = .04f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float displacementChance = .01f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float cost = 0.5;


	float calculateCost();

	void displace(FVector loc, FRotator rot);



private:
	FRandomStream random;
	FVector begin;
	FVector end;
};
