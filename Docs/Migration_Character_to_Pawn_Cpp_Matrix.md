# Matriz C++ - Migracao Character para Pawn (Big Bang)

Data: 2026-04-03 23:08:56 -03:00
Escopo auditado: Source/SpellRise + removidos planejados (total 111 arquivos).

| Arquivo | Classificacao |
| --- | --- |
| Source/SpellRise/Characters/SpellRiseCharacterBase.cpp | Remocao |
| Source/SpellRise/Characters/SpellRiseCharacterBase.h | Remocao |
| Source/SpellRise/Characters/SpellRisePawnBase.cpp | Refactor de fluxo |
| Source/SpellRise/Characters/SpellRisePawnBase.h | Refactor de fluxo |
| Source/SpellRise/Components/CatalystComponent.cpp | Troca de tipo |
| Source/SpellRise/Components/CatalystComponent.h | Troca de tipo |
| Source/SpellRise/Components/FallDamageComponent.cpp | Remocao |
| Source/SpellRise/Components/FallDamageComponent.h | Remocao |
| Source/SpellRise/Components/FallDamageMoverComponent.cpp | Refactor de fluxo |
| Source/SpellRise/Components/FallDamageMoverComponent.h | Refactor de fluxo |
| Source/SpellRise/Components/SpellRiseBuildInputFacade.cpp | Sem impacto |
| Source/SpellRise/Components/SpellRiseBuildInputFacade.h | Sem impacto |
| Source/SpellRise/Components/SpellRiseChatComponent.cpp | Sem impacto |
| Source/SpellRise/Components/SpellRiseChatComponent.h | Sem impacto |
| Source/SpellRise/Components/SpellRiseCombatUIComponent.cpp | Sem impacto |
| Source/SpellRise/Components/SpellRiseCombatUIComponent.h | Sem impacto |
| Source/SpellRise/Components/SpellRiseConstructionModeComponent.cpp | Sem impacto |
| Source/SpellRise/Components/SpellRiseConstructionModeComponent.h | Sem impacto |
| Source/SpellRise/Components/SpellRiseNarrativeBuildBridge.cpp | Sem impacto |
| Source/SpellRise/Components/SpellRiseNarrativeBuildBridge.h | Sem impacto |
| Source/SpellRise/Core/SpellRiseChatTypes.h | Sem impacto |
| Source/SpellRise/Core/SpellRiseCombatLogTypes.h | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameInstance.cpp | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameInstance.h | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameModeBase.cpp | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameModeBase.h | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameState.cpp | Sem impacto |
| Source/SpellRise/Core/SpellRiseGameState.h | Sem impacto |
| Source/SpellRise/Core/SpellRisePlayerController.cpp | Troca de tipo |
| Source/SpellRise/Core/SpellRisePlayerController.h | Sem impacto |
| Source/SpellRise/Core/SpellRisePlayerState.cpp | Sem impacto |
| Source/SpellRise/Core/SpellRisePlayerState.h | Sem impacto |
| Source/SpellRise/Equipment/SpellRiseEquipmentInstance.cpp | Sem impacto |
| Source/SpellRise/Equipment/SpellRiseEquipmentInstance.h | Sem impacto |
| Source/SpellRise/Equipment/SpellRiseEquipmentManagerComponent.cpp | Sem impacto |
| Source/SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyle.cpp | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyle.h | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyleNiagara.cpp | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyleNiagara.h | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.cpp | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.cpp | Sem impacto |
| Source/SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_MeleeCombo.cpp | Troca de tipo |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_MeleeCombo.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_FromEquipment.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_FromEquipment.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.cpp | Troca de tipo |
| Source/SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h | Troca de tipo |
| Source/SpellRise/GameplayAbilitySystem/Abilities/USpellRiseGA_CancelCast.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Abilities/USpellRiseGA_CancelCast.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.cpp | Refactor de fluxo |
| Source/SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/ExecCalc_Damage.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/ExecCalc_Damage.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_CarryWeight.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_CarryWeight.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_DerivedStats.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_DerivedStats.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxHealth.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxHealth.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxMana.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxMana.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxStamina.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxStamina.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Catalyst/CatalystData.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Catalyst/CatalystData.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.cpp | Troca de tipo |
| Source/SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.cpp | Sem impacto |
| Source/SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h | Sem impacto |
| Source/SpellRise/Inventory/SpellRiseFullLootSubsystem.cpp | Refactor de fluxo |
| Source/SpellRise/Inventory/SpellRiseFullLootSubsystem.h | Refactor de fluxo |
| Source/SpellRise/Inventory/SpellRiseLootBagActor.cpp | Sem impacto |
| Source/SpellRise/Inventory/SpellRiseLootBagActor.h | Sem impacto |
| Source/SpellRise/Inventory/SpellRiseLootBagNameComponent.cpp | Sem impacto |
| Source/SpellRise/Inventory/SpellRiseLootBagNameComponent.h | Sem impacto |
| Source/SpellRise/Persistence/SpellRiseBuildPersistenceAdapter.cpp | Sem impacto |
| Source/SpellRise/Persistence/SpellRiseBuildPersistenceAdapter.h | Sem impacto |
| Source/SpellRise/Persistence/SpellRiseFilePersistenceProvider.cpp | Sem impacto |
| Source/SpellRise/Persistence/SpellRiseFilePersistenceProvider.h | Sem impacto |
| Source/SpellRise/Persistence/SpellRisePersistenceProvider.h | Sem impacto |
| Source/SpellRise/Persistence/SpellRisePersistenceSubsystem.cpp | Refactor de fluxo |
| Source/SpellRise/Persistence/SpellRisePersistenceSubsystem.h | Refactor de fluxo |
| Source/SpellRise/Persistence/SpellRisePersistenceTypes.h | Refactor de fluxo |
| Source/SpellRise/Persistence/SpellRisePostgresPersistenceProvider.cpp | Sem impacto |
| Source/SpellRise/Persistence/SpellRisePostgresPersistenceProvider.h | Sem impacto |
| Source/SpellRise/Public/PrimaryAttributeRow.h | Sem impacto |
| Source/SpellRise/Security/SpellRiseAuditTrail.cpp | Sem impacto |
| Source/SpellRise/Security/SpellRiseAuditTrail.h | Sem impacto |
| Source/SpellRise/SpellRise.Build.cs | Sem impacto |
| Source/SpellRise/SpellRise.cpp | Sem impacto |
| Source/SpellRise/SpellRise.h | Sem impacto |
| Source/SpellRise/Tests/SpellRiseGameplayAbilitySmokeTest.cpp | Sem impacto |
| Source/SpellRise/Tests/SpellRiseGameplayAbilitySmokeTest.h | Sem impacto |
| Source/SpellRise/UI/SpellRiseCombatHUDWidget.h | Sem impacto |
| Source/SpellRise/UI/SpellRiseDamageEdgeWidget.cpp | Sem impacto |
| Source/SpellRise/UI/SpellRiseDamageEdgeWidget.h | Sem impacto |
| Source/SpellRise/UI/SpellRiseDeathScreenWidget.cpp | Sem impacto |
| Source/SpellRise/UI/SpellRiseDeathScreenWidget.h | Sem impacto |

## Resumo
- Sem impacto: 90
- Troca de tipo: 7
- Refactor de fluxo: 10
- Remocao: 4

## Gates
- Gate estrutural adicionado: `Scripts/Check-NoCharacterLegacy.ps1`.
- Gate integrado em `Scripts/SpellRise-Workflow.ps1` e `Scripts/Run-Smoke-Gate.ps1`.

## Persistencia
- Schema de personagem atualizado para versao 6 (Pawn).
- Migracao offline adicionada: `Scripts/Migrate-CharacterSaveData-To-Pawn.ps1`.
