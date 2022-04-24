// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{	
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 PlayersNum = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				600.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in Game : %d"), PlayersNum));
		}

		if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
		{
			FString PlayerName = PlayerState->GetPlayerName();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game"), *PlayerName));
			}
		}		
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{	
	if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
	{
		int32 PlayersNum = GameState.Get()->PlayerArray.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				600.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in Game : %d"), PlayersNum - 1));
		}
		
		FString PlayerName = PlayerState->GetPlayerName();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("%s has left the game"), *PlayerName));
		}
	}
	
	Super::Logout(Exiting);
}
