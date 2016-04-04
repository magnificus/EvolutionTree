// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Branch.h"


// Sets default values
ABranch::ABranch()
{


	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	//if (BranchFinder.Object != NULL)
	//	Branch_BP = BranchFinder.Object;

	static ConstructorHelpers::FObjectFinder<UClass> LeafFinder(TEXT("Class'/Game/LeafBP.LeafBP_C'"));
	if (LeafFinder.Object != NULL)
		Leaf_BP = LeafFinder.Object;


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

void ABranch::mutate(int32 &currentBranches, int32 maxBranches, int32 &currentLeafs, int32 maxLeafs)
{

	// destruction branch

	//if (random.FRand() < branchMutationChance && branches.Num() > 0) {
	//	int32 index = random.GetUnsignedInt() % branches.Num();
	//	ABranch* b = branches[index];
	//	b->annihilate();
	//	branches.RemoveAt(index);
	//	currentBranches--;
	//}

	// destruction leaf

	if (random.FRand() < leafMutationChance && leafs.Num() > 0) {
		int32 index = random.GetUnsignedInt() % leafs.Num();
		ALeaf* l = leafs[index];
		l->Destroy();
		leafs.RemoveAt(index);
		currentLeafs--;
	}


	// creation leaf

	if (random.FRand() < leafMutationChance && currentLeafs < maxLeafs) {
		spawnRandomLeaf();
		currentLeafs++;
	}
	


	// propagation

	for (ALeaf* l : leafs) {
		l->mutate();
	}

}

void ABranch::spawnRandomLeaf()
{

	TArray<UPrimitiveComponent*> comps;
	this->GetComponents(comps);
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

	FVector location = begin + (end - begin)*random.FRand() + FVector(random.RandRange(-10,10), random.RandRange(-10, 10), random.RandRange(-10, 10));
	//FRotator f(random.FRand() * 360, random.FRand() * 360, random.FRand() * 360);

	//FTransform t = getRandomPositionOnBranch();

	ALeaf* spawnedLeaf = (ALeaf*)GetWorld()->SpawnActor<ALeaf>(Leaf_BP, location, FRotator(random.FRandRange(-90, 90), random.FRandRange(-90, 90), random.FRandRange(-90, 90)));
	
	spawnedLeaf->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
	
	
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Spawned Leaf");
	leafs.Add(spawnedLeaf);


}

void ABranch::annihilate() {
	//for (ABranch* b : branches) {
	//	b->annihilate();
	//}
	for (ALeaf* l : leafs) {
		l->Destroy();
	}
	Destroy();
}

FTransform ABranch::getRandomPositionOnBranch() {

	// beginning and end of branch...
	
	
	FVector pos = begin + (random.FRand() * (end - begin));

	FVector beamStart = pos;
	float distX = (random.FRand() - .5) * 200;
	float distY = (random.FRand() - .5) * 200;
	float distZ = (random.FRand() - .5) * 200;

	beamStart.X += distX;
	beamStart.Y += distY;
	beamStart.Z += distZ;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;


	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		beamStart,    //start
		pos, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
		);

	FVector Direction = pos - beamStart;
	FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();
	FTransform t;
	t.SetLocation(RV_Hit.ImpactPoint);
	t.SetRotation(FQuat(Rot));
	return t;

}

float ABranch::calculateCost() {
	float totalCost(0);

	for (ALeaf* l : leafs) {
		totalCost += l->cost;
	}
	totalCost += cost;

	return totalCost;
}


void ABranch::addLeaf(ALeaf * l)
{
	leafs.Add(l);
	l->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
}

ABranch* ABranch::duplicate(ABranch* spawnedBranch, FVector originalLocation, FVector newLocation, bool hidden) {

	for (ALeaf* l : leafs) {
		FVector leafDiff = l->GetActorLocation() - originalLocation;
		FVector leafLocation = newLocation + leafDiff;
		

		ALeaf* spawnedLeaf = GetWorld()->SpawnActor<ALeaf>(Leaf_BP, leafLocation, l->GetActorRotation());
		spawnedLeaf->SetActorHiddenInGame(hidden);
		
		spawnedBranch->addLeaf(spawnedLeaf);
	}


	return spawnedBranch;
}

void ABranch::displace(FVector loc, FRotator rot) {

	SetActorLocation(loc);
	SetActorRotation(rot);
}