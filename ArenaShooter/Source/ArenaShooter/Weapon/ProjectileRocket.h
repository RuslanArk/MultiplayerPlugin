// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

class UAudioComponent;
class UStaticMeshComponent;
class UNiagaraSystem;

class URocketMovementComponent;

UCLASS()
class ARENASHOOTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
	
protected:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* RocketTrail;

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* RocketMovement;

public:
	AProjectileRocket();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void DestroyTimerFinished();
	
};
