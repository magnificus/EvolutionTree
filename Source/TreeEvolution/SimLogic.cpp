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

// Called when the game starts or when spawned
void ASimLogic::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASimLogic::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


void ASimLogic::simulationTick()
{
	trees.Sort([](const ATree& t1, const ATree& t2) { return t1.currentValue > t2.currentValue;});

	TArray<ATree*> winners;
	TArray<ATree*> losers;

	float totalFitness = 0;

	if (trees.Num() == 0) {
		return;
	}
	if (currentBest)
		currentBest->annihilate();
	ATree* newTree = GetWorld()->SpawnActor<ATree>(Tree_BP, currentBestLocation, trees[0]->GetActorRotation());
	currentBest = trees[0]->duplicate(newTree, currentBestLocation);


	winners.Add(trees[0]);
	maxFitness = trees[0]->currentValue;
	totalFitness += trees[0]->currentValue;
	for (int32 i = 1; i < trees.Num(); ++i) {
		totalFitness += trees[i]->currentValue;
		if (random.FRand() * trees.Num() * 4 > i) {
			winners.Add(trees[i]);
		} else{
			losers.Add(trees[i]);
		}
	}
	for (ATree* t : losers) {
		//ATree* parent = winners[random.RandRange(0, winners.Num()-1)];
		ATree* parent1 = winners[random.RandRange(0, winners.Num() - 1)];
		

		//ATree* newTree = GetWorld()->SpawnActor<ATree>(Tree_BP, t->GetActorLocation(), parent->GetActorRotation());
		//	parent->duplicate(newTree, t->GetActorLocation(), hidden);

		//ATree* newTree = GetWorld()->SpawnActor<ATree>(Tree_BP, t->GetActorLocation(), FRotator());
		if (sexualReproduction) {
			ATree* parent2 = winners[random.RandRange(0, winners.Num() - 1)];
			combine(t, parent1, parent2, t->GetActorLocation());
		} else{
			parent1->duplicate(t, t->GetActorLocation());
		}
		
		t->mutate();

		//winners.Add(newTree);
		//t->annihilate();
	}

	//trees = winners;

	averageFitness = totalFitness / trees.Num();
	
}

void ASimLogic::combine(ATree* newTree, ATree* p1, ATree* p2, FVector location) {
	
	vector<int> p1Branches;

	for (int i = 0; i < p1->branches.Num(); ++i) {
		ABranch* b;
		FVector treeLoc;
		if (random.FRand() > .5) {
			b = p1->branches[i];
			treeLoc = p1->GetActorLocation();
			p1Branches.push_back(i);
		} else {
			b = p2->branches[i];
			treeLoc = p2->GetActorLocation();
		}
		FVector diff = p1->branches[i]->GetActorLocation() - p1->GetActorLocation();
		//FVector diff = b->GetActorLocation() - treeLoc;
		FVector newLocation = location + diff;
		ABranch* spawnedBranch = newTree->branches[i];
		spawnedBranch->SetActorLocation(newLocation);
		spawnedBranch->SetActorRotation(p1->branches[i]->GetActorRotation());
		//ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, newLocation, p1->branches[i]->GetActorRotation());
		//	b->duplicate(spawnedBranch, GetActorLocation(), location);
		//newTree->addBranch(spawnedBranch);
	}

	for (int i = 0; i < p1->leafs.Num(); ++i) {
		ALeaf* l;
		FVector treeLoc;
		if (find(p1Branches.begin(), p1Branches.end(), i) != p1Branches.end()) {
			l = p1->leafs[i];
			treeLoc = p1->GetActorLocation();
		}
		else {
			l = p2->leafs[i];
			treeLoc = p2->GetActorLocation();
		}
		FVector newLocation = newTree->branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector;
		ALeaf* spawnedLeaf = newTree->leafs[i];
		spawnedLeaf->SetActorLocation(newLocation);
		spawnedLeaf->SetActorRotation(l->GetActorRotation());
		//ALeaf* spawnedLeaf = GetWorld()->SpawnActor<ALeaf>(Leaf_BP, newLocation, l->GetActorRotation());
		l->duplicate(spawnedLeaf);
		//newTree->addLeaf(newTree->branches[l->attachedToIndex], spawnedLeaf);
	}
}

void ASimLogic::init() {
	//FPlatformProcess::Sleep(2);

	int xPos = 0;
	int yPos = 0;
	currentBest = GetWorld()->SpawnActor<ATree>(Tree_BP, currentBestLocation, FRotator());
	for (int32 i = 0; i < nbrLines; ++i) {
		xPos = i * distance;
		for (int32 j = 0; j < nbrLines; ++j) {
			yPos = j * distance;
			FVector spawnVector(xPos, yPos, 0);

			ATree* spawnedTree = GetWorld()->SpawnActor<ATree>(Tree_BP, spawnVector, FRotator());
			if (spawnedTree)
				spawnedTree->init();
			
			//spawnedTree->SetActorLocation(spawnVector);
			trees.Add(spawnedTree);

		}

	}
}

