// Copyright Solstice Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NetworkedGameplayMessageComponent.generated.h"

// Place on the Game State class
UCLASS(ClassGroup=(GameplayMessages), meta=(BlueprintSpawnableComponent))
class NETWORKEDGAMEPLAYMESSAGES_API UNetworkedGameplayMessageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNetworkedGameplayMessageComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	template <typename T>
	void BroadcastMessage(FGameplayTag Channel, const T& Message, bool bReliable = false)
	{
		const UScriptStruct* StructType = TBaseStructure<T>::Get();
		BroadcastMessageInternal(Channel, StructType, reinterpret_cast<uint8*>(&Message), bReliable);
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, CustomThunk, Category=Messaging, meta=(CustomStructureParam="Message", AllowAbstract="false", DisplayName="Broadcast Message"))
	void BroadcastMessage(FGameplayTag Channel, const int32& Message, bool bReliable = false);

	DECLARE_FUNCTION(execBroadcastMessage);

	void BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, uint8* MessageBytes, bool bReliable);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMessage(FGameplayTag Channel, const FInstancedStruct& Message);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastMessageUnreliable(FGameplayTag Channel, const FInstancedStruct& Message);
};
