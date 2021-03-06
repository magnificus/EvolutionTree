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
		void introduceRandomDNA();
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
		void hillClimb();
	

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		int32 GetNumTrees() { return nbrTrees; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SetValues(int32 numT, int32 numB, int32 numL) {
		nbrTrees = numT; numBranches = numB; numLeafs = numL; init();
	}
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		int32 GetNumBranches() { return numBranches; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		int32 GetNumLeafs() { return numLeafs;  }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void forceReCalculation();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void setSun(float theta, float phi);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void setMode(int m);

	UFUNCTION(BlueprintCallable, Category = "DATA")
		void writeHistoryToFile();

	
	UPROPERTY(EditAnywhere, Category = "Gameplay")

		int32 distance = 2000;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector currentBestLocation;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numBranches = 30;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numLeafs = 70;
	UPROPERTY(EditAnywhere, Category = "Performance")
		int32 nbrTrees = 100;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 cullingConstant = 5;
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

	TArray<float> averageHistory;
	TArray<float> bestHistory;
	
};
