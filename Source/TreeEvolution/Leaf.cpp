// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Leaf.h"



// Sets default values
ALeaf::ALeaf()
{

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

void ALeaf::duplicate(ALeaf* spawnedLeaf) {
		spawnedLeaf->attachedToIndex = attachedToIndex;
		spawnedLeaf->branchOffset = branchOffset;
		spawnedLeaf->offsetVector = offsetVector;

}

void ALeaf::mutate() {
	if (random.FRand() < rotationChance) {
		FRotator f(random.FRand()*20, random.FRand()*20, random.FRand()*20);
		AddActorWorldRotation(f);
	}
	
}