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

	static ConstructorHelpers::FObjectFinder<UClass> CompositeBranchFinder(TEXT("Class'/Game/CompositeBranch.CompositeBranch_C'"));
	if (CompositeBranchFinder.Object != NULL)
		CompositeBranch_BP = CompositeBranchFinder.Object;
	

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



	//for (int i = 0; i < list.Num() / 2; ++i) {


	// //for (int j = list.Num() / 2; j < list.Num(); ++j) {

	//  FVector from = list[i];
	//  FVector to = list[list.Num()-1 -i];

	//  GetWorld()->LineTraceSingleByChannel(
	//   RV_Hit,        //result
	//   from,    //start
	//   to, //end
	//   ECC_Pawn, //collision channel
	//   RV_TraceParams
	//  );

	//  if (debugLine) {

	//    DrawDebugLine(
	//     GetWorld(),
	//     from,
	//     to,
	//     FColor(0, 0, 255),
	//     true, -1, 0,
	//     3
	//    );
	//  }



	//}




	//for (ALeaf* leaf : leafs) {


	// for (FVector v : list) {

	//  FVector leafPos = leaf->GetActorLocation();
	//  FVector dir = leafPos - v;
	//  float dot = FVector::DotProduct(leaf->GetActorForwardVector(), dir);

	//  if ( dot > 0.0) {
	//    
	//   //call GetWorld() from within an actor extending class
	//      GetWorld()->LineTraceSingleByChannel(
	//       RV_Hit,        //result
	//       v,    //start
	//       leafPos, //end
	//       ECC_Pawn, //collision channel
	//       RV_TraceParams
	//      );

	//      if (debugLine) {
	//       float  value = dir.Size();

	//       if (value < 500) {
	//        DrawDebugLine(
	//         GetWorld(),
	//         v,
	//         leafPos,
	//         FColor(0, 0, 255),
	//         true, 2, 0,
	//         2
	//        );
	//       }

	//       /*else {
	//        DrawDebugLine(
	//         GetWorld(),
	//         v,
	//         leafPos,
	//         FColor(255, 0, 0),
	//         true, -1, 0,
	//         2
	//        );
	//       }*/
	//      }
	//   

	//      if (RV_Hit.bBlockingHit) {
	//       ALeaf* leaf = Cast<ALeaf>(RV_Hit.GetActor());
	//       if (leaf) {
	//        float  value = dir.Size();
	//        hits += 1.0 + (1.0/value)*800.0;
	//        /*if (value < 500) {
	//         hits += 1.0 * 0.1*(600.0 - value);
	//        }
	//        else {
	//         hits++;
	//        }*/
	//       }
	//      }
	//  }
	// }
	//}

	return hits / (numberRays*numberRays);

}

void ATree::init() {
	for (ABranch* b : branches) {
		b->Destroy();
	}

	branches.Empty();
	for (ALeaf* l : leafs) {
		l->Destroy();
	}
	leafs.Empty();
	for (int i = 0; i < numBranches; ++i) {
		initRandomBranch(1);
	}

	for (int i = 0; i < numLeafs; ++i) {
		initRandomLeaf();
	}
}

void ATree::initRandomBranch(float scale) {
	FTransform t = GetRandomPosition();
	//t.SetScale3D(FVector(scale, scale, scale));
	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, t.GetLocation(), t.GetRotation().Rotator(), FActorSpawnParameters());
	spawnedBranch->SetActorScale3D(FVector(scale, scale, scale));
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

void ATree::mutate(bool reCalc) {

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
		FVector newVectorOffset = oldVectorOffset + FVector(random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5);
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

	if (reCalc)
		currentValue = calculateHits();

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
	FTransform t(FQuat(Rot), RV_Hit.Location, GetActorScale());
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

vector<float> ATree::createChildDNA(ATree* otherParent) {

	vector<float> DNA;
	for (int i = 0; i < branches.Num(); ++i) {
		ATree* t;
		ABranch* b;
		if (random.FRand() < .5) {
			t = this;
			b = branches[i];
		}
		else {
			t = otherParent;
			b = otherParent->branches[i];
		}
		DNA.push_back(b->GetActorLocation().X - t->GetActorLocation().X);
		DNA.push_back(b->GetActorLocation().Y - t->GetActorLocation().Y);
		DNA.push_back(b->GetActorLocation().Z - t->GetActorLocation().Z);
		DNA.push_back(b->GetActorRotation().Pitch);
		DNA.push_back(b->GetActorRotation().Yaw);
		DNA.push_back(b->GetActorRotation().Roll);

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
	int currPos = 0;
	for (int i = 0; i < branches.Num(); ++i) {
		branches[i]->SetActorLocation(FVector(DNA[currPos] + GetActorLocation().X, DNA[currPos + 1] + GetActorLocation().Y, DNA[currPos + 2] + GetActorLocation().Z));
		branches[i]->SetActorRotation(FRotator(DNA[currPos + 3], DNA[currPos + 4], DNA[currPos + 5]));
		currPos += 6;
	}
	for (int i = 0; i < leafs.Num(); ++i) {
		leafs[i]->attachedToIndex = DNA[currPos];
		leafs[i]->branchOffset = DNA[currPos + 1];
		leafs[i]->offsetVector = FVector(DNA[currPos + 2], DNA[currPos + 3], DNA[currPos + 4]);
		leafs[i]->SetActorRotation(FRotator(DNA[currPos + 5], DNA[currPos + 6], DNA[currPos + 7]));

		currPos += 8;

		leafs[i]->updateLocation(branches[leafs[i]->attachedToIndex]->getPositionOnBranch(leafs[i]->branchOffset));
	}

}