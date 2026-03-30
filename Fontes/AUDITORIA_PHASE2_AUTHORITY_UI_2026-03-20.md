# Auditoria Phase 2 - Authority e Independencia de UI (2026-03-20)

## Escopo
- Fluxo: morte -> full loot -> respawn
- Critérios alvo:
  1. Sem client-authority para morte, ownership de loot e decisão de respawn.
  2. Fluxo de dedicated server sem dependência de HUD/UI/câmera.

## 1) Matriz de autoridade (código)

1. Morte por vida <= 0
- Arquivo: `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp`
- Ponto: `OnHealthChanged`
- Regra: mutação de gameplay de morte só avança com `HasAuthority()` + `AbilitySystemComponent` válido.
- Ação autoritativa: aplica GE/tag de morte, cancela abilities, aciona full-loot, agenda respawn.

2. Spawn e transferência de full-loot
- Arquivo: `Source/SpellRise/Inventory/SpellRiseFullLootSubsystem.cpp`
- Ponto: `HandleCharacterDeath`, `ProcessCharacterDeathNow`
- Regra: só executa em mundo autoritativo (`IsAuthorityWorld`) + `DeadCharacter->HasAuthority()`.
- Ação autoritativa: seleciona inventários elegíveis por política, filtra slots não dropáveis, transfere itens, rollback anti-duplicação em falha.

3. Respawn
- Arquivo: `Source/SpellRise/Characters/SpellRiseCharacterBase.cpp`
- Pontos: `ScheduleRespawn_Server`, `ExecuteRespawn_Server`
- Regra: ambos com guarda explícita `HasAuthority()`.
- Ação autoritativa: server agenda timer e reinicia jogador via `GameMode->RestartPlayer`.

## 2) Independência de UI/HUD em dedicated

1. Guardas explícitas adicionadas
- `TriggerLocalDamageScreenEffect`: retorna em `NM_DedicatedServer`.
- `ShowLocalDeathScreenText`: retorna em `NM_DedicatedServer`.
- `HandleDeath_Implementation`: blocos de câmera/UI local só executam quando `NetMode != NM_DedicatedServer`.

2. Consequência
- Dedicated server mantém apenas lógica autoritativa de combate/loot/respawn.
- Sem necessidade de widget, HUD ou câmera para o fluxo de gameplay crítico.

## 3) Evidência operacional

1. Smoke DS+2 reconnect (normal)
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_14-38-16`
- Resultado: `OverallPass=True`

2. Validação E2E Standalone + Listen
- Summary: `Saved/Logs/SmokeAuto/2026-03-20_14-42-16_Phase2_Standalone_Listen_Summary.txt`
- Resultado: evidências de death/full-loot/respawn presentes e `FatalLikeCount=0`.

## 4) Conclusão

- Critério "No client-authority path for death, loot ownership, or respawn decision": atendido no recorte auditado.
- Critério "Dedicated server flow runs without UI/HUD dependencies": atendido no recorte auditado.
