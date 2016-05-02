// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Branch.h"
#include "Leaf.h"
#include <vector>

#include "GameFramework/Actor.h"
#include "Tree.generated.h"

const int MODE_STRAIGHT = 0;
const int MODE_MANUAL_DIRECTION = 1;
const int MODE_SWEEP = 2;
const int MODE_HEMISPHERE = 3;


using namespace std;

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

	UFUNCTION(BlueprintCallable, Category = "General")
		void hillClimb();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float sweepHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float hemisphereHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float manualDirectionHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		void exportTree();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHitsStraightAbove();

	UFUNCTION(BlueprintCallable, Category = "Mutation")
		void mutate(bool reCalc);

	void GetRandomPositionFor(ABranch* b, int recursiveLimit);

	float calculateCost();
	int lengthOfChain(ABranch* b);
	FRotator getR();

	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float zDist = 600;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float maxSpread = 600;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		float numberRays = 30;
	UPROPERTY(EditAnywhere, Category = "RayTrace")
		bool debugLine = false;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float displacementChance = .005f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafChangeBranchChance = 0.005f;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numBranches = 15;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numLeafs = 60;


	vector<float> createChildDNA(ATree* otherParent);

	void buildFromDNA(vector<float>);
	void init(int32 numB, int32 numL);


	int32 getNumBranches() { return branches.Num(); }
	int32 getNumLeafs() { return leafs.Num(); }

	void initRandomLeaf();
	void initRandomBranch();
	void displaceBranch(ABranch* b);
	void cascadePositionUpdate(ABranch* b, int limit);
	void checkCollision();
	bool selfInChain(ABranch* self, ABranch* current);
	vector<int> getChain(ABranch* b);
	vector<int> getDependencies(ABranch* b);

	void setAngles(float inTheta, float inPhi);
	void setMode(int m);

	void illustrateSun();

	void annihilate();

	float currentValue = 0;

	TArray<ABranch*> branches;
	TArray<ALeaf*> leafs;
	AActor* sunActor;


	TMap<ABranch*, TArray<ABranch*>> branchDependencies;
	TMap<ABranch*, TArray<ALeaf*>> leafDependencies;

	void testRotation(ABranch* b, FRotator r);
	void testRotation(ALeaf* l, FRotator r);

	void testLocation(ALeaf* l);

private:
	FRandomStream random;
	TSubclassOf<class ABranch> Branch_BP;
	TSubclassOf<class ALeaf> Leaf_BP;
	TSubclassOf<class AActor> Sun_BP;
	int mode = MODE_STRAIGHT;
	float theta = 0.0;
	float phi = 0.0;
	FVector sunPos;

};

