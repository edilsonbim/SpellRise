# Revisão Global SpellRise - Auditoria de Conformidade (C++/Config)

Data: 2026-03-19  
Escopo: `Source/` + `Config/` (sem varredura de assets/Blueprint no ciclo 1)  
Fonte de verdade aplicada (ordem): `PROJECT_STATE.md` -> `ARCHITECTURE.md` -> `ATTRIBUTE_MATRIX.md` -> `MULTIPLAYER_RULES.md` -> `SECURITY_MODEL.md` -> `BACKLOG.md` -> `BUG_LOG.md` -> `ROADMAP.md` -> `COMBAT_DESIGN_PHILOSOPHY.md`

## 1) Matriz de conformidade obrigatória (MD -> código)

| Regra | Status | Evidência | Observação |
|---|---|---|---|
| ASC e AttributeSets no PlayerState | Conforme | `Source/SpellRise/Core/SpellRisePlayerState.cpp:59-67` | Owner do GAS no `PlayerState` está correto. |
| ASC em replication mode Mixed | Conforme | `Source/SpellRise/Core/SpellRisePlayerState.cpp:61` | Alinhado com docs. |
| Character consome ASC/ActorInfo do PlayerState | Conforme | `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp:691-695`, `:834` | Inicialização via `InitializeAbilitySystemFromPlayerState`. |
| Fluxo de projétil server-authoritative | Conforme (com risco de orçamento) | `Source/SpellRise/GameplayAbilitySystem/Abilities/GA_SR_ProjectileBase.cpp:534-537`, `:603`, `:620`, `:867` | Valida target data no servidor antes de commit/spawn. |
| Cliente não decide dano final | Conforme | `Source/SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.cpp:101-107`, `:122-139` | GE de dano aplicado em authority. |
| Multicast apenas para apresentação | Parcial | `Source/SpellRise/Characters/SpellRiseCharacterBase.h:123,132,138,141` | Uso atual parece cosmético; precisa checklist formal por RPC. |
| RPC server com validação de payload/contexto | Parcial | Forte em `ServerSendGameplayEventToSelf` (`SpellRiseCharacterBase.cpp:1238-1368`) e `ServerSetRespawnBedReferenceData` (`SpellRisePlayerState.cpp:103-255`) | `ServerSetArchetype` e `ServerSetSelectedAbilityInputTag` têm validação mínima e sem anti-spam. |
| Dedicated server sem dependência de UI | Parcial | UI com guard local (`SpellRisePlayerController.cpp:490-535`) | Sem bloqueio crítico achado, mas faltam smoke tests automatizados de DS. |
| Sem alias legado novo (VIG/FOC) | Parcial / conflito técnico | `CombatAttributeSet.h:103-104`, `MMC_MaxMana.cpp:6-30`, `MMC_MaxHealth.cpp`, `MMC_MaxStamina.cpp`, `MMC_CarryWeight.cpp` | Dívida já reconhecida; ainda coexistindo em runtime. |
| Compatibilidade de primários com matriz atual | Parcial / conflito técnico | `BasicAttributeSet.cpp:8-9` vs `MMC_DerivedStats.cpp:8-20` | Clamp runtime 20..100 e normalização até 120 coexistem. |
| Orçamento de rede declarado por feature | Não conforme | Ausência de declaração explícita em docs de runtime e código | Regra mandatória dos MD não está operacionalizada no repo. |

## 2) Achados priorizados (S0..S3)

## S0
- Nenhum S0 confirmado no escopo C++/Config deste ciclo.

## S1

### S1-01 - Ausência de orçamento de rede explícito por feature crítica
- Problema: não há contrato formal de orçamento (`NetUpdateFrequency`, RPC/s por player, payload bytes, taxa de target data) para combate/rede.
- Causa provável: hardening evoluiu por correções pontuais sem planilha/arquivo de budget versionado.
- Evidência:
  - Regra obrigatória em `Fontes/AGENTS.md:75-81`.
  - Apenas `PlayerState` define `SetNetUpdateFrequency/SetMinNetUpdateFrequency` (`Source/SpellRise/Core/SpellRisePlayerState.cpp:56-57`).
  - Projétil replicado sem budget explícito (`Source/SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.cpp:15-16`).
- Risco authority/prediction/RPC/OnRep:
  - RPC: risco de saturação por spam sem teto documentado por entrada.
  - Prediction: maior divergência visual sob latência se taxa/throughput variar sem limite.

### S1-02 - Logging crítico de combate/rede ainda concentrado em LogTemp
- Problema: fluxo crítico de combate/rede usa `LogTemp` amplamente.
- Causa provável: backlog de instrumentação temporária após correções rápidas.
- Evidência:
  - `SpellRiseCharacterBase.cpp:512,578,898,1151,1411,1495`.
  - `FallDamageComponent.cpp:57,79,116,129,326`.
  - `GA_SR_ProjectileBase.cpp:370,400,444`.
  - `SpellRiseAbilitySystemComponent.cpp:233,250,288`.
