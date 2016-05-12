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
		FRotator f(random.FRand() * 30 - 15, random.FRand() * 30 - 15, random.FRand() * 30 - 15);
		AddActorWorldRotation(f);
	}

}

void ALeaf::updateLocation(FVector branchLoc) {
	SetActorLocation(branchLoc + offsetVector);
}

UPrimitiveComponent* ALeaf::getComponentWithName(FString name) {
	TArray<UPrimitiveComponent*> comps;
	GetComponents(comps);
	for (auto Itr(comps.CreateIterator()); Itr; ++Itr)
	{
		if ((*Itr)->GetName() == name)
			return (*Itr);
	}

	throw - 1;
}

FVector ALeaf::getCenter() {
	return getComponentWithName("Sphere")->GetComponentLocation();
}