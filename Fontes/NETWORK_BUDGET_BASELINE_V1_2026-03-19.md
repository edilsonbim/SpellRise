# Network Budget Baseline - SpellRise (v1)

Data: 2026-03-19  
Escopo: Combate/Rede (C++/Config ciclo 1)  
Objetivo: estabelecer orçamento obrigatório antes de merge de qualquer feature de combate/rede.

## 1) Atores relevantes e frequência de rede

| Ator/Sistema | Estado atual | Budget v1 (limite) | Evidência |
|---|---|---|---|
| `ASpellRisePlayerState` | `NetUpdateFrequency=30`, `MinNetUpdateFrequency=10` | Manter 30/10 como baseline | `Source/SpellRise/Core/SpellRisePlayerState.cpp:56-57` |
| `ASR_ProjectileBase` | `bReplicates=true`, `SetReplicateMovement(true)` | `NetUpdateFrequency <= 30` (quando configurado em BP/CDO) | `Source/SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.cpp:15-16` |
| `ASpellRiseCharacterBase` | replicação padrão sem override de frequência | `NetUpdateFrequency <= 30` (quando configurado em BP/CDO) | `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp` |

## 2) RPC budget por endpoint crítico

| RPC | Origem permitida | Max RPC/s por player | Payload máximo (bytes) | Validação obrigatória |
|---|---|---:|---:|---|
| `ServerSendGameplayEventToSelf` | Owner Character | 20/s por tag (`0.25s * 5`) | 256 | allowlist + magnitude + finite + context + rate-limit |
| `ServerSetRespawnBedReferenceData` | Owner PlayerState | 2/s | 704 (`actor+class+location`) | owner match + sanidade + resolução autoritativa |
| `ServerSetArchetype` | Owner Character | 2/s | 16 | enum válido + contexto + anti-spam (Lote 2) |
| `ServerSetSelectedAbilityInputTag` | Owner Character | 20/s | 64 | tag permitida + contexto + anti-spam (Lote 2) |

Nota: valores de payload são teto de política para revisão/QA; enforcement de bytes na borda entra no Lote 2.

## 3) Target Data budget

| Fluxo | Limite v1 |
|---|---:|
| Envio de target data por jogador | até 20 envios/s |
| Tamanho por target data de projétil | até 256 bytes úteis |
| Falha de validação server | reject seguro + telemetria categorizada |

Evidências de fluxo:
- `Source/SpellRise/GameplayAbilitySystem/Abilities/GA_SR_ProjectileBase.cpp:603`
- `Source/SpellRise/GameplayAbilitySystem/Abilities/GA_SR_ProjectileBase.cpp:620`
- `Source/SpellRise/GameplayAbilitySystem/Abilities/GA_SR_ProjectileBase.cpp:867`

## 3.1) Budget de recálculo de recursos (atributos canônicos)

| Fluxo | RPC/s por player | Payload máximo | Observação |
|---|---:|---:|---|
| `GE_RecalculateResources` (MaxHealth/MaxMana/MaxStamina/CarryWeight via MMCs canônicos) | 0 | 0 | Sem RPC dedicado; atualização via replicação normal de atributos/GAS |

Referência de asset:
- `/Game/GamePlayAbilitySystem/Effects/Attributes/GE_RecalculateResources` usando `MMC_MaxHealthFromPrimaries`, `MMC_MaxManaFromPrimaries`, `MMC_MaxStaminaFromPrimaries`, `MMC_CarryWeightFromPrimaries`.

Validação de rede (2026-03-20):
- Build `SpellRiseEditor Win64 Development` aprovado.
- Smoke DS+2 normal: `OverallPass=True`.
- Smoke DS+2 com lag/loss: `OverallPass=True`.

## 3.2) Cobertura de orçamento por feature (Phase 1)

| Feature Phase 1 | NetUpdateFrequency | RPC/s | Payload máx | Target data |
|---|---|---|---|---|
| Gameplay event forwarding (`ServerSendGameplayEventToSelf`) | N/A (endpoint em `Character`) | 20/s por tag | 256 B | N/A |
| Respawn bed (`ServerSetRespawnBedReferenceData`) | N/A (endpoint em `PlayerState`) | 2/s | 704 B | N/A |
| Archetype + ability wheel (`ServerSetArchetype`, `ServerSetSelectedAbilityInputTag`) | N/A (endpoint em `Character`) | 2/s e 20/s | 16 B e 64 B | N/A |
| Projectile targeting (`GA_SR_ProjectileBase`) | `Character <= 30`, `Projectile <= 30` | N/A (usa target data GAS) | N/A | 20 envios/s, 256 B úteis |
| Recalculate resources (`GE_RecalculateResources`) | replicação padrão de atributos | 0 | 0 | N/A |

Status: cobertura de orçamento declarada para o recorte de combate/rede da Phase 1 em 2026-03-20.

## 4) Regras de aprovação de PR (combate/rede)
- Todo PR deve declarar impacto de orçamento nos 4 eixos:
  - NetUpdateFrequency
  - RPC/s por player
  - payload máximo por RPC
  - taxa/tamanho de target data
- PR sem orçamento explícito deve ser marcado como risco de regressão de rede.

## 5) Cenários mínimos de validação de orçamento
- Standalone
- Listen Server
- Dedicated Server (2+ clientes)
- Rede degradada:
  - `Net PktLag=120`
  - `Net PktLoss=5`
- Aceite:
  - sem double commit/double damage
  - sem saturação de RPC acima dos limites definidos
