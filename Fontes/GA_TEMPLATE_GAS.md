# Template GAS para SpellRise

## 1) Linha mestra (padrão recomendado)
- GA = orquestra (entrada, commit, alvo, timing)
- GE = regra/estado (dano, buff, debuff, duração)
- GameplayCue = visual/audio (instantâneo ou persistente)

Fluxo:
- Ativa GA -> Commit (cost/cooldown)
- Aplica GE (owner/target)
- GE aciona GameplayCue
- Se cue persistente, remove quando GE terminar

## 2) Template A - GA instantânea (ex.: burst)
Uso:
- dano instantâneo
- heal instantâneo
- FX único

Blueprint (K2_ActivateAbility):
1. Validar ActorInfo/Target
2. `K2_CommitAbility` (ou commit cost + cooldown)
3. `BP_ApplyGameplayEffectToOwner/Target` com GE instant
4. Opcional fallback visual: `SR_TriggerGameplayCueReliable(CueTag, Execute)`
5. `K2_EndAbility`

Checklist:
- GE DurationPolicy = Instant
- Cue do GE = event de Execute
- Sem depender de WaitGameplayEvent para aplicar GE principal

## 3) Template B - GA duracional (ex.: ShieldBubble)
Uso:
- buffs com duração
- estado ativo enquanto GE existir

Blueprint (K2_ActivateAbility):
1. `K2_CommitAbility`
2. Aplicar GE duracional (`HasDuration` ou `Infinite`) imediatamente
3. Tocar montage/anim/esperas (opcional)
4. Encerrar GA quando lógica da habilidade terminar

Cue persistente:
- Preferir cue no próprio GE duracional
- Add quando GE entra, Remove quando GE sai

Importante:
- Não amarrar aplicação do GE principal a `WaitGameplayEvent(Event.Montage.Trigger)`
- Esse evento pode falhar por notify/tag e quebrar tudo

## 4) Template C - GA combo (ex.: AxeCombo)
Uso:
- cadeia de hits por janela

Pré-requisitos:
- Montage com notifies/eventos:
  - `Event.ContinueCombo.Start`
  - `Event.ContinueCombo.End`
- Input tag consistente (ex.: `InputTag.Ability.Primary`)
- MappingContext carregado no PlayerController

Blueprint base:
1. `K2_CommitAbility`
2. Iniciar montage de combo
3. `WaitGameplayEvent(Event.ContinueCombo.Start)` -> abre janela
4. `WaitInputPress` durante janela
5. Se pressionou -> avança seção do montage / próximo estágio
6. `WaitGameplayEvent(Event.ContinueCombo.End)` -> fecha janela
7. Se sem input -> finalizar combo
8. `K2_EndAbility`

Checklist anti-quebra:
- Sem evento de notify = combo para no hit 1
- Sem mapping context = input intermitente
- Não usar somente `WaitGameplayEvent` sem fallback de estado

## 5) Convenções para evitar regressão
- Todo GE crítico aplicado direto após commit
- `WaitGameplayEvent` apenas para sincronizar timing/anim
- Cues persistentes guiados por GE duracional
- Log mínimo por GA:
  - ativou
  - commit ok/falhou
  - GE aplicado
  - cue trigger
  - end/cancel

## 6) Padrão de fallback C++ (já disponível)
Em `USpellRiseGameplayAbility`:
- `SR_TriggerGameplayCueReliable(CueTag, TriggerMode)`
  - `Execute` (instant)
  - `Add` (persistente)
  - `Remove` (persistente)

Usar quando o BP estiver instável ou em migração.
