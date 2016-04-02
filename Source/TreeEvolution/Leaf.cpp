// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Leaf.h"



// Sets default values
ALeaf::ALeaf()
{
	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;

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

ALeaf* ALeaf::duplicate(FVector originalLocation, FVector newLocation) {
	UWorld* const World = GetWorld();

	if (!World) {
		return NULL;
	}

	FVector diff = GetActorLocation() - originalLocation;
	FVector location = newLocation + diff;


	ALeaf* const spawnedLeaf = World->SpawnActor<ALeaf>(Leaf_BP, location, GetActorRotation());


	return spawnedLeaf;
}

void ALeaf::mutate() {
	if (random.FRand() < leafMutationChance) {
		FRotator r(random.FRand() * 360, random.FRand() * 360, random.FRand() * 360);
		SetActorRotation(FQuat(r));
	}
	
}