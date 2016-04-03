// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Leaf.h"



// Sets default values
ALeaf::ALeaf()
{
	//static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	//if (LeafFinder.Object != NULL)
	//	Leaf_BP = LeafFinder.Object;

	PrimaryActorTick.bCanEverTick = false;

	random.GenerateNewSeed();

}

// Called when the game starts or when spawned
void ALeaf::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALeaf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//ALeaf* ALeaf::duplicate(ALeaf* spawnedLeaf, FVector originalLocation, FVector newLocation) {
//		FVector diff = GetActorLocation() - originalLocation;
//		FVector location = newLocation + diff;
//		//ALeaf* const spawnedLeaf = World->SpawnActor<ALeaf>(Leaf_BP, location, GetActorRotation());
//		return spawnedLeaf;
//
//}

void ALeaf::mutate() {
	if (random.FRand() < leafMutationChance) {
		
		FRotator r(random.FRandRange(0, 4) * .25, random.FRandRange(0, 4) * .25, random.FRandRange(0, 4) * .25);
		SetActorRotation(FQuat(r));
	}
	
}