# Bug Log

## How To Use
- Create one entry per issue.
- Always include exact build/date and net mode.
- Keep reproduction deterministic.

## Template
- ID:
- Date:
- Severity: Critical | High | Medium | Low
- Status: Open | In Progress | Fixed | Verified | Closed
- Area:
- Issue:
- Reproduction:
- Expected:
- Actual:
- Root Cause:
- Fix:
- Tested On:
- Standalone:
- Listen Server:
- Dedicated Server:
- Owner:

## Open Issues
### BUG-2026-03-28-032
- Date: 2026-03-28
- Severity: Critical
- Status: Open
- Area: Online / Dedicated Server / Steam
- Issue: Dedicated Server sobe com `OnlineSubsystem=NULL` (fallback) e rejeita login de cliente `STEAM:*` com `PreLoginFailure` por `incompatible_unique_net_id`.
- Reproduction: Subir servidor dedicado na VPS com fluxo Steam ativo no client; observar no log server `Steam Dedicated Server API failed to initialize`, `AppId: 0`/fallback para `NULL`, seguido de `PreLogin failure` para `STEAM:*`.
- Expected: Dedicated Server inicializa Steam GameServer API com AppID válido e aceita `UniqueNetId` Steam quando o modo Steam estiver ativo.
- Actual: Servidor aceita handshake de rede, porém falha no pré-login por mismatch de subsystem/identity e fecha conexão.
- Root Cause: Mismatch entre bootstrap de Steam no DS (AppID/SDK/config/env) e identidade Steam enviada pelo client, levando o server a operar em `NULL`.
- Fix: Pendente.
- Tested On: Package Win64 Development (2026-03-28), Dedicated Server AWS + client remoto.
- Standalone: N/A
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Backend/Online

### BUG-2026-03-28-028
- Date: 2026-03-28
- Severity: Medium
- Status: Open
- Area: Input / Interact / Dedicated Server Client
- Issue: `Interact` não estava fechando de forma confiável no client de DS quando o fluxo dependia de descoberta runtime do `UNarrativeInteractionComponent`.
- Reproduction: Entrar em Dedicated Server com 1 client, acionar `Interact` em contexto com componente de narrativa no pawn e observar rejeição ou fallback inconsistente no log `[Input][Interact]`.
- Expected: O client local resolve o componente de interação e executa `BeginInteract`/`EndInteract` sem depender de estado de UI.
- Actual: Em alguns caminhos do client de DS, a resolução do componente falha e o input cai para o fallback de ability ou é rejeitado.
- Root Cause: Pendente; suspeita de descompasso entre o binding do input local e a resolução controller/pawn do componente de interação em DS client.
- Fix: Pendente.
- Tested On: SpellRiseEditor Win64 Development (2026-03-28) em Dedicated Server + 1 client.
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Gameplay/Input

### BUG-2026-03-28-029
- Date: 2026-03-28
- Severity: High
- Status: Open
- Area: Death / Respawn / Presentation
- Issue: Fluxo de morte/respawn com delay de 30s ainda pode deixar o client em tela preta por toda a janela, sem reconciliação visual clara.
- Reproduction: Morrer em sessão multiplayer, observar o delay de respawn de 30s e confirmar que o client permanece sem apresentação de retorno durante a janela inteira.
- Expected: O servidor mantém o delay de respawn, mas o client recebe feedback explícito de estado morto/respawn pendente e recupera apresentação de forma previsível.
- Actual: O respawn fica agendado no servidor, mas a apresentação do client não é reconciliada de forma suficiente durante a espera.
- Root Cause: Pendente; suspeita de que o timer de respawn server-authoritative não esteja desacoplado de uma camada de feedback local para o estado morto.
- Fix: Pendente.
- Tested On: SpellRiseEditor Win64 Development (2026-03-28) em Dedicated Server + 1 client.
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Multiplayer/Respawn

