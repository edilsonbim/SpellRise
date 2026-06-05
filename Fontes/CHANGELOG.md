# Changelog

## Unreleased
### Build / Tooling
- Build policy consolidada em Unreal Source.
- Compatibilidade UE 5.7 ajustada em pontos críticos de build.
- Dependência de `CommonUI` declarada corretamente no plugin Narrative.

### Gameplay / Networking
- Combat log autoritativo com transporte nativo em C++.
- Combat log owner-only limitado para reduzir burst de replicação: buffer replicado padrão reduzido para 50 entradas, snapshot inicial limitado às 30 últimas e `ForceNetUpdate` rate-limitado em spam de dano.
- Hardening de RPCs críticos de gameplay e respawn.
- Bootstrap/auth de Dedicated Server agora registra estado do OnlineSubsystem e falha cedo quando Steam e obrigatorio mas o processo degradou para `NULL`.
- Validação server-side para target data de projétil.
- Removido `ManaCostReduction` dos atributos derivados; redução/custo de mana passa a ser responsabilidade de GE/ability level.
- Removido `CastTimeReduction` dos atributos derivados; duração/cast time passa a ser responsabilidade de GE/ability level.
- `MoveSpeed`/`MoveSpeedMultiplier` agora alimentam `CharacterMovement->MaxWalkSpeed` via Character, permitindo slow/freeze autoritativo por GameplayEffect.
- Observabilidade de slow/freeze passa a registrar mudanças reais de `MaxWalkSpeed`; logs de persistência com ID `DEV_` em PIE foram reduzidos para `Verbose`.
- Reward de morte de inimigo passa a usar o maior contribuidor como fallback de `KillerPS` quando o contexto fatal nao resolve PlayerState.
- Clamp dos primários ajustado para `0..120`; `AttributeSet` inicia em `20`, talentos persistidos podem evoluir até `100` e o teto restante é reservado para boosters.
- Contrato de grant GAS ajustado: `FSpellRiseGrantedAbility` não carrega mais level; o level editável fica no Blueprint callable do `CharacterBase`, enquanto grants por source/inimigo usam default server-side seguro.
- Fluxo de death -> full loot -> respawn fechado no recorte atual.
- Full loot da morte ajustado para spawn com delay de 3s e verificação de piso no servidor (evita bag presa no ar).
- Hook determinístico de QA para morte em smoke multiplayer.
- Building mode isolado em componente dedicado no `PlayerController`.
- Gate multiplayer validado em `DS+2` com reconnect + perfis de lag/loss A (120/1) e B (180/3), sem overflow de replicação no run baseline `2026-04-02_21-03-10`.

### Persistence / Observability
- Persistence foundation v1 ativa com snapshots de personagem e inventário.
- Snapshot de personagem schema 7 passa a persistir `CharacterCreated` e `VisualConfigurationJson`, com colunas auxiliares no PostgreSQL para checagem rápida de criação de personagem por player.
- Snapshots de personagem/inventário agora exigem SteamID64 real; IDs `DEV_`/`NULL` ficam sem gravação em DB e recebem defaults runtime.
- `PreLogin` passa a consultar denylist opcional por IP em `spellrise_portal_ip_bans`, além do ban existente por `steam_id64`.
- Snapshot de personagem evoluído para persistir talentos e pontos disponíveis; talentos deixam de depender de grant padrão em login.
- Persistência de talentos endurecida por conta SteamId, com nível de talento salvo/reaplicado clampado em `1..100`.
- Eventos de morte passam a ser persistidos em `spellrise_death_events`, com vítima, data/hora, maior causador de dano, golpe fatal, causa e mensagem auditável server-side.
- Combat log durável owner-centric em `PlayerState`.
- Gate local de smoke multiplayer com reconnect e lag/loss.

### Known Active Risks
- bootstrap/auth Steam em DS pendente de build/link e validação Steam real;
- persistência de produção ainda incompleta;
- contrato final de rede do building mode ainda pendente.

### Inventory / Equipment / Drop (2026-04-06)
- Fluxo de equip/unequip via inventory + quick slots consolidado com `SpellRiseEquipmentManagerComponent`.
- Ajustes de sync visual entre preview (`WBP_PlayerPreview`) e estado de quick slots.
- Implementado caminho C++ para request de drop com limpeza de vínculos de equipamento antes da remoção do item.
- Incluídas tentativas de robustez para spawn de pickup no servidor (fallback de classe + inicialização refletida).
- Pendente: validação final do spawn do pickup em cliente DS no fluxo de drop (ver BUG-2026-04-06-035).
