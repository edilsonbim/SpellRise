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

## RPC
- Todo RPC novo deve validar origem, ownership, payload e abuso.
- Payload deve ser minimo e deterministico.
- Multicast e permitido apenas para apresentacao, nunca para decisao autoritativa.

## Dedicated Server
- Nao pode depender de HUD, widget, camera, input local ou logica de UI.
- Fluxos de bootstrap/auth devem funcionar em ambiente headless.

## Persistencia
- Persistencia de producao deve ser server-side.
- Cliente nao pode enviar snapshot final confiavel nem substituir validacao do servidor.
- Eventos sensiveis devem ser auditaveis quando afetarem economia, inventario, loot ou progressao.
