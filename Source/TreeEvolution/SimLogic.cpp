// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "SimLogic.h"


// Sets default values
ASimLogic::ASimLogic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass> TreeFinder(TEXT("Class'/Game/TreeBP.TreeBP_C'"));
	if (TreeFinder.Object != NULL)
		Tree_BP = TreeFinder.Object;

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

	if (trees.Num() == 0) {
		return;
	}

	if (currentBest != NULL) {
		currentBest->annihilate();
	}
	ATree* newTree = GetWorld()->SpawnActor<ATree>(Tree_BP, currentBestLocation, trees[0]->GetActorRotation());
	currentBest = trees[0]->duplicate(newTree, currentBestLocation, false);

	winners.Add(trees[0]);
	
	for (int32 i = 1; i < trees.Num(); ++i) {
		if (random.FRand() * trees.Num() > i) {
			winners.Add(trees[i]);
		} else{
			losers.Add(trees[i]);
		}
	}
	for (ATree* t : losers) {
		ATree* parent = winners[random.RandRange(0, winners.Num()-1)];

		ATree* newTree = GetWorld()->SpawnActor<ATree>(Tree_BP, t->GetActorLocation(), parent->GetActorRotation());
			parent->duplicate(newTree, t->GetActorLocation(), hidden);
		newTree->mutate();

		winners.Add(newTree);
		t->annihilate();
	}


	trees = winners;
}



void ASimLogic::init() {
	int xPos = 0;
	int yPos = 0;
	for (int32 i = 0; i < nbrLines; ++i) {
		xPos = i * distance;
		for (int32 j = 0; j < nbrLines; ++j) {
			yPos = j * distance;
			ATree* spawnedTree = (ATree*) GetWorld()->SpawnActor(Tree_BP);
			FVector spawnVector(xPos, yPos, 0);
			spawnedTree->SetActorLocation(spawnVector);
			trees.Add(spawnedTree);

		}

	}
}

