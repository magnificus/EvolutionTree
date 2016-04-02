// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeEvolution.h"
#include "Branch.h"


// Sets default values
ABranch::ABranch()
{


	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UClass> BranchFinder(TEXT("Class'/Game/BranchBP.BranchBP_C'"));
	if (BranchFinder.Object != NULL)
		Branch_BP = BranchFinder.Object;

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

void ABranch::mutate()
{
	// creation branch
	if (branches.Num() < 1 && random.FRand() < branchMutationChance) {
		spawnRandomBranch();
	}


	// destruction branch

	if (random.FRand() < branchMutationChance && branches.Num() > 0) {
		int32 index = random.GetUnsignedInt() % branches.Num();
		ABranch* b = branches[index];
		b->annihilate();
		branches.RemoveAt(index);
	}


	// creation leaf

	if (random.FRand() < leafMutationChance) {
		spawnRandomLeaf();
	}

	// destruction leaf

	if (random.FRand() < leafMutationChance && leafs.Num() > 0) {
		int32 index = random.GetUnsignedInt() % leafs.Num();
		ALeaf* l = leafs[index];
		l->Destroy();
		leafs.RemoveAt(index);
	}


	// propagation

	for (ABranch* a : branches) {
		a->mutate();
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

	//FTransform trans = getRandomPositionOnBranch();
	ALeaf* spawnedLeaf = (ALeaf*)GetWorld()->SpawnActor(Leaf_BP);
	FVector location = begin + (end - begin)*random.FRand();
	spawnedLeaf->SetActorLocation(location);
	FRotator f(random.FRand() * 360, random.FRand() * 360, random.FRand() * 360);
	spawnedLeaf->SetActorRotation(f);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Spawned Leaf");
	leafs.Add(spawnedLeaf);


}

void ABranch::spawnRandomBranch() {

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


	ABranch* spawnedBranch = (ABranch*) GetWorld()->SpawnActor(Branch_BP);

	FTransform trans = getRandomPositionOnBranch();

	spawnedBranch->SetActorLocation(trans.GetLocation());
	spawnedBranch->SetActorRotation(trans.Rotator());

	branches.Add(spawnedBranch);

}
void ABranch::annihilate() {
	for (ABranch* b : branches) {
		b->annihilate();
	}
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

	//DrawDebugLine(
	//	GetWorld(),
	//	beamStart,
	//	pos,
	//	FColor(0, 255, 0),
	//	true, -1, 0,
	//	1
	//	);
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		beamStart,    //start
		pos, //end
		ECC_Camera, //collision channel
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
	for (ABranch* b : branches) {
		totalCost += b->calculateCost();
	}
	for (ALeaf* l : leafs) {
		totalCost += l->cost;
	}
	totalCost += cost;

	return totalCost;
}

void ABranch::addBranch(ABranch * b)
{
	branches.Add(b);
}

void ABranch::addLeaf(ALeaf * l)
{
	leafs.Add(l);
}

ABranch* ABranch::duplicate(FVector originalLocation, FVector newLocation) {

	//ABranch* spawnedBranch = (ABranch*)GetWorld()->SpawnActor(Branch_BP);
	FVector diff = GetActorLocation() - originalLocation;
	//FTransform t = GetTransform();
	FVector location = newLocation + diff;
	//t.SetLocation();
	//spawnedBranch->SetActorTransform(t);

	ABranch* spawnedBranch = GetWorld()->SpawnActor<ABranch>(Branch_BP, location, GetActorRotation());


	for (ABranch* b : branches) {
		spawnedBranch->addBranch(b->duplicate(originalLocation, newLocation));
	}
	for (ALeaf* l : leafs) {
		spawnedBranch->addLeaf(l->duplicate(originalLocation, newLocation));
	}


	return spawnedBranch;
}
