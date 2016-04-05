// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Tree.h"
#include "Leaf.h"


// Sets default values
ATree::ATree()
{

	static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	if (BranchFinder.Object != NULL)
		Branch_BP = BranchFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;

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
	startLocation.Z += zDist + 100;

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
					hits ++;
				}

			}
		}
	}

	return hits / (numberRays*numberRays);

}

void ATree::init() {
	for (int i = 0; i < numBranches; ++i) {
		initRandomBranch();
		
	}
	for (int i = 0; i < numLeafs; ++i) {
		
		initRandomLeaf();
	}
}

void ATree::initRandomBranch() {
	FTransform t = GetRandomPosition();
	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, t.GetLocation(), t.GetRotation().Rotator(), FActorSpawnParameters());
	//spawnedBranch->SetActorRelativeScale3D(FVector(1, 1, random.FRand() * 3));
	spawnedBranch->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
	branches.Add(spawnedBranch);
}

void ATree::initRandomLeaf() {
	if (branches.Num() == 0) {
		return;
	}
	int32 index = random.RandRange(0, branches.Num() - 1);
	FVector offset = FVector(random.RandRange(-10, 10), random.RandRange(-10, 10), random.RandRange(10, 10));
	float branchOffset = random.FRand();
	FVector loc = branches[index]->getPositionOnBranch(random.FRand());

	
	FVector location = loc + offset;
	//location()
	
	ALeaf* spawnedLeaf = GetWorld()->SpawnActor<ALeaf>(Leaf_BP, location, FRotator(random.FRandRange(-90, 90), random.FRandRange(-90, 90), random.FRandRange(-90, 90)));
		
	spawnedLeaf->AttachRootComponentToActor(branches[index], NAME_None, EAttachLocation::KeepWorldPosition);
	spawnedLeaf->attachedToIndex = index;
	spawnedLeaf->branchOffset = branchOffset;
	spawnedLeaf->offsetVector = offset;
		
		
	leafs.Add(spawnedLeaf);
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
	

	//if (f < removeMutationChance && branches.Num() > 0) {
	//	int32 index = random.GetUnsignedInt() % branches.Num();
	//	ABranch* b = branches[index];
	//	b->annihilate();
	//	branches.RemoveAt(index);
	//}

	//f = random.FRand();

	//if (f < spawnMutationChance && currentBranches < maxBranches) {
	//	FTransform t = GetRandomPosition();
	//	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, t.GetLocation(), t.GetRotation().Rotator(), FActorSpawnParameters());
	//	//spawnedBranch->SetActorRelativeScale3D(FVector(1, 1, random.FRand() * 3));
	//	spawnedBranch->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
	//	branches.Add(spawnedBranch);
	//}

	

	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			FTransform t = GetRandomPosition();
			FVector prev = b->GetActorLocation();
			b->displace(t.GetLocation(), t.Rotator());

		}
	}

	int count = 0;
	for (ALeaf* l : leafs) {
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			count++;

		}
	}
	for (int i = 0; i < count; i++) {
		int index = random.RandRange(0, leafs.Num() - 1);
		float oldOffset = leafs[index]->branchOffset;
		float newOffset = oldOffset + (random.FRand() / 2) - 1 / 4;
		newOffset = newOffset > 1 ? 1 : newOffset;
		newOffset = newOffset < 0 ? 0 : newOffset;
		leafs[index]->branchOffset = newOffset;
		//leafs[index]->offsetVector += (random.FRand() / 2) - 1 / 4;

		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(newOffset));
	/*	leafs[index]->Destroy();
		leafs.RemoveAt(index);
		initRandomLeaf();*/
	}
	

	//for (ABranch* b : branches) {
	//	b->mutate();
	//}

	for (ALeaf* l : leafs) {
		l->mutate();
	}

	currentValue = calculateHits();

}

FTransform ATree::GetRandomPosition() {
	
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

	ATree* tree = Cast<ATree>(RV_Hit.GetActor());
	if (!tree) {
		return GetRandomPosition();
	}
	FVector Direction = target - beamOrigin;
	FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();
	FTransform t(FQuat(Rot), RV_Hit.Location,  GetActorScale());
	return t;


	

}

ATree* ATree::duplicate(ATree* spawnedTree, FVector location, bool hidden) {

	for (ABranch* b : branches) {
		FVector diff = b->GetActorLocation() - GetActorLocation();
		FVector newLocation = location + diff;
		ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, newLocation, b->GetActorRotation());
	//	b->duplicate(spawnedBranch, GetActorLocation(), location);
		spawnedTree->addBranch(spawnedBranch);
	}

	for (ALeaf* l : leafs) {
		FVector diff = l->GetActorLocation() - GetActorLocation();
		FVector newLocation = location + diff;
		ALeaf* spawnedLeaf = GetWorld()->SpawnActor<ALeaf>(Leaf_BP, newLocation, l->GetActorRotation());
		l->duplicate(spawnedLeaf);
		spawnedTree->addLeaf(branches[l->attachedToIndex], spawnedLeaf);
	}
	return spawnedTree;

}

void ATree::addBranch(ABranch* b) {
	b->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
	branches.Add(b);
}

void ATree::addLeaf(ABranch* b, ALeaf* l) {
	l->AttachRootComponentToActor(b, NAME_None, EAttachLocation::KeepWorldPosition);
	leafs.Add(l);
}

void ATree::annihilate() {
	for (ABranch* b : branches) {
		b->Destroy();
	}
	for (ALeaf* l : leafs) {
		l->Destroy();
	}

	Destroy();
}