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


// Dome-like sunshine
float ATree::hemisphereHits() {

	FVector origin = GetActorLocation();
	origin.Z += 300;
	//startLocation.Z += zDist + 100;

	int hits = 0;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	int n = int(numberRays);

	float goldenAngle = PI * (3.0 - sqrt(5));
	float theta = 0.0;
	float scale = 1000.0;
	float dz = (2.0 / numberRays); // 0.02
	float z = (1.0 - dz / 2.0)*scale; //990
	float r = 0.0;

	// 

	FVector test = origin;// FVector(-1000.0, -5000.0, 2000.0);
	FVector coordinate;

	for (int k = 0; k < n - 1; ++k) {


		r = sqrt((scale*scale) - z*z);
		coordinate = FVector(test.X + cos(theta)*r, test.Y + sin(theta)*r, test.Z + z);
		z -= (dz*scale);
		theta += goldenAngle;
		if (coordinate.Z > test.Z) {
			//call GetWorld() from within an actor extending class
			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,        //result
				coordinate,    //start
				origin, //end
				ECC_Pawn, //collision channel
				RV_TraceParams
			);

			if (debugLine) {
				DrawDebugLine(
					GetWorld(),
					coordinate,
					test,
					FColor(0, 0, 255),
					true, -1, 0,
					5
				);
			}
		}

		if (RV_Hit.bBlockingHit) {
			ALeaf* leaf = Cast<ALeaf>(RV_Hit.GetActor());
			if (leaf) {
				hits++;
			}

		}



	}

	return hits / numberRays;
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
	
	ALeaf* spawnedLeaf = GetWorld()->SpawnActor<ALeaf>(Leaf_BP, location, FRotator(random.FRandRange(-90, 90), random.FRandRange(-90, 90), random.FRandRange(-90, 90)));
		
	spawnedLeaf->AttachRootComponentToActor(branches[index], NAME_None, EAttachLocation::KeepWorldPosition);
	spawnedLeaf->attachedToIndex = index;
	spawnedLeaf->branchOffset = branchOffset;
	spawnedLeaf->offsetVector = offset;
		
		
	leafs.Add(spawnedLeaf);
}

void ATree::mutate() {

	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			FTransform t = GetRandomPosition();
			FVector prev = b->GetActorLocation();
			b->displace(t.GetLocation(), t.Rotator());

		}
	}

	int count1 = 0;
	int count2 = 0;
	for (ALeaf* l : leafs) {
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			count1++;
		}
		if (random.FRand() < leafChangeBranchChance) {
			count2++;
		}
	}

	// change leaf position slightly
	for (int i = 0; i < count1; ++i) {
		int index = random.RandRange(0, leafs.Num() - 1);
		float oldOffset = leafs[index]->branchOffset;
		float newOffset = oldOffset + (random.FRand() / 2) - 1 / 4;
		newOffset = newOffset > 1 ? 1 : newOffset;
		newOffset = newOffset < 0 ? 0 : newOffset;
		leafs[index]->branchOffset = newOffset;

		FVector oldVectorOffset = leafs[index]->offsetVector;
		FVector newVectorOffset = oldVectorOffset + FVector(random.FRand()*5 - 2.5, random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5);
		newVectorOffset.X = newVectorOffset.X > 20 ? 20 : newVectorOffset.X;
		newVectorOffset.X = newVectorOffset.X < -20 ? -20 : newVectorOffset.X;
		newVectorOffset.Y = newVectorOffset.Y > 20 ? 20 : newVectorOffset.Y;
		newVectorOffset.Y = newVectorOffset.Y < -20 ? -20 : newVectorOffset.Y;
		newVectorOffset.Z = newVectorOffset.Z > 10 ? 10 : newVectorOffset.Z;
		newVectorOffset.Z = newVectorOffset.Z < -10 ? -10 : newVectorOffset.Z;
		//leafs[index]->offsetVector += (random.FRand() / 2) - 1 / 4;

		leafs[index]->offsetVector = newVectorOffset;
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(newOffset) + newVectorOffset);
	}

	for (int i = 0; i < count2; ++i) {
		int index = random.RandRange(0, leafs.Num() - 1);
		leafs[index]->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(leafs[index]->branchOffset) + leafs[index]->offsetVector);
		leafs[index]->AttachRootComponentToActor(branches[leafs[index]->attachedToIndex], NAME_None, EAttachLocation::KeepWorldPosition);
	}

	for (ALeaf* l : leafs) {
		l->mutate();
	}

	currentValue = hemisphereHits();

}

FTransform ATree::GetRandomPosition() {
	
	FVector target = GetActorLocation();
	target.Z += random.FRand() * 400 + 150;

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

void ATree::duplicate(ATree* otherTree, FVector location) {

	for (int i = 0; i < branches.Num(); ++i) {
		FVector diff = branches[i]->GetActorLocation() - GetActorLocation();
		FVector newLocation = location + diff;
		ABranch* spawnedBranch = otherTree->branches[i];
		spawnedBranch->SetActorLocation(newLocation);
		spawnedBranch->SetActorRotation(branches[i]->GetActorRotation());
	}

	for (int i = 0; i < leafs.Num(); ++i) {
		ALeaf* l = leafs[i];
		FVector newLocation = otherTree->branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector;
		ALeaf* spawnedLeaf = otherTree->leafs[i];
		spawnedLeaf->SetActorLocation(newLocation);
		spawnedLeaf->SetActorRotation(l->GetActorRotation());
		l->duplicate(spawnedLeaf);
	}

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