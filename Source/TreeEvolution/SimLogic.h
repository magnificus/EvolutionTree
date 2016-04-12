// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tree.h"
#include "GameFramework/Actor.h"
#include "SimLogic.generated.h"

UCLASS()
class TREEEVOLUTION_API ASimLogic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimLogic();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void init();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void simulationTick();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		float GetAverageFitness() { return averageFitness; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		float GetMaxFitness() { return maxFitness; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		FVector GetBestPosition() { return currentBestLocation; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		TArray<FVector> GetPositions() { 
		TArray<FVector> toReturn;
		for (ATree* t : trees) {
			toReturn.Add(t->GetActorLocation());
		}
		return toReturn;
	}

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void forceReCalculation();

	
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		int32 distance = 1000;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector currentBestLocation;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		bool sexualReproduction = true;

	UPROPERTY(EditAnywhere, Category = "Performance")
		int32 nbrLines = 10;

	void combine(ATree* newTree, ATree* p1, ATree* p2, FVector location);

	
	float averageFitness;
	float maxFitness;



private:
	TSubclassOf<class ATree> Tree_BP;
	TSubclassOf<class ABranch> Branch_BP;
	TSubclassOf<class ALeaf> Leaf_BP;
	TArray<ATree*> trees;
	FRandomStream random;
	ATree* currentBest;
	
};
