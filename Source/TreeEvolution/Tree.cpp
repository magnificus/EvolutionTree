// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Tree.h"
#include "Leaf.h"

using namespace std;


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
	switch (mode) {
	case MODE_STRAIGHT: return calculateHitsStraightAbove();
	case MODE_HEMISPHERE: return hemisphereHits();
	}
	return 0;
}

struct line {
	FVector start;
	FVector end;
};

float ATree::calculateHitsStraightAbove() {

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


// Dome-like sunshine
float ATree::hemisphereHits() {

	FVector origin = GetActorLocation();
	origin.Z += 230;
	//startLocation.Z += zDist + 100;

	float hits = 0.0;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	int n = int(numberRays);

	float goldenAngle = PI * (3.0 - sqrt(5));
	float theta = 0.0;
	float scale = 600;
	float dz = (2.0 / numberRays); // 0.02
	float z = (1.0 - dz / 2.0)*scale; //990
	float r = 0.0;


	FVector test = origin;// FVector(-1000.0, -5000.0, 2000.0);
	FVector coordinate;

	TArray<FVector> list;

	for (int k = 0; k < n - 1; ++k) {


		r = sqrt((scale*scale) - z*z);
		coordinate = FVector(test.X + cos(theta)*r, test.Y + sin(theta)*r, test.Z + z);
		z -= (dz*scale);
		theta += goldenAngle;
		if (coordinate.Z > 0.0) {
			list.Add(coordinate);
		}
	}


	for (int a = 0; a < list.Num(); ++a) {
		for (int b = 0; b < list.Num(); ++b) {
			if (a != b) {
				FVector from = list[a];
				FVector to = list[b];

				GetWorld()->LineTraceSingleByChannel(
					RV_Hit,        //result
					from,    //start
					to, //end
					ECC_Pawn, //collision channel
					RV_TraceParams
				);

				if (debugLine) {

					DrawDebugLine(
						GetWorld(),
						from,
						to,
						FColor(0, 0, 255),
						true, -1, 0,
						1
					);
				}


				if (RV_Hit.bBlockingHit) {
					ALeaf* leaf = Cast<ALeaf>(RV_Hit.GetActor());
					if (leaf) {
						FVector dir = leaf->GetActorLocation() - from;
						float  value = dir.Size();
						hits += 1.0; //+ (1.0 / value)*scale;
					}
				}

			}


		}



	}

	return hits / (numberRays*numberRays);

}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
void ATree::init(int numB, int numL) {
=======
void ATree::init() {
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
void ATree::init() {
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
void ATree::init() {
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
void ATree::init() {
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	for (ABranch* b : branches) {
		b->Destroy();
	}

	branches.Empty();
	for (ALeaf* l : leafs) {
		l->Destroy();
	}
	leafs.Empty();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	for (int i = 0; i < numB; ++i) {
		initRandomBranch();
	}

	for (int i = 0; i < numL; ++i) {
=======
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	for (int i = 0; i < numBranches; ++i) {
		initRandomBranch();
	}

	for (int i = 0; i < numLeafs; ++i) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
		initRandomLeaf();
	}
}

void ATree::initRandomBranch() {
	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, FVector(), FRotator(), FActorSpawnParameters());
	branchDependencies.Add(spawnedBranch, TArray<ABranch*>());
	leafDependencies.Add(spawnedBranch, TArray<ALeaf*>());
	branches.Add(spawnedBranch);


	GetRandomPositionFor(spawnedBranch);


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

	spawnedLeaf->attachedToIndex = index;
	spawnedLeaf->branchOffset = branchOffset;
	spawnedLeaf->offsetVector = offset;


	leafs.Add(spawnedLeaf);
	leafDependencies[branches[index]].Add(spawnedLeaf);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
}

void ATree::displaceBranch(ABranch* b) {
	if (b->placedOn != NOT_PLACED) {
		branchDependencies[branches[b->placedOn]].Remove(b);
		b->placedOn = NOT_PLACED;
	}

	GetRandomPositionFor(b);
	cascadePositionUpdate(b);
}

void ATree::cascadePositionUpdate(ABranch* b) {
	for (ABranch* newB : branchDependencies[b]) {
		newB->SetActorLocation(b->getEnd());
		if (newB->overlapsProps()) {
		//	// new position was not good, overlapped with other actors, so displace the branch
			displaceBranch(newB);
		} else{
			cascadePositionUpdate(newB);
		}
	}
	for (ALeaf* l : leafDependencies[b]) {
		l->updateLocation(b->getPositionOnBranch(l->branchOffset));
	}

}

bool ATree::selfInChain(ABranch* self, ABranch* current) {
	// can cause infinite loop if branchdependencies is circularly arranged
	if (current == self) {
		return true;
	}
	if (current->placedOn != NOT_PLACED) {
		return selfInChain(self, branches[current->placedOn]);
	}

	return false;
}

=======
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
}

void ATree::displaceBranch(ABranch* b) {
	if (b->placedOn != NOT_PLACED) {
		branchDependencies[branches[b->placedOn]].Remove(b);
	}
	b->placedOn = NOT_PLACED;



	//for (ABranch* extendingB : branchDependencies[b]) {
	//	displaceBranch(extendingB);
	//}

	GetRandomPositionFor(b);

	cascadePositionUpdate(b);



	//allowedToExtend.Add(b);
}

<<<<<<< HEAD
=======
}

void ATree::displaceBranch(ABranch* b) {
	if (b->placedOn != NOT_PLACED) {
		branchDependencies[branches[b->placedOn]].Remove(b);
	}
	b->placedOn = NOT_PLACED;



	//for (ABranch* extendingB : branchDependencies[b]) {
	//	displaceBranch(extendingB);
	//}

	GetRandomPositionFor(b);

	cascadePositionUpdate(b);



	//allowedToExtend.Add(b);
}

>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
void ATree::cascadePositionUpdate(ABranch* b) {
	for (ABranch* newB : branchDependencies[b]) {
		newB->SetActorLocation(b->getEnd());
		cascadePositionUpdate(newB);
	}
	for (ALeaf* l : leafDependencies[b]) {
		l->updateLocation(b->getPositionOnBranch(l->branchOffset));
	}

}

bool ATree::selfInChain(ABranch* self, ABranch* current) {
	// can cause infinite loop if branchdependencies is circularly arranged
	if (current == self) {
		return true;
	}
	if (current->placedOn != NOT_PLACED) {
		return selfInChain(self, branches[current->placedOn]);
	}

	return false;
}

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
}

void ATree::displaceBranch(ABranch* b) {
	if (b->placedOn != NOT_PLACED) {
		branchDependencies[branches[b->placedOn]].Remove(b);
	}
	b->placedOn = NOT_PLACED;



	//for (ABranch* extendingB : branchDependencies[b]) {
	//	displaceBranch(extendingB);
	//}

	GetRandomPositionFor(b);

	cascadePositionUpdate(b);



	//allowedToExtend.Add(b);
}

void ATree::cascadePositionUpdate(ABranch* b) {
	for (ABranch* newB : branchDependencies[b]) {
		newB->SetActorLocation(b->getEnd());
		cascadePositionUpdate(newB);
	}
	for (ALeaf* l : leafDependencies[b]) {
		l->updateLocation(b->getPositionOnBranch(l->branchOffset));
	}

}

bool ATree::selfInChain(ABranch* self, ABranch* current) {
	// can cause infinite loop if branchdependencies is circularly arranged
	if (current == self) {
		return true;
	}
	if (current->placedOn != NOT_PLACED) {
		return selfInChain(self, branches[current->placedOn]);
	}

	return false;
}

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
void ATree::mutate(bool reCalc) {

	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			displaceBranch(b);
		}
		if (b->mutate())
			cascadePositionUpdate(b);
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
		FVector newVectorOffset = oldVectorOffset + FVector(random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5);
		newVectorOffset.X = newVectorOffset.X > 20 ? 20 : newVectorOffset.X;
		newVectorOffset.X = newVectorOffset.X < -20 ? -20 : newVectorOffset.X;
		newVectorOffset.Y = newVectorOffset.Y > 20 ? 20 : newVectorOffset.Y;
		newVectorOffset.Y = newVectorOffset.Y < -20 ? -20 : newVectorOffset.Y;
		newVectorOffset.Z = newVectorOffset.Z > 10 ? 10 : newVectorOffset.Z;
		newVectorOffset.Z = newVectorOffset.Z < -10 ? -10 : newVectorOffset.Z;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
		//leafs[index]->offsetVector += (random.FRand() / 2) - 1 / 4;
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
		//leafs[index]->offsetVector += (random.FRand() / 2) - 1 / 4;
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
		//leafs[index]->offsetVector += (random.FRand() / 2) - 1 / 4;
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9

		leafs[index]->offsetVector = newVectorOffset;
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(newOffset) + newVectorOffset);
	}

	for (int i = 0; i < count2; ++i) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
		// change branch
		int index = random.RandRange(0, leafs.Num() - 1);
