// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "SimLogic.h"
#include <vector>
#include <algorithm> 

using namespace std;

// Sets default values
ASimLogic::ASimLogic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass> TreeFinder(TEXT("Class'/Game/TreeBP.TreeBP_C'"));
	if (TreeFinder.Object != NULL)
		Tree_BP = TreeFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	if (BranchFinder.Object != NULL)
		Branch_BP = BranchFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;

	random.GenerateNewSeed();
	currentBestLocation.X = -1000;
}

void ASimLogic::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASimLogic::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


/*
This is the main logic function
*/

void ASimLogic::simulationTick()
{
	// sort the list of trees according to their fitness, making the best tree appear at position (0)
	trees.Sort([](const ATree& t1, const ATree& t2) { return t1.currentValue > t2.currentValue;});

	TArray<ATree*> winners;
	TArray<ATree*> losers;

	float totalFitness = 0;

	if (trees.Num() == 0) 
		return;
	
		trees[0]->duplicate(currentBest, currentBestLocation);

		winners.Add(trees[0]);
		maxFitness = trees[0]->currentValue;
		totalFitness += trees[0]->currentValue;

		int32 cullingConstant = 4;

		// for all trees except the best, randomly decide if it dies or not, but where better trees (lower in the list) have a higher chance of survival, on average, 1 / cullingConstant trees will die and be replaced each generation..
		for (int32 i = 1; i < trees.Num(); ++i) {
			totalFitness += trees[i]->currentValue;
			if (random.FRand() * trees.Num() * cullingConstant > i) {
				winners.Add(trees[i]);
			}
			else {
				losers.Add(trees[i]);
			}
		}
		for (ATree* t : losers) {
			// replace all losing trees with new trees that are either clones of old trees or children of two of them depending on the boolean "sexualReproduction"
			ATree* parent1 = winners[random.RandRange(0, winners.Num() - 1)];

			if (sexualReproduction) {
				ATree* parent2 = winners[random.RandRange(0, winners.Num() - 1)];
				combine(t, parent1, parent2, t->GetActorLocation());
			}
			else {
				parent1->duplicate(t, t->GetActorLocation());
			}

			t->mutate();

		}

		averageFitness = totalFitness / trees.Num();


	
}

void ASimLogic::combine(ATree* newTree, ATree* p1, ATree* p2, FVector location) {

	newTree->buildFromDNA(p1->createChildDNA(p2));

	// right now takes the branchpositions from one tree, and then randomly picks branches from tree 1 and tree 2 and then puts them in that position.
	//vector<int> p1Branches;

	//for (int i = 0; i < p1->branches.Num(); ++i) {
	//	if (random.FRand() > .5) {
	//		p1Branches.push_back(i);
	//	} 
	//	FVector diff = p1->branches[i]->GetActorLocation() - p1->GetActorLocation();
	//	FVector newLocation = location + diff;
	//	ABranch* spawnedBranch = newTree->branches[i];
	//	spawnedBranch->SetActorLocation(newLocation);
	//	spawnedBranch->SetActorRotation(p1->branches[i]->GetActorRotation());
	//}

	//for (int i = 0; i < p1->leafs.Num(); ++i) {
	//	ALeaf* l;
	//	if (find(p1Branches.begin(), p1Branches.end(), i) != p1Branches.end()) {
	//		l = p1->leafs[i];
	//	}
	//	else {
	//		l = p2->leafs[i];
	//	}
	//	FVector newLocation = newTree->branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector;
	//	ALeaf* spawnedLeaf = newTree->leafs[i];
	//	spawnedLeaf->SetActorLocation(newLocation);
	//	spawnedLeaf->SetActorRotation(l->GetActorRotation());
	//	l->duplicate(spawnedLeaf);
	//}
}

void ASimLogic::init() {
	for (ATree* t : trees) {
		t->annihilate();
	}
		trees.Empty();
	if (currentBest != nullptr) {
		currentBest->annihilate();
	}

	int xPos = 0;
	int yPos = 0;
	currentBest = GetWorld()->SpawnActor<ATree>(Tree_BP, currentBestLocation, FRotator(0,0,0));
	if (currentBest)
		currentBest->init();

	for (int32 i = 0; i < nbrLines; ++i) {
		xPos = i * distance;
		for (int32 j = 0; j < nbrLines; ++j) {
			yPos = j * distance;
			FVector spawnVector(xPos, yPos, 0);

			ATree* spawnedTree = GetWorld()->SpawnActor<ATree>(Tree_BP, spawnVector, FRotator(0, 0, 0));
			if (spawnedTree)
				spawnedTree->init();
			
			trees.Add(spawnedTree);

		}

	}
}


void ASimLogic::forceReCalculation() {
	for (ATree* t : trees) {
		t->currentValue = t->calculateHits();
	}
}