### BUG-2026-03-28-030
- Date: 2026-03-28
- Severity: Medium
- Status: Open
- Area: Combat / Chat / Fall Damage
- Issue: Mensagens de chat de dano por queda ainda podem mostrar causer/instigator genérico ou incorreto no fluxo de combate.
- Reproduction: Sofrer fall damage em cenário multiplayer e comparar a mensagem de chat gerada com o evento real de queda.
- Expected: O chat identifica o dano como queda de forma consistente, sem atribuir causer incorreto.
- Actual: Em alguns caminhos, a mensagem depende de fallback de causer/instigator e pode degradar para nome errado ou contexto ambíguo.
- Root Cause: Pendente; suspeita de contexto de dano incompleto no `GameplayEffectContext` de fall damage para a resolução da mensagem de chat.
- Fix: Pendente.
- Tested On: SpellRiseEditor Win64 Development (2026-03-28) em Dedicated Server + 1 client.
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Combat/Feedback

### BUG-2026-03-28-031
- Date: 2026-03-28
- Severity: High
- Status: Open
- Area: Online / Steam Auth Gating
- Issue: Gating de autenticação Steam ainda precisa ser explicitado para flows sensíveis de sessão e persistência em DS.
- Reproduction: Iniciar sessão em DS com Steam indisponível ou identidade local degradada e observar se paths sensíveis continuam aceitando fallback de identidade.
- Expected: Flows que exigem autenticação rejeitam identidade placeholder/offline e registram bloqueio seguro.
- Actual: O projeto ainda expõe fallback de identidade local que precisa ser separado com mais clareza dos caminhos que realmente exigem Steam auth.
- Root Cause: Pendente; suspeita de mistura entre bootstrap offline seguro e gating de auth para sessão/persistência.
- Fix: Pendente.
- Tested On: SpellRiseEditor Win64 Development (2026-03-28) em Dedicated Server + 1 client.
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Backend/Online

### BUG-2026-03-25-026
- Date: 2026-03-25
- Severity: Low
- Status: Fixed
- Area: Build / Plugin Metadata
- Issue: Build emitted warning that plugin `NarrativeInventory` depends on module `CommonUI` but did not declare plugin dependency in `.uplugin`.
- Reproduction: Run `Build.bat SpellRiseEditor Win64 Development` and inspect build output warning.
- Root Cause: `NarrativeInventory.Build.cs` depended on `CommonUI`, but `NarrativeInventory.uplugin` lacked `"Plugins"` dependency entry.
- Fix: Added `"Plugins": [{ "Name": "CommonUI", "Enabled": true }]` to `Plugins/NarrativeInventory/NarrativeInventory.uplugin`.
- Tested On: Build `SpellRiseEditor Win64 Development` succeeded and warning disappeared (2026-03-25).
- Standalone: N/A
- Listen Server: N/A
- Dedicated Server: N/A
- Owner: Plugins/Integration

### BUG-2026-03-25-027
- Date: 2026-03-25
- Severity: Low
- Status: Fixed
- Area: Build / UE 5.7 API Compatibility
- Issue: Deprecated usage of `FString::LeftInline` signature generated warning C4996.
- Reproduction: Compile `SpellRiseEditor Win64 Development` and inspect warning on `SpellRiseGameModeBase.cpp`.
- Root Cause: Legacy overload using `bool bAllowShrinking`.
- Fix: Replaced with `SafeToken.LeftInline(16, EAllowShrinking::No);`.
- Tested On: Build `SpellRiseEditor Win64 Development` succeeded with warning removed (2026-03-25).
- Standalone: N/A
- Listen Server: N/A
- Dedicated Server: N/A
- Owner: Core/Gameplay

