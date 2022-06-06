// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;

UCLASS()
class ARENASHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	
public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	

};
