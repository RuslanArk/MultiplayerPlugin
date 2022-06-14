// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class UStaticMeshComponent;

UCLASS()
class ARENASHOOTER_API ACasing : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CasingMesh;
	
public:
	ACasing();

protected:
	virtual void BeginPlay() override;

public:

};