- Risco authority/prediction/RPC/OnRep:
  - RPC/security: baixa rastreabilidade por categoria em incidentes.
  - OnRep/debug: triagem de regressão mais lenta por falta de taxonomia de logs.

## S2

### S2-01 - RPCs server com validação mínima e sem anti-spam dedicado
- Problema: `ServerSetArchetype` e `ServerSetSelectedAbilityInputTag` não têm política explícita de rate limit/contexto além de validação básica do valor.
- Causa provável: priorização de validação completa apenas nos RPCs de maior risco imediato.
- Evidência:
  - `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp:508-518`.
  - `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp:574-582`.
  - Em contraste, RPC robusto: `ServerSendGameplayEventToSelf` (`:1238-1368`) e respawn-bed (`Source/SpellRise/Core/SpellRisePlayerState.cpp:103-255`).
- Risco authority/prediction/RPC/OnRep:
  - RPC: spam de seleção/arquétipo pode gerar churn de estado e ruído de replicação owner-only.

### S2-02 - Conflito de fonte de verdade: clamp primário vs normalização derivada
- Problema: runtime clampa primários em 20..100, enquanto MMC derivado trabalha com teto efetivo 120.
- Causa provável: migração incompleta do balance pass.
- Evidência:
  - `Source/SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.cpp:8-9,39`.
  - `Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_DerivedStats.cpp:8-20`.
  - Mismatch documentado em `Fontes/PROJECT_STATE.md` e `Fontes/ATTRIBUTE_MATRIX.md`.
- Risco authority/prediction/RPC/OnRep:
  - Prediction/UI: divergência de expectativa de scaling e tooltip/balance.

### S2-03 - Coexistência de aliases legados (VIG/FOC) e MMCs antigos no pipeline
- Problema: aliases e MMCs legados permanecem no código ativo/compilado.
- Causa provável: necessidade de compatibilidade de assets ainda não migrados.
- Evidência:
  - `Source/SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h:103-104`.
  - `Source/SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxHealth.cpp`, `MMC_MaxMana.cpp`, `MMC_MaxStamina.cpp`, `MMC_CarryWeight.cpp`.
  - Backlog cita unificação pendente: `Fontes/BACKLOG.md` (Attribute pipeline unification).
- Risco authority/prediction/RPC/OnRep:
  - OnRep/UI: inconsistência de origem de atributo derivado entre pipelines.

## S3

### S3-01 - Matriz operacional de RPC (origem/payload/limites) ainda não centralizada
- Problema: validações existem, mas sem documento único operacional por RPC.
- Causa provável: evolução incremental por bugfix.
- Evidência:
  - RPCs custom encontrados: `SpellRiseCharacterBase.h` (3 server RPC) e `SpellRisePlayerState.h` (1 server RPC).
- Risco authority/prediction/RPC/OnRep:
  - Governança: aumenta chance de regressão em alterações futuras de rede.

## 3) Matriz obrigatória de validação de RPC (estado atual)

| RPC | Origem permitida | Payload/Limites | Validações de contexto | Anti-spam | Falha segura + log |
|---|---|---|---|---|---|
| `ServerSendGameplayEventToSelf` | Owner do Character | `EventTag`, `EventMagnitude` (clamp lógico por tag) | allowlist + controller context + finite magnitude | Sim (janela por tag) | Sim (`AuditRejected...`, `LogSpellRiseSecurity`) |
| `ServerSetRespawnBedReferenceData` | Owner do PlayerState | strings com limite + `FVector_NetQuantize` finito | owner match + heurística bed + resolução autoritativa no mundo | Parcial (sem rate limit explícito) | Sim (`LogSpellRiseRespawnSecurity`) |
| `ServerSetArchetype` | Owner do Character | enum `ESpellRiseArchetype` | valida enum | Não explícito | Parcial (`LogTemp`) |
| `ServerSetSelectedAbilityInputTag` | Owner do Character | `FGameplayTag` | valida conjunto de tags permitidas | Não explícito | Parcial (`LogTemp`) |

## 4) Conflitos de fonte de verdade (código legado vs MD atual)
- Conflito 01: primários 20..100 (runtime) vs normalização até 120 (MMC derivado).
- Conflito 02: regra de não expandir aliases legados em implementações novas vs presença operacional de `VIG/FOC` em MMCs legados.
- Conflito 03: obrigação de orçamento de rede por feature vs ausência de baseline formal versionada para combate.

## 5) Checklist de teste (revisão documental)
- Standalone: checklist pronto.
- Listen Server: checklist pronto.
- Dedicated Server 2+ clientes: checklist pronto.
- Rede degradada: incluir testes com `Net PktLag=120` e `Net PktLoss=5` para validar ausência de double commit/damage.
