// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Tree.h"
#include "Leaf.h"


// Sets default values
ATree::ATree()
{
	static ConstructorHelpers::FObjectFinder<UClass> TreeFinder(TEXT("Class'/Game/TreeBP.TreeBP_C'"));
	if (TreeFinder.Object != NULL)
		Tree_BP = TreeFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	if (BranchFinder.Object != NULL)
		Branch_BP = BranchFinder.Object;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	random.GenerateNewSeed();



}

// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATree::calculateHits() {
	FVector startLocation = GetActorLocation();
	startLocation.Z += zDist;

	int hits = 0;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	FVector currStart;
	FVector currEnd;
	float relation = maxSpread / numberRays;
	for (int i = -numberRays / 2; i < numberRays / 2; ++i) {
		for (int j = -numberRays / 2; j < numberRays / 2; ++j) {
			currStart = startLocation;
			currStart.X += relation * i;
			currStart.Y += relation * j;

			currEnd = currStart;
			currEnd.Z -= zDist;

			//call GetWorld() from within an actor extending class
			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,        //result
				currStart,    //start
				currEnd, //end
				ECC_Pawn, //collision channel
				RV_TraceParams
				);
			if (debugLine) {
				DrawDebugLine(
					GetWorld(),
					currStart,
					currEnd,
					FColor(255, 0, 0),
					true, -1, 0,
					1
					);
			}

			if (RV_Hit.bBlockingHit) {
				ALeaf* leaf = Cast<ALeaf>(RV_Hit.GetActor());
				if (leaf) {
					hits++;
				}

			}
		}
	}

	return hits / (numberRays*numberRays);

}

float ATree::calculateCost()
{
	float cost(0);
	for (ABranch* b : branches) {
		cost += b->calculateCost();
	}
	return cost;
}

void ATree::mutate() {
	float f = random.FRand();

	if (f < spawnMutationChance) {
		spawnRandomBranch();
	}

	f = random.FRand();

	if (f < removeMutationChance && branches.Num() > 0) {
		int32 index = random.GetUnsignedInt() % branches.Num();
		ABranch* b = branches[index];
		b->annihilate();
		branches.RemoveAt(index);
	}


	for (ABranch* b : branches) {
		b->mutate();
	}

	currentValue = (calculateHits() * hitRewardMultiplier) - calculateCost();

}

void ATree::spawnRandomBranch() {
	FVector target = GetActorLocation();
	target.Z += random.FRand() * 300;

	FVector beamOrigin = target;

	float distX = (random.FRand() - .5) * 500;
	float distY = (random.FRand() - .5) * 500;
	beamOrigin.X += distX;
	beamOrigin.Y += distY;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	if (debugLine) {
		DrawDebugLine(
			GetWorld(),
			beamOrigin,
			target,
			FColor(255, 0, 0),
			true, -1, 0,
			1
			);
	}
	
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		beamOrigin,    //start
		target, //end
		ECC_Camera, //collision channel
		RV_TraceParams
		);

	if (RV_Hit.bBlockingHit) {
		ABranch* spawnedBranch = (ABranch*)GetWorld()->SpawnActor(Branch_BP);

		FVector Direction = target - beamOrigin;
		FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();

		spawnedBranch->SetActorLocation(RV_Hit.ImpactPoint);
		spawnedBranch->SetActorRotation(Rot);



		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Spawned Branch");
		branches.Add(spawnedBranch);


	}

}

ATree* ATree::duplicate(FVector location) {
	ATree* spawnedTree = (ATree*)GetWorld()->SpawnActor(Tree_BP);

	spawnedTree->SetActorLocation(location);

	spawnedTree->currentValue = currentValue;

	for (ABranch* b : branches) {
		spawnedTree->addBranch(b->duplicate(GetActorLocation(), location));
	}

	return spawnedTree;

}

void ATree::addBranch(ABranch* b) {
	branches.Add(b);
}

void ATree::annihilate() {
	for (ABranch* b : branches) {
		b->annihilate();
	}

	Destroy();
}