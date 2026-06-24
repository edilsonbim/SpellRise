# SpellRise Agent Instructions

Voce trabalha no projeto SpellRise em `C:\Users\biM\Documents\Unreal Projects\SpellRise`.

Responda em portugues do Brasil, curto, direto e com foco em producao. Trate o repo como multiplayer live service continuo. Economize tokens reduzindo repeticao, contexto e chamadas desnecessarias sem perder rigor, validacao ou qualidade. Se o historico ficar pesado, sugira novo chat com resumo objetivo.

## Fonte de verdade

Antes de alterar arquitetura, gameplay, rede, GAS, atributos, persistencia ou build, consulte nesta ordem: `PROJECT_STATE.md`, `AGENTS.md`, `ARCHITECTURE.md`, `ATTRIBUTE_MATRIX.md`, `MULTIPLAYER_RULES.md`, `SECURITY_MODEL.md`, `COMBAT_DESIGN_PHILOSOPHY.md`, `REFERENCIAS_JOGOS.md`, `BACKLOG.md`, `BUG_LOG.md`, `CHANGELOG.md`, `ENGINEERING_PLAYBOOK.md`.

Se codigo legado conflitar com documentacao atual, sinalize e siga os documentos. Nao invente classe, asset, tag, API, funcao, fluxo ou regra sem verificar equivalente existente.

## Contexto fixo

- Unreal Engine 5.7.
- Dedicated Server obrigatorio.
- Hardcore PvP MMORPG, Full Loot, Free Target, combate Skill-Based.
- Sem auto-aim, sem tab target, combate server-authoritative.

## Build e validacao

- Use apenas `C:\UnrealSource\UnrealEngine` para build, smoke ou validacao final.
- Se eu pedir build sem target explicito, use `SpellRiseEditor Win64 Development`.
- Comando permitido: `C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat SpellRiseEditor Win64 Development "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" -WaitMutex -NoHotReloadFromIDE`
- Nao use engine Launcher, MSBuild, `.sln`, `.vcxproj`, Visual Studio ou Rider como validacao.
- Nao execute Clean, apague `Binaries`, `Intermediate`, `DerivedDataCache` ou recompile engine sem pedido explicito.
- Se o usuario pedir apenas edicao, nao rode build. Nunca afirme validacao sem executar e ver resultado.

## Arquitetura obrigatoria

- GAS autoritativo no PlayerState: ASC e AttributeSets vivem nele; Character consome ASC/ActorInfo do PlayerState.
- ASC usa replication mode Mixed.
- Primarios canonicos: STR, AGI, INT, WIS. VIG/FOC apenas compatibilidade passiva; novo codigo, asset, GE, tag, UI ou formula nao deve depender deles.
- Cliente nunca decide dano, recursos, atributos, morte, loot, persistencia, custo, cooldown, hit final ou resultado de gameplay.
- Dedicated Server nao pode depender de HUD, widget, camera, input local ou UI.

## Server vs Client

Server resolve dano, custo, cooldown, recursos, atributos, morte, loot, persistencia, respawn, fall damage, catalyst e mutacao de estado; valida RPC/payload/contexto/ownership/rate-limit/anti-spam; commita abilities; valida target data; spawna projeteis replicados; aplica GE; decide transicoes.

Client coleta input, pode fazer aim trace local como intencao, usa prediction apenas para UX e exibe UI/VFX/SFX. Nunca decide resultado autoritativo.

## GAS, combate e projetil

Respeite a arquitetura GAS atual e separe ativacao, target data, commit, aplicacao de GE, cue/apresentacao replicada, cosmetico local e cleanup.

Fluxo obrigatorio de projetil: `aim local -> target data -> validacao server -> commit -> spawn replicado -> hit/GE no servidor`.

Ao alterar abilities, informe onde inicia, commita, valida no servidor, preve no cliente e replica. Ao alterar atributos, informe impacto em AttributeSet, MMC, ExecCalc, GameplayEffect, UI e replicacao.

## Rede, PlayerController e RPC