### BUG-2026-03-16-001
- Date: 2026-03-16
- Severity: High
- Status: Fixed
- Area: GAS / Catalyst
- Issue: Resource damage flow writes SetByCaller with tag Data.CatalystCharge, while tag config currently defines Data.CatalystChargeDelta.
- Reproduction: Trigger damage that should add catalyst charge through GE_Catalyst_AddCharge and inspect resulting charge delta behavior.
- Root Cause: Tag name divergence between C++ and gameplay tags config.
- Fix: Code now writes Data.CatalystChargeDelta and keeps Data.CatalystCharge as legacy fallback.
- Tested On: SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-002
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: UI / FX
- Issue: Number pop component spawns an additional debug Niagara system per hit.
- Reproduction: Land repeated hits and profile Niagara component spawn count.
- Root Cause: Debug block left active in runtime path.
- Fix: Removed debug runtime Niagara spawn path and reduced log noise for high-frequency paths.
- Tested On: SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-003
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: Config / Boot flow
- Issue: Game map and server map defaults are duplicated with conflicting values between DefaultEngine.ini and DefaultGame.ini.
- Reproduction: Compare GameMapsSettings in both config files and run server/client startup.
- Root Cause: Drift between config sources.
- Fix: DefaultGame.ini became the single source of truth for GameDefaultMap/ServerDefaultMap/TransitionMap; conflicting keys were removed from DefaultEngine.ini.
- Tested On: Config validation + startup map checks (2026-03-17)

### BUG-2026-03-16-004
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: Chat / UI Integration
- Issue: Combat log append currently relies on reflective field/function lookup in Blueprint chat assets (`AddChatMessage`, `S_ChatMessage` field names).
- Reproduction: Rename chat component function or struct fields and trigger combat damage; combat message append may fail silently.
- Root Cause: No native C++ chat message contract is exposed by current Blueprint chat package.
- Fix: Introduced native `FSpellRiseChatMessage` + `USpellRiseChatComponent` and migrated combat-feed transport to C++; legacy reflection path kept as opt-in fallback only.
- Tested On: SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-005
- Date: 2026-03-16
- Severity: High
- Status: Fixed
- Area: Ability / Delegates
- Issue: Editor crash when causing damage through projectile ability flow.
- Reproduction: Cast projectile ability and apply damage during cast/release flow.
- Root Cause: Dynamic delegate bind target was not exposed as `UFUNCTION`, causing bind failure/ensure and subsequent instability.
- Fix: Added `UFUNCTION()` on `OnReleaseBeforeCastFinished` callback in `GA_SR_ProjectileBase`.
- Tested On: SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-006
- Date: 2026-03-16
- Severity: High
- Status: Fixed
- Area: UI / FX / Damage Pop
- Issue: Damage pop stopped rendering despite combat damage events firing.
- Reproduction: Deal spell damage and observe `[POP][PC]` logs without visible on-screen pop.
- Root Cause: Pop dispatch path depended on source actor context and Niagara component lifecycle was fragile in some runtime states.
- Fix: Dispatch pop from target character context and harden Niagara spawn/validation path in `USpellRiseNumberPopComponent_NiagaraText`.
- Tested On: Runtime validation with logs + SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-007
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: Combat Log UX
- Issue: Combat channel lacked death feedback and damage-type context.
- Reproduction: Kill target or take elemental/physical damage; log only showed raw damage number text.
- Root Cause: Chat message formatter did not resolve gameplay damage type tags and had no death message hook.
- Fix: Added damage-type tag resolution/labeling and death messages in `ResourceAttributeSet` combat log dispatch.
- Tested On: Runtime validation + SpellRiseEditor Win64 Development build (2026-03-16)

### BUG-2026-03-16-008
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / GameplayCue
- Issue: Gameplay cues from `GA_toHealth`, `GA_Shield`, and `GA_FillShield` stopped triggering consistently.
- Reproduction: Activate listed abilities and observe missing `GameplayCue.toHealth.Active`, `GameplayCue.ShieldUp`, and `GameplayCue.ShieldBubble`.
- Root Cause: Notify/cue path em BP estava inconsistente por configuração de Anim Notify, causando falha intermitente de disparo de cue.
- Fix:
  - Added package redirect in `DefaultEngine.ini` and both cue directories to always-cook in `DefaultGame.ini`.
  - Added reliable cue trigger API in `USpellRiseGameplayAbility` (`SR_TriggerGameplayCueReliable`).
  - Applied auto-cue fallback by ability tag/class hint for `toHealth` and `FillShield`.
  - Moved `ShieldBubble` lifecycle to GE state in ASC (`Status.Buff.Shield` add/remove).
  - Added combo-start fallback event dispatch for abilities tagged `GameplayAbility.MeleeAttack.Combo`.
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-009
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: Build / GAS Ability Base
- Issue: Build failed after defensive auto-GE fallback was added to `USpellRiseGameplayAbility`.
- Reproduction: Compile `SpellRiseEditor Win64 Development` after adding auto-application for shield/fillshield GEs.
- Root Cause: `ApplyGameplayEffectToOwner` in UE 5.7 expects `const UGameplayEffect*` but code passed `TSubclassOf<UGameplayEffect>`.
- Fix: Converted GE class references to CDO pointers via `GetDefaultObject<UGameplayEffect>()` before calling `ApplyGameplayEffectToOwner`.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-16.

