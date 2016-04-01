// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tree.h"
#include "GameFramework/Actor.h"
#include "SimLogic.generated.h"

UCLASS()
class TREEEVOLUTION_API ASimLogic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimLogic();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void init();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void simulationTick();


	UPROPERTY(EditAnywhere, Category = "Gameplay")
		int32 distance = 1000;


	UPROPERTY(EditAnywhere, Category = "Performance")
		int32 nbrLines = 10;


private:
	TSubclassOf<class ATree> Tree_BP;
	TArray<ATree*> trees;
	FRandomStream random;
	
};
