# SpellRise Agent Instructions

Voce esta trabalhando no projeto SpellRise.

Trate este repositorio como codigo de producao continua de um multiplayer live, nao como tarefas isoladas.

## Fonte de verdade
Siga esta ordem:
1. `PROJECT_STATE.md`
2. `AGENTS.md`
3. `ARCHITECTURE.md`
4. `ATTRIBUTE_MATRIX.md`
5. `MULTIPLAYER_RULES.md`
6. `SECURITY_MODEL.md`
7. `COMBAT_DESIGN_PHILOSOPHY.md`
8. `REFERENCIAS_JOGOS.md`
9. `BACKLOG.md`
10. `BUG_LOG.md`
11. `CHANGELOG.md`
12. `ENGINEERING_PLAYBOOK.md`

Se houver conflito entre codigo legado e documentacao atual, sinalize o conflito e siga os documentos.

## Contexto do projeto
- Unreal Engine 5.7.
- Dedicated Server obrigatorio.
- Hardcore PvP MMORPG.
- Full Loot.
- Free Target.
- Combate Skill-Based.
- Sem auto-aim.
- Sem tab target.
- Combate server-authoritative.

## Verdades tecnicas obrigatorias
- O owner autoritativo do GAS e o PlayerState.
- ASC e AttributeSets vivem no PlayerState.
- O Character consome ASC/ActorInfo do PlayerState.
- O ASC usa replication mode Mixed.
- Primarios validos: STR, AGI, INT, WIS.
- Novo codigo e novos assets nao devem usar aliases legados como VIG e FOC.
- O cliente nunca e autoridade para dano, recursos, atributos primarios, morte, loot ou resultado final de gameplay.
- Dedicated server nao pode depender de HUD, widget, camera ou logica de UI.

## Regra fixa de build
- Quando o usuario pedir para rodar build, interpretar como build do target `SpellRiseEditor`, nao como build da engine.
- Nunca usar `MSBuild`, `SpellRise.sln`, `Intermediate/ProjectFiles/SpellRise.vcxproj` ou build pelo botao do Visual Studio/Rider como validacao do agente.
- O comando permitido para build de editor e:
  `C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat SpellRiseEditor Win64 Development "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" -WaitMutex -NoHotReloadFromIDE`
- Nao adicionar target manual de `ShaderCompileWorker`.
- Nao executar `Clean`, apagar `Binaries`, `Intermediate`, `DerivedDataCache` ou disparar rebuild da engine sem pedido explicito.
- Se o usuario disser para apenas editar arquivos, nao rodar build.

## Padrao de engenharia
- Responder em portugues do Brasil.
- Responder em passos curtos e diretos.
- Priorizar C++.
- Usar Blueprint apenas para UI, animacao, VFX, assets, data assets e configuracao.
- Preservar nomes, arquitetura, convencoes e fluxo existentes salvo defeito real.
- Nao afirmar build, teste ou validacao sem ter verificado.

## Server vs Client
### Server
- resolve dano, custo, cooldown, recurso, morte, loot e mutacao de estado;
- valida RPC, payload, contexto, ownership e anti-spam;
- commita abilities autoritativas;
- valida e consome target data;
- spawna projeteis replicados;
- aplica GameplayEffects autoritativos;
- decide transicoes de estado de gameplay.

### Client
- coleta input;
- faz prediction apenas para UX;
- pode fazer aim trace local como entrada;
- exibe UI, VFX, SFX e feedback local;
- nunca decide dano final, hit final, custo, cooldown, morte ou loot.

## Regras de rede
- Nunca usar multicast para decisao autoritativa de gameplay.
- Multicast e apenas apresentacao.
- Ability code nao deve depender apenas de `HasAuthority()` para ativacao.
- Separar claramente previsao local, confirmacao do servidor e reconciliacao.
- Evitar replicacao desnecessaria, Tick desnecessario e payload excessivo.

## GAS
- Respeitar a arquitetura atual baseada em GAS.
- Separar claramente ativacao, target data, commit, aplicacao de GE, cue/apresentacao replicada, cosmetico local e cleanup.
- Ao alterar abilities, informar onde inicia, commita, valida no servidor, preve no cliente e replica.
- Ao alterar atributos, informar impacto em AttributeSet, MMC, ExecCalc, GameplayEffect, UI e replicacao.

## Definicao de pronto
So considerar pronto quando:
- estiver compile-safe;
- respeitar GAS;
- respeitar replicacao;
- separar claramente Server vs Client;
- nao quebrar dedicated server;
- nao criar trust indevido no cliente;
- registrar riscos de authority, prediction, RPC e OnRep;
- incluir checklist objetivo de teste.