### BUG-2026-03-16-010
- Date: 2026-03-16
- Severity: High
- Status: Fixed
- Area: GAS / GameplayCue / Input Replication
- Issue: `FillShield` cue blinked/disappeared and melee combo stopped at first hit on dedicated flow.
- Reproduction: Activate `GA_FillShield` repeatedly and execute `GA_MeleeAttack_AxeCombo`; observe short-lived cue and combo not chaining.
- Root Cause:
  - `FillShield` cue lifecycle was being driven by ability-level timed fallback, conflicting with BP/GE flow.
  - Combo input while ability was active was not being force-forwarded to server for combo-tagged abilities under current custom input pipeline.
- Fix:
  - Removed timed `FillShield` cue fallback from `USpellRiseGameplayAbility`.
  - Added GE lifecycle-driven `GameplayCue.ShieldUp` add/remove in `USpellRiseAbilitySystemComponent` for `GE_FillShield`.
  - Added server input forwarding for active specs tagged `GameplayAbility.MeleeAttack.Combo` (`ServerSetInputPressed/Released`) in `SR_ProcessAbilityInput`.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-16.

### BUG-2026-03-16-011
- Date: 2026-03-16
- Severity: High
- Status: Fixed
- Area: Input / GAS Combo
- Issue: `GA_MeleeAttack_AxeCombo` stopped at first hit after migration to input tags.
- Reproduction: Trigger attack chain using `IA_Attack`; ability activates first hit but does not continue combo window input reliably.
- Root Cause: `IA_Attack` path was directly activating ability by class (`TryActivateAbilityByClass`) and bypassing the unified input-tag/spec flow expected by combo continuation tasks (`WaitInputPress` / active spec input state).
- Fix:
  - Updated `ASpellRisePlayerController::OnAttackPressed` to route through `InputTag.Ability.Primary` first.
  - Updated `ASpellRisePlayerController::OnAttackReleased` to send release for `InputTag.Ability.Primary`.
  - Kept class activation as legacy fallback only when no ability is mapped to `InputTag.Ability.Primary`.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-16.

