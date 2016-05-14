// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Tree.h"
#include "Leaf.h"

using namespace std;

const int RECURSIVE_DEPTH_LIMIT = 100;

// Sets default values
ATree::ATree()
{

	static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	if (BranchFinder.Object != NULL)
		Branch_BP = BranchFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> SunFinder(TEXT("Class'/Game/sunActor.sunActor_C'"));
	if (SunFinder.Object != NULL)
		Sun_BP = SunFinder.Object;

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

void ATree::setMode(int m) {
	mode = m;
}


float ATree::calculateHits() {


	float cost = calculateCost();
	switch (mode) {
		case MODE_STRAIGHT: return calculateHitsStraightAbove() - cost;
		case MODE_MANUAL_DIRECTION: return manualDirectionHits() - cost;
		case MODE_SWEEP: return sweepHits() - cost;
		case MODE_HEMISPHERE: return hemisphereHits() - cost;
	}
	return 0;
}


float ATree::calculateCost() {


	float total = 0;
	for (ABranch* b : branches) {
		total += lengthOfChain(b);
	}
	total *= branchExtensionCost;

	return total;
}

int ATree::lengthOfChain(ABranch* b) {
	int len = 0;
	ABranch* current = b;
	while (current->placedOn != NOT_PLACED) {
		current = branches[current->placedOn];
		++len;
	}
	return len;
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
					FColor(175, 0, 175),
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


void ATree::setAngles(float inTheta, float inPhi) {
	float r = 2000;
	theta = FMath::DegreesToRadians(inTheta);
	phi = FMath::DegreesToRadians(inPhi);
	FVector origin = GetActorLocation();
	origin.Z += 400;
	sunPos = FVector(origin.X + sin(theta) *cos(phi)*r, origin.Y + sin(theta)*sin(phi)*r, origin.Z + cos(theta)*r);
}

void ATree::illustrateSun() {
	if (!sunActor) {
		sunActor = (AActor*)GetWorld()->SpawnActor(Sun_BP);
	}

	sunActor->SetActorLocation(sunPos);
}


float ATree::manualDirectionHits() {


	int size = maxSpread*2;

	//float theta = PI / 6.0; // 0 - pi , latitute where 0 = north pole
	//float phi = 1.5*PI; // 0 - 2pi , longitude coordinates
	FRotator ActorRotation = GetActorRotation();
	ActorRotation.Pitch -= FMath::RadiansToDegrees(theta);
	ActorRotation.Yaw += FMath::RadiansToDegrees(phi);
	SetActorRotation(ActorRotation);

	float offset = size / numberRays;
	FVector  forward = GetActorForwardVector();
	FVector right = GetActorRightVector();
	FVector sunDir = -GetActorUpVector();
	float sunDist = 2800;



	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	float hits = 0.0;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;



	for (int x = -numberRays / 2; x < numberRays / 2; ++x) {

		FVector offsetRight = (right *x*offset);

		for (int y = -numberRays / 2; y < numberRays / 2; ++y) {

			FVector offsetForward = (forward *y*offset);
			FVector planePointDirection = (offsetRight)+(offsetForward);
			FVector planePoint = sunPos + planePointDirection;
			FVector endPoint = planePoint + (sunDir*sunDist);



			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,        //result
				planePoint,    //start
				endPoint, //end
				ECC_Pawn, //collision channel
				RV_TraceParams
			);

			if (debugLine) {

				DrawDebugLine(
					GetWorld(),
					planePoint,
					endPoint,
					FColor(0, 0, 255),
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
	ActorRotation.Pitch += FMath::RadiansToDegrees(theta);
	ActorRotation.Yaw -= FMath::RadiansToDegrees(phi);
	SetActorRotation(ActorRotation);


	return hits / (numberRays*numberRays);
}

float ATree::sweepHits() {


	int size = maxSpread * 2;
	float offset = size / numberRays;
	float sunDist = 2800;
	float hits = 0.0;
	float ang = 90.0;


	FRotator ActorRotation = GetActorRotation();
	setAngles(ang, 0.0);
	float angleDiff = 180.0 / 4.0;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;



	for (int k = 0; k < 5; ++k) {
		ActorRotation.Pitch -= FMath::RadiansToDegrees(theta);
		ActorRotation.Yaw += FMath::RadiansToDegrees(phi);
		SetActorRotation(ActorRotation);

		FVector  forward = GetActorForwardVector();
		FVector right = GetActorRightVector();
		FVector sunDir = -GetActorUpVector();

		for (int x = -numberRays / 2; x < numberRays / 2; ++x) {

			FVector offsetRight = (right *x*offset);

			for (int y = -numberRays / 2; y < numberRays / 2; ++y) {

				FVector offsetForward = (forward *y*offset);
				FVector planePointDirection = (offsetRight)+(offsetForward);
				FVector planePoint = sunPos + planePointDirection;
				FVector endPoint = planePoint + (sunDir*sunDist);



				GetWorld()->LineTraceSingleByChannel(
					RV_Hit,        //result
					planePoint,    //start
					endPoint, //end
					ECC_Pawn, //collision channel
					RV_TraceParams
				);

				if (debugLine) {

					DrawDebugLine(
						GetWorld(),
						planePoint,
						endPoint,
						FColor(255, 165, 10),
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
		ActorRotation.Pitch += FMath::RadiansToDegrees(theta);
		ActorRotation.Yaw -= FMath::RadiansToDegrees(phi);
		SetActorRotation(ActorRotation);

		ang = ang - angleDiff;
		if (ang < 0.0) {
			setAngles(-ang, 180.0);
		}
		else {
			setAngles(ang, 0.0);
		}

	}

	return hits / ((numberRays*numberRays)*5.0);
}


FVector GetForwardVector(FRotator InRot)
{
	return FRotationMatrix(InRot).GetScaledAxis(EAxis::X);
}

FVector GetRightVector(FRotator InRot)
{
	return FRotationMatrix(InRot).GetScaledAxis(EAxis::Y);
}

FVector GetUpVector(FRotator InRot)
{
	return FRotationMatrix(InRot).GetScaledAxis(EAxis::Z);
}


float ATree::hemisphereHits() {


	int size = maxSpread *2.0;
	float offset = size / numberRays;
	float sunDist = 2000;
	float hits = 0.0;
	float ang = 90.0;
	

	FRotator ActorRotation = GetActorRotation();
	setAngles(ang, 0.0);

	int pointsInAngle = 12;
	int pointsInTwist = 7;

	float angleDiff = 180.0 / pointsInAngle;
	float twistDiff = 360.0 / pointsInTwist;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	for (int s = 0; s < pointsInTwist; ++s) {
		for (int k = 0; k <= pointsInAngle; ++k) {

			ang = 90.0 - angleDiff*k;
			if (ang < 0.0) {
				float newTwistAngle = 180.0 + twistDiff*s;
				if (newTwistAngle > 360.0) newTwistAngle -= 360.0;
				setAngles(-ang, newTwistAngle);
			}
			else {
				setAngles(ang, twistDiff*s);
			}
			

			// if either this is the first beam (s == 0) or it's not the beam from above
			if (k != pointsInAngle/2 || s == 0) {
				FRotator r = GetActorRotation();
				r.Pitch -= FMath::RadiansToDegrees(theta);
				r.Yaw += FMath::RadiansToDegrees(phi);

				FVector forward = GetForwardVector(r);
				FVector right = GetRightVector(r);
				FVector sunDir = -GetUpVector(r);

				for (int x = -numberRays / 2; x < numberRays / 2; ++x) {

					FVector offsetRight = (right *x*offset);

					for (int y = -numberRays / 2; y < numberRays / 2; ++y) {

						FVector offsetForward = (forward *y*offset);
						FVector planePointDirection = (offsetRight)+(offsetForward);
						FVector planePoint = sunPos + planePointDirection;
						FVector endPoint = planePoint + (sunDir*sunDist);



						GetWorld()->LineTraceSingleByChannel(
							RV_Hit,        //result
							planePoint,    //start
							endPoint, //end
							ECC_Pawn, //collision channel
							RV_TraceParams
						);
						FColor color = FColor((s==0) ? 255 : 0	, 120, 100);
						if (s == 0) color = FColor(255, 0, 0);
						else if (s == 1) color = FColor(0, 255, 0);
						else color = FColor(140, 140, 140);
						if (debugLine) {

							DrawDebugLine(
								GetWorld(),
								planePoint,
								endPoint,
								color,
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
				r.Pitch += FMath::RadiansToDegrees(theta);
				r.Yaw -= FMath::RadiansToDegrees(phi);
				

			}
		}
	}

	return hits / (numberRays*numberRays)/(pointsInAngle*pointsInTwist);



}




void ATree::init(int numB, int numL) {

	for (ABranch* b : branches) {
		b->Destroy();
	}

	branches.Empty();
	for (ALeaf* l : leafs) {
		l->Destroy();
	}
	leafs.Empty();

	for (int i = 0; i < numB; ++i) {
		initRandomBranch();
	}

	for (int i = 0; i < numL; ++i) {
		initRandomLeaf();
	}
}

void ATree::initRandomBranch() {
	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, FVector(), FRotator(), FActorSpawnParameters());
	branchDependencies.Add(spawnedBranch, TArray<ABranch*>());
	leafDependencies.Add(spawnedBranch, TArray<ALeaf*>());
	
	spawnedBranch->myIndex = branches.Add(spawnedBranch);

	GetRandomPositionFor(spawnedBranch, RECURSIVE_DEPTH_LIMIT);


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
}

void ATree::displaceBranch(ABranch* b) {


	GetRandomPositionFor(b, RECURSIVE_DEPTH_LIMIT);
	cascadePositionUpdate(b, RECURSIVE_DEPTH_LIMIT * 300);
}

void ATree::cascadePositionUpdate(ABranch* b, int limit) {
	if (limit == 0) {
		throw "Recursive depth limit reached, aborting...";
	}
	--limit;

	TArray<ABranch*> toRemove;
	for (ABranch* newB : branchDependencies[b]) {
		if (b == NULL || newB == NULL) {
			continue;
		}
		FVector end = b->getEnd();
		newB->displace(end, FRotator(), GetActorLocation());
		if (newB->overlapsProps()) {

			// new position was not good, overlapped with other actors, so displace the branch
			newB->placedOn = NOT_PLACED;
			toRemove.Add(newB);
			do {
				displaceBranch(newB);
			} while (newB->overlapsProps());
		}
		else {
			cascadePositionUpdate(newB, limit);
		}
	}

	for (ABranch* newB : toRemove) {
		branchDependencies[b].Remove(newB);
	}
	for (ALeaf* l : leafDependencies[b]) {
		l->updateLocation(b->getPositionOnBranch(l->branchOffset));
	}

}


void ATree::mutate(bool reCalc) {

	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			if (b->placedOn != NOT_PLACED) {
				branchDependencies[branches[b->placedOn]].Remove(b);
			}

			displaceBranch(b);
		}
		if (b->mutate())
			cascadePositionUpdate(b, RECURSIVE_DEPTH_LIMIT);
	}

	//int count1 = 0;
	//int count2 = 0;
	for (ALeaf* l : leafs) {
		float f = random.FRand();
		if (random.FRand() < displacementChance) {
			// change leaf position slightly

			int index = random.RandRange(0, leafs.Num() - 1);
			float oldOffset = leafs[index]->branchOffset;
			float newOffset = oldOffset + (random.FRand() / 4) - (1 / 8);
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
			leafs[index]->offsetVector = newVectorOffset;
			leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(newOffset) + newVectorOffset);
		}
		if (random.FRand() < leafChangeBranchChance) {
			// change branch entirely
			int index = random.RandRange(0, leafs.Num() - 1);
			ALeaf* l = leafs[index];
			leafDependencies[branches[l->attachedToIndex]].Remove(l);
			l->attachedToIndex = random.RandRange(0, branches.Num() - 1);
			leafDependencies[branches[l->attachedToIndex]].Add(l);
			l->SetActorLocation(branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector);

		}
	}

	//for (int i = 0; i < count1; ++i) {
	//	int index = random.RandRange(0, leafs.Num() - 1);
	//	float oldOffset = leafs[index]->branchOffset;
	//	float newOffset = oldOffset + (random.FRand() / 4) - (1 / 8);
	//	newOffset = newOffset > 1 ? 1 : newOffset;
	//	newOffset = newOffset < 0 ? 0 : newOffset;
	//	leafs[index]->branchOffset = newOffset;

	//	FVector oldVectorOffset = leafs[index]->offsetVector;
	//	FVector newVectorOffset = oldVectorOffset + FVector(random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5, random.FRand() * 5 - 2.5);
	//	newVectorOffset.X = newVectorOffset.X > 20 ? 20 : newVectorOffset.X;
	//	newVectorOffset.X = newVectorOffset.X < -20 ? -20 : newVectorOffset.X;
	//	newVectorOffset.Y = newVectorOffset.Y > 20 ? 20 : newVectorOffset.Y;
	//	newVectorOffset.Y = newVectorOffset.Y < -20 ? -20 : newVectorOffset.Y;
	//	newVectorOffset.Z = newVectorOffset.Z > 10 ? 10 : newVectorOffset.Z;
	//	newVectorOffset.Z = newVectorOffset.Z < -10 ? -10 : newVectorOffset.Z;
	//	leafs[index]->offsetVector = newVectorOffset;
	//	leafs[index]->SetActorLocation(branches[leafs[index]->attachedToIndex]->getPositionOnBranch(newOffset) + newVectorOffset);
	//}

	//for (int i = 0; i < count2; ++i) {
	//	int index = random.RandRange(0, leafs.Num() - 1);
	//	ALeaf* l = leafs[index];
	//	leafDependencies[branches[l->attachedToIndex]].Remove(l);
	//	l->attachedToIndex = random.RandRange(0, branches.Num() - 1);
	//	leafDependencies[branches[l->attachedToIndex]].Add(l);
	//	l->SetActorLocation(branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector);
	//}

	for (ALeaf* l : leafs) {
		l->mutate();
	}

	if (reCalc)
		currentValue = calculateHits();

}

FRotator ATree::getR() {
	return FRotator(random.FRand() * 360, random.FRand() * 360, random.FRand() * 360);
}



void ATree::GetRandomPositionFor(ABranch* b, int recursiveLimit) {

	--recursiveLimit;

	if (recursiveLimit == 0) {
		// not finding anywhere to place the branch
		throw "No valid positions for branch";
	}

	if (b->myIndex > 0 && random.FRand() > .5) {
		// X % chance to spawn on another branch instead of the stem
		int32 index = random.RandRange(0, b->myIndex-1);
		ABranch* toBuildFrom = branches[index];

		branchDependencies[toBuildFrom].Add(b);
		b->placedOn = index;
		FVector pos = toBuildFrom->getEnd();
		b->displace(pos, toBuildFrom->GetActorRotation() - FRotator(random.FRand() * 150 - 75, random.FRand() * 150 - 75, random.FRand() * 150 - 75), GetActorLocation());
	}
	else {
		// spawn on stem
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
			GetRandomPositionFor(b, recursiveLimit);
			return;
		}

		b->placedOn = NOT_PLACED;
		b->displace(RV_Hit.Location, getR(), GetActorLocation());
	}

	if (b->overlapsProps()) {
		//abandon if the branch overlaps 
		if (b->placedOn != NOT_PLACED) {
			branchDependencies[branches[b->placedOn]].Remove(b);
			b->placedOn = NOT_PLACED;
		}
		GetRandomPositionFor(b, recursiveLimit);
		return;
	}


}

void ATree::duplicate(ATree* otherTree, FVector location) {
	otherTree->buildFromDNA(createChildDNA());
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
		ABranch* b = (random.FRand() < .5) ? (branches[i]) : otherParent->branches[i];
		DNA.push_back(b->treeOffset.X);
		DNA.push_back(b->treeOffset.Y);
		DNA.push_back(b->treeOffset.Z);

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

vector<float> ATree::createChildDNA() {
	vector<float> DNA;
	for (int i = 0; i < branches.Num(); ++i) {
		ABranch* b = branches[i];
		DNA.push_back(b->treeOffset.X);
		DNA.push_back(b->treeOffset.Y);
		DNA.push_back(b->treeOffset.Z);

		DNA.push_back(b->GetActorRotation().Pitch);
		DNA.push_back(b->GetActorRotation().Yaw);
		DNA.push_back(b->GetActorRotation().Roll);
		DNA.push_back(b->placedOn);
	}
	for (int i = 0; i < leafs.Num(); ++i) {
		ALeaf* l = leafs[i];
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

		FRotator rot = FRotator(DNA[currPos + 3], DNA[currPos + 4], DNA[currPos + 5]);
		b->placedOn = DNA[currPos + 6];
		if (b->placedOn != NOT_PLACED) {
			branchDependencies[branches[b->placedOn]].Add(b);
			b->displace(branches[b->placedOn]->getEnd(), rot, GetActorLocation());
			if (b->overlapsProps()) {
				displaceBranch(b);
			}
		}
		else {
			b->displace(FVector(DNA[currPos] + GetActorLocation().X, DNA[currPos + 1] + GetActorLocation().Y, DNA[currPos + 2] + GetActorLocation().Z), rot, GetActorLocation());
		}

		currPos += 7;
	}


	// leafs
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
	}

}

void ATree::checkCollision() {
	for (ABranch* b : branches) {
		if (b->overlapsProps()) {
			if (b->placedOn != NOT_PLACED) {
				branchDependencies[branches[b->placedOn]].Remove(b);
			}
			while (b->overlapsProps()) {
				displaceBranch(b);
			}

		}
	}
}

void ATree::exportTree() {
	// TODO
}

void ATree::hillClimb() {
	for (ABranch* b : branches) {
		float currentBest = calculateHits();
		FRotator pre = b->GetActorRotation();
		FRotator best = pre;

		// PITCH
		testRotation(b, FRotator(5, 0, 0), 5);
		testRotation(b, FRotator(-5, 0, 0), 5);
		// YAW
		testRotation(b, FRotator(0, 5, 0), 5);
		testRotation(b, FRotator(0, -5, 0), 5);
		//ROLL
		testRotation(b, FRotator(0, 0, 5), 5);
		testRotation(b, FRotator(0, 0, -5), 5);

		
	}
	for (ALeaf* l : leafs) {

		// location
		testLocation(l, 0.1);

		// PITCH
		testRotation(l, FRotator(2, 0, 0), 2);
		testRotation(l, FRotator(-2, 0, 0), 2);
		// YAW
		testRotation(l, FRotator(0, 2, 0), 2);
		testRotation(l, FRotator(0, -2, 0), 2);
		//ROLL
		testRotation(l, FRotator(0, 0, 2), 2);
		testRotation(l, FRotator(0, 0, -2), 2);
	}

	currentValue = calculateHits();
}

void ATree::testRotation(ABranch* b, FRotator r, int stepSize) {
	float currentBest = calculateHits();
	float latestHits = currentBest;
	int count = 0;
	int bestFirstMeasuredAt = 0;
	while (currentBest == latestHits && count < 360 / stepSize) {
		b->AddActorLocalRotation(r);
		cascadePositionUpdate(b, RECURSIVE_DEPTH_LIMIT);
		latestHits = calculateHits();
		if (latestHits > currentBest) {
			bestFirstMeasuredAt = count;
			currentBest = latestHits;
		}
		count++;
	}
	b->AddActorLocalRotation(-(((count - bestFirstMeasuredAt)/2) * r));
	cascadePositionUpdate(b, RECURSIVE_DEPTH_LIMIT);
}

void ATree::testRotation(ALeaf* l, FRotator r, int stepSize) {
	float currentBest = calculateHits();
	float latestHits = currentBest;
	int bestFirstMeasuredAt = 0;
	int count = 0;
	while (currentBest == latestHits && count < 360 / stepSize) {
		l->AddActorLocalRotation(r);
		latestHits = calculateHits();
		if (latestHits > currentBest) {
			bestFirstMeasuredAt = count;
			currentBest = latestHits;
		}
		count++;
	}
	l->AddActorLocalRotation(-(((count - bestFirstMeasuredAt)/2) * r));
}

void ATree::testLocation(ALeaf* l, int stepSize) {
	float currentBest = 0;
	float bestDist = 0;

	for (float dist = 0; dist < 1; dist += stepSize) {
		l->branchOffset = dist;
		l->SetActorLocation(branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector);
		float currRes = calculateHits();
		if (currRes > currentBest) {
			bestDist = dist;
			currentBest = currRes;
		}
	}
	l->branchOffset = bestDist;
	l->SetActorLocation(branches[l->attachedToIndex]->getPositionOnBranch(l->branchOffset) + l->offsetVector);

}
