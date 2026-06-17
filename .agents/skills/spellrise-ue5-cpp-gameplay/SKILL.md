---
name: spellrise-ue5-cpp-gameplay
description: Use para implementar ou revisar C++ gameplay em SpellRise/UE5.7, incluindo Actor, Component, DataAsset, GAS-adjacent logic, UPROPERTY/UFUNCTION, GameplayTags e codigo runtime que precisa respeitar Dedicated Server e authority.
---

# SpellRise UE5 C++ Gameplay

Baseado nos padroes uteis do pack publico `UnrealXu/UnrealEngine5-Skills`, mas subordinado ao `AGENTS.md` e aos documentos fonte do SpellRise.

## Antes de agir
- Consulte, nesta ordem: `PROJECT_STATE.md`, `AGENTS.md`, `Fontes/ARCHITECTURE.md`, `Fontes/ATTRIBUTE_MATRIX.md`, `Fontes/MULTIPLAYER_RULES.md`, `Fontes/SECURITY_MODEL.md`.
- Verifique se ja existe classe, asset, tag, componente, API ou fluxo equivalente no projeto antes de criar algo novo.
- Priorize C++; use Blueprint apenas para UI, animacao, VFX, assets, data assets e configuracao.

## Contrato de implementacao
- Declare o tipo alvo: `AActor`, `UActorComponent`, `UObject`, `UDataAsset`, `UGameplayAbility`, `UAttributeSet` ou outro.
- Entregue `.h` e `.cpp` pareados quando criar classe C++.
- Use forward declarations em headers e includes concretos no `.cpp`.
- Use `TObjectPtr<>` para referencias `UPROPERTY` a objetos.
- Evite Tick; prefira evento, timer server-side ou fluxo GAS.
- Nao use `LogTemp` em fluxo importante; use categoria explicita existente ou crie uma categoria apropriada.

## Regras SpellRise obrigatorias
- Dedicated Server nao pode depender de HUD, widget, camera, input local ou logica de UI.
- Gameplay autoritativo deve acontecer no servidor.
- Cliente envia intencao e pode prever apenas UX.
- GAS autoritativo vive no `ASpellRisePlayerState`.
- ASC e AttributeSets vivem no PlayerState; Character consome ASC/ActorInfo do PlayerState.
- ASC usa replication mode `Mixed`.
- Primarios validos: `STR`, `AGI`, `INT`, `WIS`.
- Novo codigo nao pode depender de `VIG` ou `FOC`.

## Rede e replicacao
Se tocar rede, responda explicitamente:
- atores replicados afetados;
- propriedades replicadas novas/alteradas;
- condicao de replica;
- RPC/s maximo esperado por jogador;
- payload aproximado;
- risco de burst;
- risco de `OnRep` fora de ordem;
- impacto esperado em DS+2 normal e DS+2 com lag/loss.

## Proibido
- Decidir dano, custo, cooldown, recursos, morte, loot, persistencia ou resultado final no cliente.
- Usar multicast para decisao de gameplay.
- Adicionar replicacao no PlayerController sem justificar budget, condicao, alternativa e risco de `FBitReader::SetOverflowed`.
- Criar dependencia runtime de HUD/widget/camera em fluxo que roda no Dedicated Server.
- Criar Blueprint estrutural generico para fluxo que deve ser C++.

## Saida esperada
Quando alterar C++ gameplay, inclua:
1. Problema
2. Correcao exata
3. Server
4. Client
5. Riscos de authority / prediction / RPC / OnRep
6. Arquivos alterados
7. Checklist de teste

