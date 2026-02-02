# SpellRise

> PvP skill-based com magias de risco/recompensa, cast com interrupção e combate rápido  
> Inspirado em **Darkfall Unholy Wars**

![SpellRise Banner](Docs/Images/banner.png)

---

## Visão Geral

**SpellRise** é um projeto em desenvolvimento na **Unreal Engine** focado em combate PvP técnico, rápido e responsivo.  
O jogo prioriza **habilidade do jogador**, leitura de combate e tomada de risco, evitando sistemas automáticos ou dependência excessiva de números.

O projeto usa o **Gameplay Ability System (GAS)** como base para:
- Atributos
- Dano e mitigação
- Estados de personagem
- Cast, interrupções e cooldowns
- Progressão futura

> ⚠️ Projeto em **desenvolvimento ativo**. O foco atual é consolidar o **core combat** antes de expandir conteúdo.

---

## Pillars de Gameplay

- 🎯 **Skill > Gear**
- 🔮 Magias com **cast real**, risco e recompensa
- ❌ Cast pode ser **interrompido**
- ⚔️ Melee simples, fluido e responsivo
- 🧠 Decisão, posicionamento e timing importam mais que builds automáticas

---

## Estado Atual do Projeto (Progress)

### ✅ Combate & GAS (Core)

- **Sistema de Dano via ExecCalc (GAS)**
  - ExecCalc dedicado (`ExecCalc_Damage`)
  - Mitigação por:
    - Armor (diminishing returns)
    - Resistências mágicas (com cap)
  - Dano aplicado via **meta attribute** (`Damage`)
  - Compatível com melee, magia e ranged

- **Sistema de Cast (Hold-to-Release)**
  - Segurar input → inicia cast
  - Soltar após completar → dispara imediatamente
  - Soltar antes de completar → **não cancela**, dispara ao completar
  - Cancelamento só ocorre por:
    - Ability dedicada de cancel
    - Interrupção externa (evento)

- **Cancel Cast Manual**
  - Gameplay Ability dedicada
  - Cancela qualquer habilidade com a tag `State.Casting`

- **Interrupção de Cast (PvP)**
  - Dano envia `Event.Interrupt.Cast`
  - Abilities castáveis escutam o evento
  - Cast é cancelado corretamente via `CancelAbility()`

---

### ✅ Sistema Catalyst (Proc / Progressão de Combate)

- AttributeSet dedicado:
  - `CatalystCharge (0–100)`
  - `CatalystXP`
  - `CatalystLevel (1–3)`
- Charge:
  - Acumula **ao causar dano real**
  - Não acumula se:
    - Alvo estiver morto (`State.Dead`)
    - Proc já estiver ativo (`State.Catalyst.Active`)
- Proc:
  - Ativa automaticamente ao atingir 100
  - Zera charge
  - Aplica buff temporário via GameplayEffect
- Totalmente **server-authoritative** (sem duplicação/predição)

---

### ✅ Regeneração de Recursos

- Health / Mana / Stamina:
  - Regens periódicos via GameplayEffect
  - Ativos o tempo todo (inclusive em combate)
  - Baseados em atributos (`HealthRegen`, etc.)

---

## Em Desenvolvimento (Próximos Passos)

- 🗡️ **Dano básico por arma**
  - Melee (cast oculto)
  - Staff (cast com barra)
  - Bow (cast com barra)
- 📊 **Widget de Cast (UI)**
  - Barra de progresso sincronizada com Ability
- 🔁 **Canalizações**
  - Dreno periódico de recurso
  - Heal / Beam / Buffs contínuos
- 🧪 Balance inicial (TTK curto, PvP rápido)
- 🌐 Multiplayer dedicado (em andamento)

---

## Tech Stack

- **Unreal Engine 5.7.x**
- **C++ + Blueprint**
- Gameplay Ability System (GAS)
- Enhanced Input
- Multiplayer (listen + dedicated server)

---

## Requisitos

- Windows 10 / 11
- Unreal Engine **5.7.x**
- Visual Studio 2022 **ou** Rider configurado para UE
- Git (recomendado)

---

## Como Rodar (Desenvolvimento)

```bash
git clone https://github.com/<seu-user>/<seu-repo>.git
cd SpellRise