### BUG-2026-03-16-012
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Combo Input Tasks
- Issue: `GA_MeleeAttack_AxeCombo` still stalls on first hit in some runtime paths.
- Reproduction: Trigger combo ability and press attack again during combo window; chain does not continue.
- Root Cause: Janela de combo no BP não abria de forma consistente por evento de Anim Notify incorreto.
- Fix:
  - Added explicit `InvokeReplicatedEvent` calls for `InputPressed` and `InputReleased` in `USpellRiseAbilitySystemComponent::SR_ProcessAbilityInput` when spec is active.
  - Kept server forwarding (`ServerSetInputPressed/Released`) for combo-tagged active specs.
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-013
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Combo Window Events
- Issue: Combo chain still intermittently stalls at hit 1 despite input replication hardening.
- Reproduction: Activate melee combo and press again during expected chain window.
- Root Cause: Anim Notify de abertura/fechamento da janela de combo no BP estava inconsistente.
- Fix:
  - Added combo-active fallback in `SR_ProcessAbilityInput`: when combo spec is active and input is pressed, send `Event.ContinueCombo.Start` and `Event.ContinueCombo.Input` to avatar.
  - Added runtime diagnostics in `PlayerController` input handlers (`[GAS][Input] OnAttackPressed/Released`, `[GAS][Input] OnPrimaryPressed/Released`).
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-014
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Melee Combo Montage
- Issue: Combo did not advance to hit2 even with valid combo input/events reaching ASC.
- Reproduction: Spam/queue combo input during active `GA_MeleeAttack_AxeCombo`; logs show `ComboInput` and `Event.ContinueCombo.*` but no chain progression.
- Root Cause: Encadeamento de seção dependia de notifies BP com configuração inconsistente.
- Fix:
  - Added native fallback in `USpellRiseAbilitySystemComponent::SR_ProcessAbilityInput` to force montage section advance (`Montage_JumpToSection`) when combo ability is active and new combo input is pressed.
  - Added combo event telemetry (`[GAS][Event]`) and montage transition telemetry (`[GAS][ComboMontage]`).
  - Removed noisy warning for missing optional `IA_Attack`.
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-015
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Combo Timing Robustness
- Issue: Combo input/events reached ASC but montage transition still failed intermittently.
- Reproduction: Logs show repeated `[GAS][ComboInput]` and `[GAS][Event]` (`Start/Input/End`) without hit2 progression.
- Root Cause: Corrida de timing observada era efeito colateral de notify de janela de combo mal configurado no BP.
- Fix:
  - Reworked fallback to queue combo intent (`bComboAdvanceRequested`) on `Event.ContinueCombo.Input`.
  - Execute montage section advance on `Event.ContinueCombo.Start` (window-open signal), then clear queue.
  - Clear queue on `Event.ContinueCombo.End`.
  - Added `[GAS][ComboAdvance] Requested/Advanced` telemetry.
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-016
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Combo Architecture
- Issue: BP combo flow became fragile after input/tag migration and montage section timing drift.
- Reproduction: Combo events/input arrive but behavior remains unstable and hard to tune in BP graph-only flow.
- Fix:
  - Implemented native ability `USpellRiseGA_MeleeCombo` (`SpellRiseGA_MeleeCombo.h/.cpp`) with:
    - authoritative montage playback,
    - input buffering,
    - combo window gating by gameplay events,
    - section advancement per combo step.
  - Added Blueprint extension hooks (`On Combo Window Opened/Closed`, `On Combo Advanced`) for FX/gameplay integration.
- Tested On: Verified em runtime BP com ajuste de Anim Notify e migração aplicada no asset (2026-03-21).

### BUG-2026-03-16-017
- Date: 2026-03-16
- Severity: High
- Status: Verified
- Area: GAS / Combo Replication / HitScan Events
- Issue: Local combo/hitscan behavior diverged from remote view; combo consumed incorrectly and hitscan events were not consistently reaching BP damage path.
- Reproduction: Owner saw hit1 stop while remote saw different combo progression; no hitscan damage despite montage notify events.
- Root Cause:
  - Configuração de Anim Notify no BP estava causando perda/intermitência de eventos de HitScan e janela de combo.
  - O problema se manifestava como divergência de consumo de combo entre owner/remoto.
- Fix:
  - Stopped held-tick `InputPressed` spam for active native combo specs in ASC.
  - Disabled base combo-start fallback for native combo ability class.
  - Removed payload filter gate from native hitscan callbacks (`OnHitScanStart/End`) so BP events always fire when notify event is received.
- Tested On: Verified em runtime BP com ajuste de Anim Notify (2026-03-21).

### BUG-2026-03-16-018
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: UI / Damage Pop Readability
- Issue: Damage pop text became hard to read at long distances due to world-space scaling behavior.
- Reproduction: Land hits at short and long range and compare apparent pop size in screen space.
- Root Cause: Niagara pop renderer was using fixed world scale, causing strong perceived shrink as distance increased.
- Fix:
  - Added distance-compensated world scale in `USpellRiseNumberPopComponent_NiagaraText`.
  - Added tuning properties:
    - `bCompensateScaleByDistance`
    - `ReferenceDistance`
    - `MinWorldScale`
    - `MaxWorldScale`
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-16.

### BUG-2026-03-16-019
- Date: 2026-03-16
- Severity: Medium
- Status: Fixed
- Area: UI / Damage Pop Readability
- Issue: User requested truly constant perceived size for damage numbers regardless of target distance.
- Reproduction: Compare pop size at close and long ranges with distance compensation enabled.
- Root Cause: Previous compensation still clamped scale and did not preserve exact constant apparent size.
- Fix:
  - Added `bExactScreenSizeCompensation` to `USpellRiseNumberPopComponent_NiagaraText`.
  - When enabled, world scale is set exactly proportional to camera distance (`distance/reference`) without clamp.
  - Kept clamped mode available for projects that prefer bounded sizing.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-16.

