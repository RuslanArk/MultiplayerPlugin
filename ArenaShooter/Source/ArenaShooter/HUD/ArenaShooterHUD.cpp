// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaShooterHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#include "ArenaShooter/HUD/CharacterOverlay.h"
#include "ArenaShooter/HUD/Announcement.h"

void AArenaShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter { ViewportSize.X / 2.f, ViewportSize.Y / 2.f };

		const float SpreadScaled = CrosshairsSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter)
		{
			const FVector2D Spread{0.f, 0.f};
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, Spread, HUDPackage.CrossHairsColor);
		}
		if (HUDPackage.CrosshairTop)
		{
			const FVector2D Spread {0.f, -SpreadScaled};
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, Spread, HUDPackage.CrossHairsColor);
		}
		if (HUDPackage.CrosshairBottom)
		{
			const FVector2D Spread {0.f, SpreadScaled};
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, Spread, HUDPackage.CrossHairsColor);
		}
		if (HUDPackage.CrosshairLeft)
		{
			const FVector2D Spread {-SpreadScaled, 0.f};
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, Spread, HUDPackage.CrossHairsColor);
		}
		if (HUDPackage.CrosshairRight)
		{
			const FVector2D Spread {SpreadScaled, 0.f};
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, Spread, HUDPackage.CrossHairsColor);
		}		
	}
}

void AArenaShooterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AArenaShooterHUD::AddCharacterOverlay()
{
	if (!CharacterOverlayClass) return;
	
	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AArenaShooterHUD::AddAnnouncement()
{
	if (!AnnouncementClass) return;
	
	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AArenaShooterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint {
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	};

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
	
}