- Nunca use multicast para decisao autoritativa; multicast e apenas apresentacao.
- Ability code nao deve depender so de `HasAuthority()` para ativacao.
- OnRep e reconciliacao/apresentacao, nunca decisao final.
- Evite replicacao desnecessaria, Tick desnecessario e payload excessivo.
- PlayerController e camada de input, UX local e ponte de RPC validada; nao armazena estado autoritativo, nao replica cosmetico, nao decide gameplay e nao depende de HUD/widget/camera em fluxo de DS.
- Toda nova replicacao no PlayerController exige justificativa, alternativa considerada, condicao de replica, budget e risco de `FBitReader::SetOverflowed`.

Todo RPC novo ou alterado deve declarar: origem permitida, owner esperado, payload maximo, limites por campo, validacao de contexto/ownership, rate-limit, comportamento em rejeicao, categoria de log e por que precisa existir se afetar gameplay. RPC autoritativo sem validacao de contexto, ownership e abuso e defeito.

Mudanca net-critical deve informar: atores/propriedades replicadas, condicao de replicacao, NetUpdateFrequency, RPC/s por jogador, payload aproximado, target data quando aplicavel, risco de burst, risco de OnRep fora de ordem e impacto em DS+2 normal e com lag/loss.

## Engenharia

- Priorize C++; use Blueprint apenas para UI, animacao, VFX, assets, data assets e configuracao.
- Preserve nomes, arquitetura e convencoes salvo defeito real.
- Prefira data-driven quando nao enfraquecer autoridade/arquitetura.
- Evite abstracao precoce e solucao estrutural generica em Blueprint.
- Codigo deve ser claro e compile-ready.
- Nao remova replicacao existente sem justificativa.
- Variavel replicada exige UPROPERTY adequada, GetLifetimeReplicatedProps, condicao de replica e OnRep quando necessario.
- Use categorias explicitas de log; evite LogTemp em fluxo importante.
- Nao deixe debug FX/logs ruidosos/caminhos editor-only em runtime shipping.

## Debug e observabilidade

Em debug, responda: causa mais provavel, correcao exata, risco de replicacao, testes em Standalone/Listen/DS e lag/loss quando tocar combate, prediction, RPC, projetil, atributos replicados ou sistema net-critical.

Fluxos criticos devem registrar quando aplicavel: RPC rejeitado e motivo, activation fail/FailureTags, commit/cost/cooldown negado, target data rejeitado, overflow de replicacao, falha de auth/session, persistencia recusada/reconciliada/salva.

## Gate e live ops

- C++ simples sem rede/GAS: compile quando build for pedida.
- UCLASS/UFUNCTION/UPROPERTY: validar SpellRiseEditor com UHT.
- GAS/Ability/GE/ExecCalc/MMC: revisar activation, commit, GE, prediction e replicacao.
- Projetil/combate/prediction e PlayerController/RPC/replicacao: validar DS+2 normal e lag/loss quando possivel, budget e overflow.
- Persistencia/loot/economia: validar server-side, reconciliacao e auditoria.
- UI/VFX/SFX: nao criar dependencia em Dedicated Server.

Mudanca sensivel de gameplay, rede, economia ou persistencia deve considerar default seguro no servidor, rollback, compatibilidade com dados persistidos, flag/config/data asset quando viavel e atualizar CHANGELOG/BACKLOG/BUG_LOG se alterar contrato live.

## Formato quando aplicavel

1. Problema
2. Causa provavel
3. Correcao exata
4. Server
5. Client
6. Riscos de authority / prediction / RPC / OnRep
7. Arquivos alterados ou patch exato
8. Checklist de teste

## Definicao de pronto

Pronto exige: compile-safe, GAS e replicacao respeitados, Server/Client separados, Dedicated Server preservado, sem trust indevido no cliente, riscos de authority/prediction/RPC/OnRep registrados, checklist objetivo e nenhuma validacao alegada sem execucao real.

## Foco ativo

1. Corrigir overflow de replicacao no PlayerController.
2. Fechar bootstrap/auth Steam em Dedicated Server.
3. Fechar persistencia/economia server-side de producao.
4. Consolidar building mode com budget de rede e matriz de RPC.
5. Avancar automacao multiplayer em gate continuo/CI.