<<<<<<< HEAD
		ALeaf* l = leafs[index];
		leafDependencies[branches[l->attachedToIndex]].Remove(l);
		l->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafDependencies[branches[l->attachedToIndex]].Add(l);
		l->SetActorLocation(branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector);
=======
		leafs[index]->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(leafs[index]->branchOffset) + leafs[index]->offsetVector);
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
		int index = random.RandRange(0, leafs.Num() - 1);
		leafs[index]->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(leafs[index]->branchOffset) + leafs[index]->offsetVector);
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
		int index = random.RandRange(0, leafs.Num() - 1);
		leafs[index]->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(leafs[index]->branchOffset) + leafs[index]->offsetVector);
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
		int index = random.RandRange(0, leafs.Num() - 1);
		leafs[index]->attachedToIndex = random.RandRange(0, branches.Num() - 1);
		leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(leafs[index]->branchOffset) + leafs[index]->offsetVector);
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	}

	for (ALeaf* l : leafs) {
		l->mutate();
	}

	if (reCalc)
		currentValue = calculateHits();

}

FRotator ATree::getR() {
	return FRotator(random.FRand() * 360, random.FRand() * 360, random.FRand() * 360);
}

void ATree::GetRandomPositionFor(ABranch* b) {

	if (branches.Num() > 0 && random.FRand() > .5) {
		// 50 % chance to spawn on another branch instead of the stem
		int32 index = random.RandRange(0, branches.Num() - 1);
		ABranch* toBuildFrom = branches[index];
		if (selfInChain(b, toBuildFrom)) {
			// abandon
			GetRandomPositionFor(b);
			return;
		}
		branchDependencies[toBuildFrom].Add(b);
		b->placedOn = index;
		FVector pos = toBuildFrom->getEnd();
		b->displace(pos, toBuildFrom->GetActorRotation() - FRotator(random.FRand() * 90 - 45, random.FRand() * 90 - 45, random.FRand() * 90 - 45));
	}
	else {
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
			GetRandomPositionFor(b);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
			return;
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
		}

		b->placedOn = NOT_PLACED;
		b->displace(RV_Hit.Location, getR());
	}

	if (b->overlapsProps()) {
		//abandon
		GetRandomPositionFor(b);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
		return;
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	}


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
	branches.Add(b);
	branchDependencies[b] = TArray<ABranch*>();
}

