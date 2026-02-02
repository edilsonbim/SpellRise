# SpellRise

> PvP skill-based com magias de risco/recompensa, cast com interrupção e combate rápido (inspirado em Darkfall Unholy Wars).

![SpellRise Banner](Docs/Images/banner.png)

## Visão geral

**SpellRise** é um projeto em **Unreal Engine** focado em combate PvP técnico e responsivo, com ênfase em:
- Magias com **cast**, posicionamento e risco
- **Interrupção de cast** como parte central do meta
- Melee mais simples e fluido (sem ataques direcionais)
- **Gameplay Ability System (GAS)** como base para atributos, habilidades, dano, estados e progressão

## Principais features (estado atual)

✅ **Sistema de Cast (hold-to-release)**  
- Segura o botão → inicia cast  
- Cast completa → dispara ao soltar  
- Se soltar antes, **não cancela**: dispara automaticamente ao completar  
- Cancelamento só por **GA dedicada** ou **interrupção por evento**

✅ **Cancel Cast manual**  
- Ability dedicada que cancela habilidades com `State.Casting`

✅ **Interrupção por dano (PvP)**  
- Dano envia `Event.Interrupt.Cast`  
- Abilities castáveis escutam o evento e chamam `CancelAbility()`

✅ **Sistema de dano por ExecCalc (GAS)**  
- `ExecCalc_Damage` calcula dano final  
- Mitigação por Armor (diminishing returns) e Resistências (cap)  
- Dano aplicado via **meta attribute** (`Damage`) e consumido no AttributeSet

## Tech Stack

- **Unreal Engine** (projeto C++/Blueprint)
- **Gameplay Ability System (GAS)**
- Enhanced Input
- Multiplayer (planejado/ativo conforme branch)

## Requisitos

- Windows 10/11 (setup principal)
- Unreal Engine **5.7.x** (ajuste se seu projeto estiver em outra versão)
- Visual Studio 2022 (C++ build tools) **ou** Rider + toolchain configurado
- Git (opcional, mas recomendado)

## Como rodar (desenvolvimento)

1) Clone o repositório:
```bash
git clone https://github.com/<seu-user>/<seu-repo>.git
cd SpellRise
