// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ArenaShooterHUD.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

	UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;

	float CrosshairSpread;
};

UCLASS()
class ARENASHOOTER_API AArenaShooterHUD : public AHUD
{
	GENERATED_BODY()

	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairsSpreadMax = 16.f;

public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage Package) { HUDPackage = Package; }

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);
	
};