### BUG-2026-03-17-020
- Date: 2026-03-17
- Severity: High
- Status: Fixed
- Area: GAS / Melee Combo / Replicação
- Issue: Combo melee alternava entre auto-chain com 1 clique e travamento no hit1, com consumo instável dos eventos de notify.
- Reproduction: Ativar `GA_MeleeAttack_AxeCombo` e testar em owner client/listen/dedicated; observar avanço inesperado sem novo clique ou falta de avanço para hit2.
- Root Cause:
  - `NativeOnAbilityInputPressed` aceitava input antes da janela de combo abrir (pré-buffer indesejado do clique inicial).
  - Filtro de payload rejeitava eventos de montage notify com payload anônimo, comuns em alguns caminhos de runtime.
  - Resolução de `AnimInstance` estava sensível ao mesh do avatar em vez de priorizar `ActorInfo`.
- Fix:
  - Em `USpellRiseGA_MeleeCombo`, input de avanço agora só é aceito com `bComboWindowOpen == true`.
  - `IsPayloadFromAvatar` passa a aceitar payload anônimo no contexto da ability ativa.
  - Novo helper `GetComboAnimInstance()` prioriza `CurrentActorInfo->GetAnimInstance()` para sincronismo de montage da ability.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-17.

### BUG-2026-03-17-021
- Date: 2026-03-17
- Severity: Medium
- Status: Fixed
- Area: GAS / Melee Combo / Input Buffer
- Issue: Clique muito rápido durante a transição para a janela de combo não era reconhecido e o fluxo travava no hit1.
- Reproduction: Em `GA_MeleeAttack_AxeCombo`, fazer taps rápidos em sequência; parte dos inputs de continuação é perdida.
- Root Cause:
  - Gating estrito exigindo `bComboWindowOpen` no momento exato do `InputPressed`.
  - Ausência de buffer controlado entre input rápido e abertura da janela.
- Fix:
  - Reintroduzido buffer controlado de continuação (`bQueuedNextInput`) com consumo apenas dentro da janela.
  - Adicionado filtro `bIgnoreFirstActivationPress` para impedir auto-chain do clique inicial de ativação.
  - Limpeza do filtro no `InputReleased` para não bloquear tap seguinte.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-17.

### BUG-2026-03-17-022
- Date: 2026-03-17
- Severity: High
- Status: Fixed
- Area: GAS / Melee Combo / HitScan Consistency
- Issue: `HitScan` às vezes permanecia ativo por perda de `HitScanEnd`, e combo ainda falhava em taps rápidos perto da abertura da janela.
- Reproduction: Executar combo com spam de clique e troca rápida de seção; observar casos de trace ativo após fim de seção e/ou perda de avanço de combo.
- Root Cause:
  - Dependência excessiva de notify de fim para desligar hitscan sem fail-safe nativo.
  - Input rápido próximo da janela podia expirar sem tolerância temporal.
- Fix:
  - Adicionado estado nativo `bHitScanActive` com desligamento garantido em:
    - `OnComboWindowEnd`
    - `OnComboMontageCompleted`
    - `OnComboMontageBlendOut`
    - `OnComboMontageInterrupted`
    - `OnComboMontageCancelled`
    - `EndAbility`
  - Adicionado buffer temporal de input (`QueuedInputTimeSeconds` + `InputBufferGraceSeconds`) para aceitar continue combo com taps rápidos perto da janela.
  - Em troca de seção (`TryAdvanceComboSection`), força `StopHitScanIfActive()` antes de encadear próxima seção.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-17.

