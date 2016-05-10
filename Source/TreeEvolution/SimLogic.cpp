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
	//currentBestLocation.X = -1000;
}

void ASimLogic::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASimLogic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// for all trees except the best, randomly decide if it dies or not, but where better trees (lower in the list) have a higher chance of survival, on average, a portion the size (1 / cullingConstant) will die and be replaced each generation.
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

		t->mutate(true);
	}

	averageFitness = totalFitness / trees.Num();

	averageHistory.Add(averageFitness);
	bestHistory.Add(maxFitness);



}

void ASimLogic::combine(ATree* newTree, ATree* p1, ATree* p2, FVector location) {
	newTree->buildFromDNA(p1->createChildDNA(p2));
}

void ASimLogic::init() {
	for (ATree* t : trees) {
		t->annihilate();
	}
	trees.Empty();


	AActor* sun = nullptr;

	if (currentBest != nullptr) {
		sun = currentBest->sunActor;
		currentBest->annihilate();
	}

	currentBest = GetWorld()->SpawnActor<ATree>(Tree_BP, currentBestLocation, FRotator(0, 0, 0));
	if (sun) {
		// transfer same displayed sun so it isn't duplicated if it already exists
		currentBest->sunActor = sun;
	}

	int xPos = 0;
	int yPos = 0;

	if (currentBest)
		currentBest->init(numBranches, numLeafs);

	int placed = 0;
	int32 currX = 0;
	int32 currY = 0;

	while (true) {
		xPos = currX * distance;
		for (currY = 0; currY < 10; ++currY) {
			if (placed == nbrTrees) {
				goto outOfLoop;
			}
			yPos = currY * distance;
			FVector spawnVector(xPos, yPos, 0);
			ATree* spawnedTree = GetWorld()->SpawnActor<ATree>(Tree_BP, spawnVector, FRotator(0, 0, 0));
			if (spawnedTree)
				spawnedTree->init(numBranches, numLeafs);

			trees.Add(spawnedTree);
			++placed;
		}
		++currX;
	}
outOfLoop:

	setSun(0, 0);



}

void ASimLogic::introduceRandomDNA() {
	trees.Sort([](const ATree& t1, const ATree& t2) { return t1.currentValue > t2.currentValue;});

	TArray<ATree*> winners;
	TArray<ATree*> losers;

	for (int32 i = 1; i < trees.Num(); ++i) {
		if (random.FRand() * trees.Num() * cullingConstant > i) {
			winners.Add(trees[i]);
		}
		else {
			losers.Add(trees[i]);
		}
	}
	for (ATree* t : losers) {
		for (int i = 0; i < 100; ++i) {
			t->mutate(false);
		}
		t->mutate(true);
	}
}

void ASimLogic::setMode(int m) {

	for (ATree* t : trees) {
		t->setMode(m);
	}

}

void ASimLogic::setSun(float theta, float phi) {

	for (ATree* t : trees) {
		t->setAngles(theta, phi);
	}

	if (currentBest) {
		currentBest->setAngles(theta, phi);
		currentBest->illustrateSun();
	}


}

void ASimLogic::forceReCalculation() {
	for (ATree* t : trees) {
		t->checkCollision();
		t->currentValue = t->calculateHits();
	}
	//simulationTick();
}


void ASimLogic::writeHistoryToFile() {
	FString SaveDirectory = FPaths::GameDir();
	FString FileName = FString("data.txt");
	FString TextToSave;

	TextToSave.Append("Average \t Best \n");
	for (int i = 0; i < averageHistory.Num(); ++i) {
		float average = averageHistory[i];
		float best = bestHistory[i];
		TextToSave.Append(FString::SanitizeFloat(average) + "\t" + FString::SanitizeFloat(best) + "\n");
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;

		FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath);
		

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("** Could not export data **"));
	}

}

void ASimLogic::hillClimb() {
	trees[0]->hillClimb();
	simulationTick();
}