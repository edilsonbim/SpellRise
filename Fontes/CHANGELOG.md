# Changelog

## Unreleased
### Build / Tooling
- Build policy consolidada em Unreal Source.
- Adicionado plugin Editor-only `CodexBlueprintBridge` para inspecao segura de Blueprints via commandlet JSON; validado com `ListBlueprints` sem alteracao de assets.
- Compatibilidade UE 5.7 ajustada em pontos críticos de build.
- Dependência de `CommonUI` declarada corretamente no plugin Narrative.

### Gameplay / Networking
- `ExecCalc_Damage` passa a aplicar escala final por atributo primario depois de arma/escola e `Data.DamageScaling`: STR para melee, AGI para bow, INT para spell e WIS para divine. `ExecCalc_Healing` passa a aplicar a mesma escala final por WIS.
- `ExecCalc_Damage` passa a escalar dano da ability por nivel da escola e dano da arma por nivel da arma, cada um com 50% fixo + ate 50% por progressao; `AbilityLevel` deixa de afetar dano e permanece restrito a custo/cooldown. O debug `sr.Damage.Debug` agora registra as contribuicoes separadas de ability e arma.
- `USpellRiseProgressionComponent` passa a expor `EnsureSchoolLevelFromTalent_Server` e `EnsureProgressionLevelFromAbilityDefinitionTalent_Server`, permitindo que talentos de escola como `DA_Talent_Fire` atualizem `SchoolLevels` autoritativos no `PlayerState` em vez de cair no fluxo de arma com tag vazia.
- Implementado fluxo player-only de `State.Downed`: ao zerar vida, o player agoniza por 60s com vida travada em 1, sem input/abilities/regen; morte final, full loot e respawn foram movidos para `FinalizeDeath_Server`.
- Adicionado `USpellRiseDownedInteractableComponent` para interações Narrative de revive/finish em alvo agonizando; revive server-side retorna com `20% MaxHealth`, `0 Mana` e `0 Stamina`.
- `USpellRiseDeathScreenWidget` passa a expor `RequestAcceptDeath`, permitindo UI chamar `ServerAcceptDeath` enquanto o player está em `State.Downed`.
- Ragdoll, fade/camera e tela de morte foram suprimidos temporariamente no fluxo de morte/downed para testar a mecanica server-side limpa.
- Adicionado atributo `EquippedWeaponBaseDamage` em `UCombatAttributeSet` para o dano base da arma equipada, alimentado por GE de equipamento via `Data.EquippedWeaponBaseDamage`.
- `ExecCalc_Damage` passa a consumir `EquippedWeaponBaseDamage`, `AbilityLevel`, nivel de arma e nivel de escola quando a ability declara os metadados de progressao.
- `ExecCalc_Damage` passa a aceitar `USpellRiseProgressionComponent` opcional em actors de AI, configuravel por Blueprint, sem exigir o componente em `EnemyBase`.
- Progressao de arma passa a usar `Progression.Weapon.TwoHandSword` no lugar de `Progression.Weapon.Katana`; o mapeamento C++ ainda aceita tag legada `.Katana` como compatibilidade temporaria para assets antigos.
- `USpellRiseGameplayAbility` passa a declarar metadados de dano/progressao (`DamageChannelTag`, `DamageTypeTag`, `SchoolProgressionTag`, `WeaponProgressionTag`, `bUsesEquippedWeaponDamage`) e remove os campos antigos de requisito/bloqueio por `WeaponTag`.
- Adicionado `USpellRiseAbilityDefinition` como data asset C++ tipado para consolidar dados editaveis de UI/hotbar/grants/effects/progressao de talents e abilities; hotbar passa a ler a classe tipada primeiro e preserva fallback legado por reflection. `DefinitionType` e `DisplayName` passam a ser os campos finais para tipo técnico e nome exibido.
- Adicionadas tags de progressao `Progression.Weapon.*` e `Progression.School.*` para o novo componente de progressao no `PlayerState`.
- Adicionado `USpellRiseProgressionComponent` no `ASpellRisePlayerState` para armazenar niveis owner-only de arma e escola por `GameplayTag`.
- `ASpellRisePlayerState` passa a expor `GetProgressionComponent` para Blueprint, e `USpellRiseProgressionComponent` passa a expor `EnsureWeaponSkillLevelFromTalent_Server` para o `TalentTreeComponent` BP vincular talentos de arma aos niveis autoritativos de progressao.
- Removido `HealingMultiplier` do runtime C++; cura passa a depender de base/scaling explicito e futura progressao de escola/GA.
- Removidos multiplicadores derivados de dano por canal (`MeleeDamageMultiplier`, `BowDamageMultiplier`, `SpellDamageMultiplier`) do runtime C++ para abrir caminho ao modelo baseado em nivel da GA, arma equipada, nivel de arma e escola.
- Adicionado suporte base de lifesteal server-authoritative: `LifestealPercent` em `UCombatAttributeSet`, `Status.Lifesteal` para estado/UX e cura pós-dano real via `GE_Lifesteal_Healing`.
- Adicionado contrato de cura GAS server-authoritative: `ExecCalc_Healing` calcula cura por `Data.BaseHeal`/`Data.Heal` e `Data.HealingScaling`; `UResourceAttributeSet` consome meta `Healing` e aplica clamp em `Health <= MaxHealth`.
- Corrigido input da ability hotbar para respeitar `USpellRiseGameplayAbility::bFireOnAbilityInput`: slots que apontam direto para `AbilityClass` agora apenas armam abilities configuradas para disparar pelo `Primary`, em vez de ativá-las imediatamente.
- Documentado recorte de correcoes reportadas em 2026-06-14: inventario/vendor/loot fechando aceitavelmente, barra de ability ao morrer, regen de atributos, animacao equip/unequip, camera sem arma, AoE sem dano, `Blizzard` nao entrando mais no solo e `shoot arrow` corrigido. Validacao formal ainda pendente quando nao houver evidencia de build/smoke registrada.
- Registrados como pendencias ativas: sockets, melhoria de ragdoll, fluxo `dead`/`revive` e revisao de luzes.
- Movido o contrato de grant/remove/evento de abilities de player para `ASpellRisePlayerState`, mantendo o `Character` apenas como avatar do ASC.
- Grants persistentes/startup passam a usar source estável no `PlayerState`; grants de equipamento continuam usando item/instância como source.
- Inventario de player movido para `ASpellRisePlayerState`; resolucao de inventario para pawn/controller passa a priorizar o `PlayerState`, e full loot/persistencia/UI deixam de coletar inventario do `Character` como fonte de verdade.
- Interacao Narrative instantanea em Dedicated Server passa a completar no proprio `BeginInteract` apos trace/`CanInteract` server-side; hold-interact reativa tick no DS apenas enquanto necessario. Corrige containers/loot que nao abriam porque `SetLootSource` nunca era executado no ramo autoritativo.
- `UInventoryWidget` passa a capturar foco de teclado, tratar ESC nativamente, chamar `StopLooting` no inventario do owning player e restaurar input `GameOnly` ao fechar inventory/loot; `ASpellRisePlayerController` tambem fecha looting via ESC/clear antes do bloqueio de gameplay, cobrindo BP/widget sem foco confiavel.
- Auditoria P0 de performance DS 100: tick local do `CharacterBase` passa a ligar somente para pawn local vivo; Dedicated Server não força refresh de bones/AnimBP por padrão; trace contínuo do `NarrativeInteractionComponent` foi desativado no DS com validação sob demanda no RPC de interação.
- Chat público deixa de usar multicast reliable; RPC genérico de gameplay event passa a rejeitar target data, objetos opcionais, context handle e tag containers enviados pelo cliente.
- Multicasts visuais de morte, corpse despawn e refresh visual de equipment deixam de usar reliable; a decisão autoritativa continua em GE/tag/estado do servidor.
- Bootstrap NoSteam em escala local passa a manter pending persistent id por endereco de conexao com porta, evitando colisao entre varios clientes `127.0.0.1`.
- `Run-Load-NoSteam-Scale.ps1` passa a bloquear cargas locais acima de 12 clientes sem `-AllowHighLocalClientCount`, evitando saturar RAM/pagefile da maquina de desenvolvimento durante auditoria.
- `Run-Load-NoSteam-Scale.ps1` passa a usar `-nosound` e a contar apenas timeouts reais de rede, evitando falso positivo causado pelo parametro `ConnectionTimeout`.
- `Run-Load-NoSteam-Scale.ps1` passa a enviar `InitialConnectTimeout` e `ConnectionTimeout` tambem na URL de conexao do cliente, que e o caminho lido pelo `PendingNetDriver`.
- `Run-Load-NoSteam-Scale.ps1` passa a suportar `-UseProjectBinaries` para usar `SpellRiseServer.exe`/`SpellRiseClient.exe` em vez de `UnrealEditor-Cmd`, reduzindo custo local da carga.
- Adicionado `Scripts/Build-Cooked-Load-NoSteam.ps1` para preparar build staged/cooked usado por carga com binarios de projeto.
- `Scripts/Build-Cooked-Load-NoSteam.ps1` passa a gerar client e server em etapas separadas (`-TargetSet Client|Server|Both`), garantindo que `SpellRiseServer.exe` entre no archive/stage.
- Primeiro `DS+2` com binarios staged/cooked conectou 2/2 sem timeout e sem `FBitReader::SetOverflowed`; o run inicial ficou bloqueado por ensure client-side de Gameplay Cameras e warnings Blueprint de `DLWE_Interaction`.
- Gate `DS+2` staged/cooked revalidado apos ajustes de assets: `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26` passou com 2/2 clientes, `OverallUnstable=False`, zero ensure, zero timeout, zero overflow, sem erro `FocalLength/FieldOfView` e sem spam `DLWE_Interaction`.
- Gate staged/cooked com 12 clientes (`Saved\Logs\LoadNoSteam\2026-06-10_21-02-52`) conectou 12/12 com zero timeout, zero prelogin/network failure e zero overflow, mas reabriu o bloqueio de Gameplay Cameras por 8 ensures `FocalLength/FieldOfView`.
- `Run-Load-NoSteam-Scale.ps1` passa a desligar Gameplay Cameras por padrao apenas em clientes sinteticos `-unattended/-nullrhi`; usar `-EnableGameplayCameras` para reproduzir o fluxo visual real.
- `ASpellRisePlayerController` e `ASpellRiseCharacterBase` passam a respeitar `-SpellRiseSkipLocalHUDFlow` para carga headless, evitando parte do fluxo visual local; `Saved\Logs\LoadNoSteam\2026-06-10_21-38-14` ainda falha por 4 ensures no `CameraRigAsset_2147482084`, entao a correcao definitiva continua no asset/runtime de camera.
- Ajuste de CameraAsset/CameraDirector removeu o bloqueio de Gameplay Cameras no gate: `Saved\Logs\LoadNoSteam\2026-06-11_08-49-18` passou com 12/12 clientes, zero ensure, zero timeout e zero overflow; warnings residuais `FocalLength/FieldOfView` sem ensure permanecem como follow-up visual.
- Gate staged/cooked local escalado para 16 clientes em `Saved\Logs\LoadNoSteam\2026-06-11_09-42-59`: 16/16 conectados, `OverallUnstable=False`, zero ensure, zero timeout, zero prelogin/network failure e zero overflow; warnings de pose mista de camera seguem como divida visual.
- `Run-Load-NoSteam-Scale.ps1` passa a aplicar emulacao de rede via `NetEmulation.PktLag/PktLoss` e argumentos `PktLag/PktLoss`; Perfil A `DS+16` em `Saved\Logs\LoadNoSteam\2026-06-11_09-55-56` passou com `LagLossApplied=True`, 16/16 conectados, zero ensure, timeout e overflow.
- Perfil B `DS+16` em `Saved\Logs\LoadNoSteam\2026-06-11_10-00-58` passou com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, 16/16 conectados, zero ensure, timeout, prelogin/network failure e overflow.
- `Run-Load-NoSteam-Scale.ps1` passa a iniciar o servidor de carga com `?MaxPlayers=100` por padrao (`-LoadTestMaxPlayers`), evitando falso bloqueio `Server full.` em cenarios acima de 16 clientes; `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_10-17-12` passou com 24/24 conectados, zero ensure, timeout, network failure e overflow.
- Perfil A `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_10-30-45` passou com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, 24/24 conectados, zero ensure, timeout, prelogin/network failure, `Server full.` e overflow.
- Perfil B `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_11-05-33` passou com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, 24/24 conectados, zero ensure, timeout, prelogin/network failure, `Server full.` e overflow.
- Cleanup do `Run-Load-NoSteam-Scale.ps1` passa a aguardar encerramento por `ProcessExitGraceSeconds` e usar `taskkill /PID /T /F` como fallback por arvore de processo, evitando clientes cooked residuais apos rodadas pesadas.
- Atributos privados de recurso (`Mana`, `Stamina`, regens e `CarryWeight`) passam a replicar `OwnerOnly`; `Health`/`MaxHealth` permanecem públicos.
- RPCs de inventário Narrative receberam rate-limit server-side e limite de quantidade por chamada, com log `InventoryRpcRejected`.
- `Run-Load-NoSteam-Scale.ps1` passa a suportar `-WithInsightsTrace` e `-WithPerfStats` para gerar `.utrace` e coletar stats de rede/gameplay nos cenários de escala.
- Base C++ da hotbar de abilities 8+8 adicionada: slots `0-7` para weapon abilities e `8-15` para common abilities em componente owner-only no `PlayerState`, com RPC server-side validado/rate-limitado para edição de slots.
- `USpellRiseGameplayAbility` passou a suportar validacao C++ de arma por tags; contrato atual usa `WeaponProgressionTag`.
- `USpellRiseWeaponComponent` remove animação/notify de equip/unequip e visual stowed; apenas arma equipada fica visível, anexada ao socket equipado, priorizando `VisualOverride` válido sem modificar o mesh.
- Combat log autoritativo com transporte nativo em C++.
- Fall damage agora resolve causer/instigator como causa ambiental determinística quando o GE é aplicado pelo ASC do próprio alvo, evitando self-causer em chat/combat log/death event.
- Combat log owner-only limitado para reduzir burst de replicação: buffer replicado padrão reduzido para 50 entradas, snapshot inicial limitado às 30 últimas e `ForceNetUpdate` rate-limitado em spam de dano.
- Hardening de RPCs críticos de gameplay e respawn.
- Bootstrap/auth de Dedicated Server agora registra estado do OnlineSubsystem, registra `SteamAuthEnabled`, falha cedo quando Steam e obrigatorio mas o processo degradou para `NULL`, e exige o packet handler `OnlineSubsystemSteam.SteamAuthComponentModuleInterface` para autenticar ticket Steam no handshake.
- Validação server-side para target data de projétil.
- Removido `ManaCostReduction` dos atributos derivados; redução/custo de mana passa a ser responsabilidade de GE/ability level.
- Removido `CastTimeReduction` dos atributos derivados; duração/cast time passa a ser responsabilidade de GE/ability level.
- `MoveSpeed`/`MoveSpeedMultiplier` agora alimentam `CharacterMovement->MaxWalkSpeed` via Character, permitindo slow/freeze autoritativo por GameplayEffect.
- Observabilidade de slow/freeze passa a registrar mudanças reais de `MaxWalkSpeed`; logs de persistência com ID `DEV_` em PIE foram reduzidos para `Verbose`.
- Reward de morte de inimigo passa a usar o maior contribuidor como fallback de `KillerPS` quando o contexto fatal nao resolve PlayerState.
- Clamp dos primários ajustado para `0..120`; `AttributeSet` inicia em `20`, talentos persistidos podem evoluir até `100` e o teto restante é reservado para boosters.
- Regen de `Health/Mana/Stamina` passa a usar atributos finais modificados por GE server-side, com tick autoritativo de 2s no `Character`, aplicação idempotente dos GEs de bootstrap, bloqueio por morte/sangramento, multiplicadores de combate, pausa de stamina por ação e penalidade de mana por debuff.
- Regen de bootstrap agora zera os atributos `HealthRegen/ManaRegen/StaminaRegen` antes de aplicar GE instant/infinite e o tick de 2s aplica o valor do atributo uma vez por tick, evitando acúmulo e escala indevida por segundo.
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