### BUG-2026-03-17-023
- Date: 2026-03-17
- Severity: Medium
- Status: Fixed
- Area: GAS / Combo Diagnostics
- Issue: Fluxo crítico de combo ainda dependia de `LogTemp`, dificultando triagem específica do sistema.
- Reproduction: Analisar logs durante `ContinueCombo`/troca de seção/hitscan e observar mistura de categorias gerais.
- Root Cause: Logging legado permaneceu em `LogTemp` em pontos críticos do combo.
- Fix:
  - Introduzida categoria dedicada `LogSpellRiseCombo` em `USpellRiseGA_MeleeCombo` e `USpellRiseAbilitySystemComponent`.
  - Logs críticos de combo migrados para `Verbose` nessa categoria.
  - Fallback legado no ASC mantido apenas para abilities de combo não nativas.
- Tested On: SpellRiseEditor Win64 Development build succeeded on 2026-03-17.

### BUG-2026-03-17-024
- Date: 2026-03-17
- Severity: High
- Status: Fixed
- Area: GAS / Damage / Armor Penetration
- Issue: Physical armor penetration had near-zero practical effect despite derived stats indicating up to 30%.
- Reproduction: Compare physical damage against high-resistance targets with low vs high STR-driven armor penetration; mitigation delta remains negligible.
- Root Cause: Unit mismatch in ExecCalc treated ArmorPenetration as percent (0..30) while runtime value is fractional (0..0.30), followed by extra `/100` conversion.
- Fix: Standardized ExecCalc to consume ArmorPenetration as fractional value and clamp directly to 0..0.30 without duplicate conversion.
- Tested On: Compile validation + formula/code-path inspection (2026-03-17)

### BUG-2026-03-17-025
- Date: 2026-03-17
- Severity: High
- Status: Fixed
- Area: Network Security / Respawn
- Issue: `ServerSetRespawnBedReferenceData` accepted client payload with no server-side context/payload validation.
- Reproduction: Call RPC with forged actor/class/location data from client and observe server accepting owner-only replicated respawn fields.
- Root Cause: Missing server-side validation and missing authoritative resolution of requested bed actor.
- Fix:
  - Added context validation (`owner/controller/playerstate` coherence).
  - Added payload sanity checks (string bounds, finite location, bed-like descriptor checks).
  - Added authoritative server resolution of bed actor before persisting replicated respawn fields.
  - Added dedicated security telemetry category for rejected payloads.
- Tested On: Compile validation + runtime-path inspection (2026-03-17)

### BUG-2026-03-19-001
- Date: 2026-03-19
- Severity: Medium
- Status: Fixed
- Area: Input / PlayerController
- Issue: `DefaultMappingContext` nulo em clientes durante entrada no mapa em cenário multiplayer.
- Reproduction: Rodar cliente em DS (`-game`) e observar log `[SpellRise] PlayerController: DefaultMappingContext is null.`.
- Expected: MappingContext válido e aplicado para input local.
- Actual: Contexto ausente em runtime em pelo menos parte das sessões de smoke.
- Root Cause: `SetupEnhancedInput` dependia apenas de `DefaultMappingContext` no PlayerController, sem fallback para o IMC definido no Character.
- Fix: Adicionado fallback no `ASpellRisePlayerController` para usar `IMC_Default` do `ASpellRiseCharacterBase` quando `DefaultMappingContext` estiver nulo, com proteção contra `AddMappingContext` duplicado.
- Tested On: Build `SpellRiseEditor Win64 Development` + smoke DS+2 recheck (2026-03-19)
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado (warning removido no recheck)
- Owner: Gameplay/Input

### BUG-2026-03-19-002
- Date: 2026-03-19
- Severity: Medium
- Status: Verified
- Area: Inventory / RPC Ownership
- Issue: RPC `SetAllowedItems_SERVER` em atores de storage é chamada sem owning connection, gerando reject no net driver.
- Reproduction: Entrar em sessão DS com mapa Stylized e observar warnings `No owning connection ... SetAllowedItems_SERVER will not be processed`.
- Expected: RPC owner-only chamada apenas por ator/controlador com conexão proprietária válida.
- Actual: Chamadas são emitidas em contexto sem owner de conexão.
- Root Cause: Fluxo de storage em Blueprint dispara RPC server em ator sem owner de conexão no contexto do cliente.
- Fix:
  - Fluxo de storage em Blueprint ajustado para invocar RPC owner-only apenas por contexto com owning connection válida.
  - Rejeições `No owning connection ... SetAllowedItems_SERVER` não reproduzidas nos smokes atuais.
