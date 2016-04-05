// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Branch.h"


// Sets default values
ABranch::ABranch()
{


	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	// beginning and end of branch...

	//int32 index = random.RandRange(0, branches.Num() - 1);
	TArray<UPrimitiveComponent*> comps;
	GetComponents(comps);
	FVector begin;
	FVector end;
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


float ABranch::calculateCost() {
	return cost;
}


void ABranch::displace(FVector loc, FRotator rot) {
	SetActorLocation(loc);
	SetActorRotation(rot);
}