// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Branch.h"
#include "Tree.h"



// Sets default values
ABranch::ABranch()
{

	PrimaryActorTick.bCanEverTick = false;

	random.GenerateNewSeed();
}

// Called when the game starts or when spawned
void ABranch::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABranch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


FVector ABranch::getPositionOnBranch(float offset) {

	TArray<UPrimitiveComponent*> comps;
	GetComponents(comps);
	FVector begin;
	FVector end;

	// this is a strange way to find the end and starting components, should probably be refactored sometime...
	for (auto Itr(comps.CreateIterator()); Itr; ++Itr)
	{
		if ((*Itr)->GetName() == "start") {
			begin = (*Itr)->GetComponentLocation();
		}
		else if ((*Itr)->GetName() == "end")
		{
			end = (*Itr)->GetComponentLocation();

		}
	}

	return begin + (end - begin)*offset;
}

FVector ABranch::getBegin() {
	return getComponentWithName("start")->GetComponentLocation();
}

FVector ABranch::getEnd() {
	return getEndComponent()->GetComponentLocation();
}

FVector ABranch::getCenter() {
	return getComponentWithName("Cylinder")->GetComponentLocation();
}

UPrimitiveComponent* ABranch::getEndComponent() {
	return getComponentWithName("end");
}

UPrimitiveComponent* ABranch::getComponentWithName(FString name) {
	TArray<UPrimitiveComponent*> comps;
	GetComponents(comps);
	for (auto Itr(comps.CreateIterator()); Itr; ++Itr)
	{
		if ((*Itr)->GetName() == name)
			return (*Itr);

	}

	throw - 1;
}



float ABranch::calculateCost() {
	return cost;
}

bool ABranch::overlapsProps() {

	if (!useOverlap)
		return false;


	TArray< AActor * > OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (auto a : OverlappingActors) {
		ABranch* branch = Cast<ABranch>(a);
		ALeaf* leaf = Cast<ALeaf>(a);
		ATree* tree = Cast<ATree>(a);
		if (!branch && !leaf && !tree) {
			// prop was not a branch or a leaf or a tree, this means there is an overlap with another actor
			return true;
		}
	}

	return false;
}


void ABranch::displace(FVector loc, FRotator rot) {
	SetActorLocation(loc);
	SetActorRotation(rot);
}

bool ABranch::mutate() {
	if (random.FRand() < rotationChance) {
		AddActorLocalRotation(FQuat(FRotator(random.FRand() * 60 - 30, random.FRand() * 60 - 30, random.FRand() * 60 - 30)));
		while (overlapsProps()) {
			AddActorLocalRotation(FQuat(FRotator(random.FRand() * 60 - 30, random.FRand() * 60 - 30, random.FRand() * 60 - 30)));
		}
		return true;
	}
	return false;
}