- Tested On: Smoke DS+2 e DS+2 com lag/loss (2026-03-20)
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Multiplayer/Inventory

### BUG-2026-03-20-003
- Date: 2026-03-20
- Severity: High
- Status: Fixed
- Area: FullLoot / Death Flow
- Issue: Janela de risco em `disconnect/death overlap`: bag podia ser cancelada quando morte entrava com delay de spawn e o personagem era destruído antes do processamento.
- Reproduction: Morrer e desconectar rapidamente durante janela de delay de spawn de bag.
- Expected: Loot processado de forma determinística no servidor mesmo com desconexão imediata após morte.
- Actual: Com delay ativo, processamento dependia da validade do actor morto no callback atrasado.
- Root Cause: `HandleCharacterDeath` agendava processamento atrasado com ponteiro fraco para o personagem morto.
- Fix: `USpellRiseFullLootSubsystem::HandleCharacterDeath` passou a processar imediatamente no servidor, ignorando delay configurado para eliminar janela de overlap.
- Tested On: Build `SpellRiseEditor Win64 Development` + smoke DS+2 e DS+2 lag/loss (2026-03-20)
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Multiplayer/Inventory

### BUG-2026-03-20-004
- Date: 2026-03-20
- Severity: High
- Status: Fixed
- Area: FullLoot / Inventory Transfer Consistency
- Issue: Em falha de remoção no inventário de origem, itens já adicionados na bag podiam permanecer, criando risco de duplicação.
- Reproduction: Forçar cenário onde `AddSlotsToInventory` na bag retorna sucesso parcial/total e `RemoveSlotsFromInventory` na origem falha.
- Expected: Sem duplicação; operação deve manter consistência de transferência.
- Actual: Antes da correção, falha de remove na origem só gerava warning e mantinha itens movidos na bag.
- Root Cause: Fluxo não tratava remove-failure como transação com rollback.
- Fix:
  - Implementado rollback server-side no inventário da bag quando `RemoveSlotsFromInventory` da origem falha após add.
  - `TotalMovedSlots`/`bTransferredAnyItem` só contabilizam quando add + remove da origem concluem com sucesso.
  - Adicionada política explícita de elegibilidade de inventários para death-loot (allowlist/denylist por owner/component) com logs de auditoria.
- Tested On: Build `SpellRiseEditor Win64 Development` + gate smoke DS+2 reconnect normal (`Saved/Logs/SmokeAuto/2026-03-20_14-33-59`)
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Multiplayer/Inventory

### BUG-2026-03-20-005
- Date: 2026-03-20
- Severity: High
- Status: Fixed
- Area: FullLoot / Drop Policy
- Issue: Slots de inventário sem política explícita de exclusão podiam entrar no loot bag mesmo quando deveriam ser não dropáveis (`NoDrop/Bound/Quest/Protected`).
- Reproduction: Matar personagem com itens marcados como protegidos/bound/quest em metadados de slot e observar entrada no fluxo de transferência.
- Expected: Itens não dropáveis bloqueados no servidor antes do move para a bag.
- Actual: Sem filtro server-side dedicado por slot; critério era apenas `Quantity > 0`.
- Root Cause: Ausência de validação de elegibilidade por slot no pipeline de full-loot.
- Fix:
  - Adicionado filtro server-side por reflexão de slot (`IsSlotAllowedForDeathLoot`) no `USpellRiseFullLootSubsystem`.
  - Bloqueio por bool flags e tags/texto com keywords configuráveis (`NoDrop/Bound/Soulbound/Quest/Protected/Locked`), incluindo bools de permissão de drop com valor `false`.
  - Logs de auditoria para cada slot bloqueado.
- Tested On: Build `SpellRiseEditor Win64 Development` + gate smoke DS+2 reconnect normal (`Saved/Logs/SmokeAuto/2026-03-20_14-38-16`).
- Standalone: Pendente
- Listen Server: Pendente
- Dedicated Server: Verificado
- Owner: Multiplayer/Inventory
