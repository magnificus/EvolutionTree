// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Branch.h"
#include "Leaf.h"
#include <vector>

#include "GameFramework/Actor.h"
#include "Tree.generated.h"

const int MODE_STRAIGHT = 0;
const int MODE_HEMISPHERE = 1;
const int MODE_STRAIGHT_HEMISPHERE = 2;


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

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float hemisphereHits();

	UFUNCTION(BlueprintCallable, Category = "Statistics")
		float calculateHitsStraightAbove();


	UFUNCTION(BlueprintCallable, Category = "Mutation")
		void mutate(bool reCalc);

	void GetRandomPositionFor(ABranch* b);
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
		float displacementChance = .001f;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		float leafChangeBranchChance = 0.002f;

	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numBranches = 15;
	UPROPERTY(EditAnywhere, Category = "Algorithm")
		int32 numLeafs = 60;

	vector<float> createChildDNA(ATree* otherParent);

	void buildFromDNA(vector<float>);
	void init();

	void initRandomLeaf();
	void initRandomBranch();
	void displaceBranch(ABranch* b);
	void cascadePositionUpdate(ABranch* b);
	void checkCollision();
	bool selfInChain(ABranch* self, ABranch* current);

	void addBranch(ABranch* b);
	void addLeaf(ABranch* b, ALeaf* l);

	void annihilate();

	float currentValue = 0;

	TArray<ABranch*> branches;
	//TArray<ABranch*> allowedToExtend;
	TArray<ALeaf*> leafs;

	TMap<ABranch*, TArray<ABranch*>> branchDependencies;
	TMap<ABranch*, TArray<ALeaf*>> leafDependencies;

private:
	FRandomStream random;
	TSubclassOf<class ABranch> Branch_BP;
	TSubclassOf<class ALeaf> Leaf_BP;
	int mode = MODE_STRAIGHT;
};

