// Copyright Solstice Games

#include "NetworkedGameplayMessageComponent.h"
#include "InstancedStruct.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NetworkedGameplayMessageComponent)

UNetworkedGameplayMessageComponent::UNetworkedGameplayMessageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNetworkedGameplayMessageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Cast<AGameStateBase>(GetOwner()))
	{
		ensureMsgf(false, TEXT("NetworkedGameplayMessageComponent must be attached to a GameStateBase."));
	}
}

void UNetworkedGameplayMessageComponent::BroadcastMessage(FGameplayTag Channel, const int32& Message, bool bReliable)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UNetworkedGameplayMessageComponent::execBroadcastMessage)
{
	P_GET_STRUCT(FGameplayTag, Channel);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	uint8* MessagePtr = Stack.MostRecentPropertyAddress;
	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_GET_UBOOL(bReliable);

	P_FINISH;

	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (MessagePtr != nullptr)))
	{
		P_THIS->BroadcastMessageInternal(Channel, StructProp->Struct.Get(), MessagePtr, bReliable);
	}
}

void UNetworkedGameplayMessageComponent::BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType,
	uint8* MessageBytes, bool bReliable)
{
	FInstancedStruct Message;
	Message.InitializeAs(StructType, MessageBytes);
	UGameplayMessageSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();
	checkf(Subsystem, TEXT("GameplayMessageSubsystem is not registered in the GameInstance."));

	Subsystem->BroadcastMessage(Channel, Message);
	if (bReliable)
	{
		MulticastMessage(Channel, Message);
	}
	else
	{
		MulticastMessageUnreliable(Channel, Message);
	}
}

void UNetworkedGameplayMessageComponent::MulticastMessageUnreliable_Implementation(FGameplayTag Channel,
                                                                                   const FInstancedStruct& Message)
{
	UGameplayMessageSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();
	checkf(Subsystem, TEXT("GameplayMessageSubsystem is not registered in the GameInstance."));
	Subsystem->BroadcastMessageInternal(Channel, Message.GetScriptStruct(), Message.GetMemory());
}

void UNetworkedGameplayMessageComponent::MulticastMessage_Implementation(FGameplayTag Channel,
                                                                         const FInstancedStruct& Message)
{
	UGameplayMessageSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameplayMessageSubsystem>();
	checkf(Subsystem, TEXT("GameplayMessageSubsystem is not registered in the GameInstance."));
	Subsystem->BroadcastMessageInternal(Channel, Message.GetScriptStruct(), Message.GetMemory());
}