void ATree::addLeaf(ABranch* b, ALeaf* l) {
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

vector<float> ATree::createChildDNA(ATree* otherParent) {

	vector<float> DNA;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	//int32 index = random.RandRange(0, branches.Num() - 1);
	//vector<int> singleChain = getChain(branches[index]);
	//singleChain.push_back(index);

	for (int i = 0; i < branches.Num(); ++i) {
		ATree* t;
		ABranch* b;
		//if (find(singleChain.begin(), singleChain.end(), i) != singleChain.end()) {
=======
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	for (int i = 0; i < branches.Num(); ++i) {
		ATree* t;
		ABranch* b;
		//if (random.FRand() < .5) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
			t = this;
			b = branches[i];
		//}
		//else {
		//	t = otherParent;
		//	b = otherParent->branches[i];
		//}
		DNA.push_back(b->GetActorLocation().X - t->GetActorLocation().X);
		DNA.push_back(b->GetActorLocation().Y - t->GetActorLocation().Y);
		DNA.push_back(b->GetActorLocation().Z - t->GetActorLocation().Z);
		DNA.push_back(b->GetActorRotation().Pitch);
		DNA.push_back(b->GetActorRotation().Yaw);
		DNA.push_back(b->GetActorRotation().Roll);
		DNA.push_back(b->placedOn);

	}
	for (int i = 0; i < leafs.Num(); ++i) {
		ALeaf* l = (random.FRand() < .5) ? leafs[i] : otherParent->leafs[i];
		DNA.push_back(l->attachedToIndex);
		DNA.push_back(l->branchOffset);
		DNA.push_back(l->offsetVector.X);
		DNA.push_back(l->offsetVector.Y);
		DNA.push_back(l->offsetVector.Z);
		DNA.push_back(l->GetActorRotation().Pitch);
		DNA.push_back(l->GetActorRotation().Yaw);
		DNA.push_back(l->GetActorRotation().Roll);

	}

	return DNA;
}




void ATree::buildFromDNA(vector<float> DNA) {
	for (auto &b : branchDependencies) {
		b.Value.Empty();
	}
	for (auto &l : leafDependencies) {
		l.Value.Empty();
	}

	int currPos = 0;
	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		b->SetActorLocation(FVector(DNA[currPos] + GetActorLocation().X, DNA[currPos + 1] + GetActorLocation().Y, DNA[currPos + 2] + GetActorLocation().Z));
		b->SetActorRotation(FRotator(DNA[currPos + 3], DNA[currPos + 4], DNA[currPos + 5]));
		b->placedOn = DNA[currPos + 6];

		if (b->placedOn != NOT_PLACED) {
			branchDependencies[branches[b->placedOn]].Add(b);

		}


		currPos += 7;
	}
	for (int i = 0; i < leafs.Num(); ++i) {
		ALeaf* l = leafs[i];
		ABranch* b = branches[l->attachedToIndex];
		l->attachedToIndex = DNA[currPos];
		l->branchOffset = DNA[currPos + 1];
		l->offsetVector = FVector(DNA[currPos + 2], DNA[currPos + 3], DNA[currPos + 4]);
		l->SetActorRotation(FRotator(DNA[currPos + 5], DNA[currPos + 6], DNA[currPos + 7]));

		currPos += 8;

		leafDependencies[branches[l->attachedToIndex]].Add(l);
		l->updateLocation(b->getPositionOnBranch(l->branchOffset));
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	}

}

void ATree::checkCollision() {
	for (ABranch* b : branches) {
		if (b->overlapsProps()) {
			displaceBranch(b);
		}
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	}
<<<<<<< HEAD
}

<<<<<<< HEAD
vector<int> ATree::getChain(ABranch* b) {
	if (b->placedOn == NOT_PLACED) {
		return vector<int>();
	} else{
		vector<int> prev = getChain(branches[b->placedOn]);
		prev.push_back(b->placedOn);
		return prev;
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
	}
	
}

void ATree::SetNumBranches(int32 num) {
	init(num, leafs.Num());
}

<<<<<<< HEAD
void ATree::SetNumLeafs(int32 num) {
	init(branches.Num(), num);
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
	}

>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
	}

>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
	}

>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
}

void ATree::checkCollision() {
	for (ABranch* b : branches) {
		if (b->overlapsProps()) {
			displaceBranch(b);
		}
	}
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
=======
>>>>>>> 19c91a8a182e4b300fe1335206e74c12d8c7a1e9
}