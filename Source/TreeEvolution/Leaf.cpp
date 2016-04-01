// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Leaf.h"



// Sets default values
ALeaf::ALeaf()
{
	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;
	//// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


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
	if (Leaf_BP == NULL) {
		return NULL;
	}
	FVector diff = GetActorLocation() - originalLocation;
	ALeaf* spawnedLeaf = (ALeaf*)GetWorld()->SpawnActor(Leaf_BP);

	spawnedLeaf->SetActorLocation(newLocation - diff);
	return spawnedLeaf;
}

