# Security Model

## Principio central
O servidor e a unica autoridade para dano, custos, cooldowns, recursos, atributos primarios, morte, loot, persistencia e resultado final de gameplay.

## Cliente
- Coleta input.
- Pode prever UX local.
- Pode fazer aim trace local como entrada.
- Exibe UI, VFX, SFX e feedback.
- Nunca decide hit final, dano final, loot, morte, custo, cooldown ou mutacao de estado persistente.

## Servidor
- Valida ownership, payload, contexto, rate/anti-spam e estado de gameplay.
- Consome target data apos validacao.
- Commita abilities autoritativas.
- Spawna projeteis replicados.
- Aplica GameplayEffects autoritativos.
- Resolve morte, loot, respawn, fall damage, catalyst e persistencia.
- Resolve entrada em downed, cooldown anti-chain, conclusão de revive/execução e percentuais de recursos derivados de WIS; cliente não informa duração nem valor restaurado.
- `USpellRiseLifeStateComponent` revalida estado, distância e LOS no servidor mesmo que o cliente tenha aberto o widget para um alvo que depois ficou stale.

## RPC
- Todo RPC novo deve validar origem, ownership, payload e abuso.
- Payload deve ser minimo e deterministico.
- Multicast e permitido apenas para apresentacao, nunca para decisao autoritativa.
- Chat deriva remetente exclusivamente do owner do `PlayerController`; nome declarado pelo cliente não é identidade confiável.
- Whisper valida destinatário, tamanho, rate-limit e block list no servidor; conteúdo privado não deve ser registrado em logs comuns.
- Inventário aceita somente GUID da instância, slot de destino, quantidade limitada e request id; classe, definição, peso, preço, posição de drop e resultado final são derivados no servidor.
- Equipamento revalida ownership, compatibilidade de slot, conflito de arma 2H/OffHand e contexto do ASC. Grants são aplicados/removidos server-side por `ItemInstanceId`, nunca em `OnRep`.
- RPCs legados de inventário no `PlayerController` não fazem parte do contrato novo e não devem receber `UObject*`, arrays ou resultado declarado pelo cliente.

## Dedicated Server
- Nao pode depender de HUD, widget, camera, input local ou logica de UI.
- Fluxos de bootstrap/auth devem funcionar em ambiente headless.

## Persistencia
- Persistencia de producao deve ser server-side.
- Cliente nao pode enviar snapshot final confiavel nem substituir validacao do servidor.
- Eventos sensiveis devem ser auditaveis quando afetarem economia, inventario, loot ou progressao.
- Schemas ativos continuam `14/2`. Contratos `15/3` por GUID e `PrimaryAssetId` permanecem default-off até migração e rollback estarem validados.
- Conversão legacy deve rejeitar item sem mapeamento, registrar equipamento ambíguo e nunca inventar durabilidade/estado ausente sem default controlado pela definição.
- Gasto de `AttributePoints` aceita somente intencao minima do owner (`STR/AGI/INT/WIS` + quantidade); saldo, cap, perfil carregado, rate-limit, mutacao GAS e persistencia sao resolvidos exclusivamente no servidor.
- Compra aceita somente o enum; custo progressivo, saldo e posse sao resolvidos no servidor. Ativacao aceita enum, nivel `1..4` e booleano; o servidor exige ativar o próximo nível ou desativar exatamente o último nível ativo.
- `-SRTestPersistentId=<SteamID64>` pode fixar a identidade apenas em PIE ou quando o modo NoSteam de teste estiver explicitamente habilitado; fora desses contextos o servidor ignora o parâmetro.
