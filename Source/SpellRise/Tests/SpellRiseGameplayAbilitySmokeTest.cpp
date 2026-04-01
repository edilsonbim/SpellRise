#include "SpellRise/Tests/SpellRiseGameplayAbilitySmokeTest.h"
#include "Misc/AutomationTest.h"
#include "GameFramework/PlayerController.h"
#include "UObject/Package.h"
#include "Net/UnrealNetwork.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpellRiseAbilityInputBroadcastSmokeTest, "SpellRise.Smoke.AbilityInput.Broadcast", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FSpellRiseAbilityInputBroadcastSmokeTest::RunTest(const FString& Parameters)
{
	USpellRiseAbilityBroadcastSmokeTestAbility* TestAbility = NewObject<USpellRiseAbilityBroadcastSmokeTestAbility>(GetTransientPackage());
	TestAbility->AddToRoot();

	ASpellRiseAbilityBroadcastSmokeTestAvatar* ClientAvatar = NewObject<ASpellRiseAbilityBroadcastSmokeTestAvatar>(GetTransientPackage());
	ClientAvatar->bIsLocallyControlledOverride = true;
	ClientAvatar->SetRole(ROLE_AutonomousProxy);
	ClientAvatar->AddToRoot();

	FGameplayAbilityActorInfo ClientActorInfo;
	ClientActorInfo.AvatarActor = ClientAvatar;

	TestTrue("Cliente deve permitir eventos de input locais", TestAbility->IsInputEventAllowedForActorInfo(&ClientActorInfo));

	ASpellRiseAbilityBroadcastSmokeTestAvatar* ServerAvatar = NewObject<ASpellRiseAbilityBroadcastSmokeTestAvatar>(GetTransientPackage());
	ServerAvatar->bIsLocallyControlledOverride = false;
	ServerAvatar->SetRole(ROLE_Authority);
	ServerAvatar->AddToRoot();

	FGameplayAbilityActorInfo ServerActorInfo;
	ServerActorInfo.AvatarActor = ServerAvatar;

	TestFalse("Servidor dedicado não deve permitir eventos de input locais", TestAbility->IsInputEventAllowedForActorInfo(&ServerActorInfo));

	TestAbility->RemoveFromRoot();
	ServerAvatar->RemoveFromRoot();
	ClientAvatar->RemoveFromRoot();

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
