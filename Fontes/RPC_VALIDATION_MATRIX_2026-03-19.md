# Matriz Operacional de Validação de RPC - SpellRise

Data: 2026-03-19  
Escopo: C++/Config (Ciclo 1)  
Fonte de verdade aplicada: `PROJECT_STATE.md` -> `ARCHITECTURE.md` -> `ATTRIBUTE_MATRIX.md` -> `MULTIPLAYER_RULES.md` -> `SECURITY_MODEL.md`

## 1) Server RPCs (entrada autoritativa)

| RPC | Evidência (decl/impl) | Origem permitida | Payload + limites | Validação de contexto | Anti-spam | Falha segura + log |
|---|---|---|---|---|---|---|
| `ASpellRiseCharacterBase::ServerSendGameplayEventToSelf` | `Source/SpellRise/Characters/SpellRiseCharacterBase.h:112`, `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp:1333` | Owner `Character` (AutonomousProxy -> Server) | `FGameplayEventData`; tag em allowlist; magnitude finita e limitada por tag | server-only + validação interna de tag/magnitude antes de aplicar evento | Janela por tag (`0.25s`, max `10`) | reject sem mutação + log de segurança (categoria explícita) |
| `ASpellRiseCharacterBase::ServerSetArchetype` | `.../SpellRiseCharacterBase.h:169`, `.../SpellRiseCharacterBase.cpp:511` | Owner `Character` | enum válido | `ValidateServerRpcOwnerContext` (`...cpp:1465`), morto/inválido bloqueia, faixa de enum válida | `CheckServerRpcRateLimit` (`...cpp:1495`) janela `1.0s`, max `2` | `AuditRejectedServerRpc` (`...cpp:1535`) + reject sem mutação |
| `ASpellRiseCharacterBase::ServerSetSelectedAbilityInputTag` | `.../SpellRiseCharacterBase.h:247`, `.../SpellRiseCharacterBase.cpp:608` | Owner `Character` | `FGameplayTag` wheel (Skill1..Skill8/empty), tamanho prático <= 64 bytes | owner context + estado morto + validação de tag de wheel + ASC/contexto | janela `0.25s`, max `5` (20/s) | auditoria de rejeição + reject sem mutação |
| `ASpellRisePlayerState::ServerSetRespawnBedReferenceData` | `Source/SpellRise/Core/SpellRisePlayerState.h:50`, `Source/SpellRise/Core/SpellRisePlayerState.cpp:103` | Owner `PlayerState` | `ActorName<=128`, `ClassPath<=512`, `FVector_NetQuantize` finito | `ValidateRespawnBedPayload` (`...cpp:204`), owner/context, sanidade, bed resolution server-side (`ResolveRespawnBedActorOnServer`) | `CheckRespawnBedServerRateLimit` (`...cpp:174`) janela `1.0s`, max `2` | reject sem mutação + `LogSpellRiseRespawnSecurity` |
| `ASpellRisePlayerController::ServerSR_ForceDeath` | `Source/SpellRise/Core/SpellRisePlayerController.h`, `Source/SpellRise/Core/SpellRisePlayerController.cpp` | Owner `PlayerController` (AutonomousProxy -> Server) | sem payload | server-only, valida pawn/ASC válidos e estado de morte antes de mutar atributo | N/A (hook QA interno) | reject sem mutação + log categorizado `[Debug][ForceDeath]` |

## 2) Client/Multicast RPCs (somente apresentação)

| RPC | Evidência | Tipo | Regra de uso |
|---|---|---|---|
| `MultiHandleDeath` | `Source/SpellRise/Characters/SpellRiseCharacterBase.h:124`, `...cpp:1858` | NetMulticast Reliable | apenas apresentação/sync após decisão autoritativa de morte |
| `MultiOnCatalystProc` | `.../SpellRiseCharacterBase.h:133`, `...cpp:2028` | NetMulticast Unreliable | feedback cosmético |
| `MultiShowDamagePop` | `.../SpellRiseCharacterBase.h:139`, `...cpp:2033` | NetMulticast Unreliable | feedback de UI/VFX |
| `MultiPlayHitReactionMontage` | `.../SpellRiseCharacterBase.h:142`, `...cpp:2076` | NetMulticast Unreliable | feedback de animação |
| `USpellRiseChatComponent::Multi_ReceivePublicMessage` | `Source/SpellRise/Components/SpellRiseChatComponent.h:26`, `...cpp:202` | NetMulticast Reliable | entrega de chat público (não autoritativo de combate) |
| `ASpellRisePlayerController::ClientReceiveCombatLogEntry` | `Source/SpellRise/Core/SpellRisePlayerController.h:66`, `...cpp:1209` | Client Reliable | entrega de log para UI/local |
| `ASpellRisePlayerController::ClientReceiveChatMessage` | `Source/SpellRise/Core/SpellRisePlayerController.h:69`, `...cpp:1225` | Client Reliable | entrega de mensagem para UI/local |

## 3) Checagens mandatórias para RPC novo/alterado (gate de review)

1. Origem permitida (`Owner`, `AutonomousProxy`, `Server-only`) declarada explicitamente.
2. Payload com limites objetivos (faixa, tamanho, sanidade, finite checks).
3. Validação de contexto no servidor (estado do ator, custo/cooldown/tags, distância/LOS quando aplicável).
4. Proteção anti-spam no servidor (rate-limit/cooldown por endpoint).
5. Falha segura (reject sem mutação) + log categorizado (`LogTemp` proibido em fluxo crítico).

## 4) Conflitos de fonte de verdade (status)

- Sem conflito aberto neste recorte de RPC após endurecimento de `ServerSetArchetype` e `ServerSetSelectedAbilityInputTag`.
- Sem pendência aberta de anti-spam para os RPCs server deste recorte.

## 5) Atualização de recorte - 2026-03-20 (Atributos canônicos)

- Mudança aplicada: `GE_RecalculateResources` migrado para classes canônicas de cálculo (`MMC_*FromPrimaries`), incluindo `MMC_CarryWeightFromPrimaries`.
- Impacto em RPC: **nenhum RPC novo/alterado**.
- Orçamento de rede do recorte: 0 RPC/s e 0 payload dedicado para esse fluxo (apenas replicação padrão de atributos/GAS).
- Evidência de validação: smoke DS+2 normal e DS+2 com lag/loss aprovados em 2026-03-20.

## 6) Atualização de recorte - 2026-03-20 (Hook determinístico de morte para QA)

- Mudança aplicada: comando `Exec` `SR_ForceDeath` no `PlayerController`, roteando para `ServerSR_ForceDeath` (non-shipping) para forçar `Health=0` no servidor e validar pipeline death/full-loot/respawn em smoke automatizado.
- Impacto em RPC: 1 RPC server novo, sem payload (owner-only, contexto validado no servidor).
- Uso previsto: apenas QA/automação; não altera fluxo de combate produtivo.
