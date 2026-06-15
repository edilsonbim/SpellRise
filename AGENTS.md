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
- Novo codigo e novos assets devem usar apenas primarios canonicos STR, AGI, INT e WIS.
- O cliente nunca e autoridade para dano, recursos, atributos primarios, morte, loot ou resultado final de gameplay.
- O fluxo de projetil deve permanecer: aim local -> target data -> validacao server -> commit -> spawn replicado -> hit/GE no servidor.
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
- Responder por padrao de forma curta, objetiva e bem resumida.
- Falar apenas o basico quando a pergunta for simples.
- Usar respostas maiores somente quando o usuario pedir detalhe, plano, revisao completa ou quando o risco tecnico exigir contexto.
- Responder em passos curtos e diretos quando houver procedimento.
- Priorizar C++.
- Usar Blueprint apenas para UI, animacao, VFX, assets, data assets e configuracao.
- Evitar solucao estrutural generica em Blueprint.
- Preservar nomes, arquitetura, convencoes e fluxo existentes salvo defeito real.
- Nao inventar classe, asset, tag, API ou funcao sem verificar se ja existe equivalente no projeto.
- Preferir solucoes data-driven quando isso nao enfraquecer a arquitetura.
- Evitar abstracao precoce e bases excessivamente genericas.
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
- executa fall damage, catalyst, respawn, loot, persistencia e reconciliacao server-side.

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
- Para mudancas de combate/rede, sempre informar escopo de replicacao, authority, RPC, prediction e risco de OnRep.

## PlayerController
- PlayerController e camada de input, UX local e ponte de RPC validada.
- PlayerController nao deve armazenar estado autoritativo de gameplay.
- PlayerController nao deve replicar payload cosmetico.
- PlayerController nao decide dano, custo, cooldown, loot, morte, persistencia ou resultado final de gameplay.
- Toda nova replicacao no PlayerController exige justificativa de budget, condicao de replica e alternativa considerada.
- Mudancas no PlayerController devem avaliar risco de `FBitReader::SetOverflowed`.

## GAS
- Respeitar a arquitetura atual baseada em GAS.
- Separar claramente ativacao, target data, commit, aplicacao de GE, cue/apresentacao replicada, cosmetico local e cleanup.
- Ao alterar abilities, informar onde inicia, commita, valida no servidor, preve no cliente e replica.
- Ao alterar atributos, informar impacto em AttributeSet, MMC, ExecCalc, GameplayEffect, UI e replicacao.

## Regras de codigo
- Em implementacao, entregar codigo claro e compile-ready.
- Em edicao, preservar arquitetura e nomes salvo necessidade real.
- Nao remover replicacao existente sem justificativa.
- Se adicionar variavel replicada, incluir `UPROPERTY` adequada, `GetLifetimeReplicatedProps`, condicao de replica e `OnRep` quando necessario.
- Se adicionar RPC, validar origem, payload, contexto, ownership e abuso.
- Preferir clareza, determinismo, baixo acoplamento e seguranca de rede.
- Evitar `LogTemp` em fluxo importante; usar categorias explicitas.
- Nao deixar debug FX, debug logs ou caminhos editor-only no runtime shipping.

## Contrato obrigatorio de RPC
Todo RPC novo ou alterado deve declarar:
- origem permitida;
- owner esperado;
- payload maximo e limites por campo;
- validacao de contexto;
- validacao de ownership;
- rate-limit/anti-spam;
- comportamento em rejeicao;
- categoria de log;
- se afeta gameplay, por que precisa existir e nao pode ser derivado no servidor.

## Budget de rede
Mudanca net-critical deve informar:
- atores replicados afetados;
- propriedades replicadas novas ou alteradas;
- condicao de replicacao (`OwnerOnly`, `SkipOwner`, `None` etc.);
- `NetUpdateFrequency` dos atores relevantes;
- RPC/s maximo esperado por jogador;
- tamanho aproximado do payload;
- taxa/tamanho de target data quando aplicavel;
- risco de burst;
- risco de `OnRep` fora de ordem;
- impacto esperado em DS+2 e lag/loss.

## Gate por tipo de mudanca
| Tipo de mudanca | Validacao minima | Smoke obrigatorio | Evidencia |
|---|---|---|---|
| C++ simples sem rede/GAS | `SpellRiseEditor` quando build for pedida | Nao obrigatorio | log de build se executado |
| `UCLASS`/`UFUNCTION`/`UPROPERTY` | `SpellRiseEditor` com UHT | Conforme escopo | sem erro UHT |
| GAS/Ability/GE/ExecCalc/MMC | `SpellRiseEditor` + revisao GAS | Standalone, Listen, DS+2 | activation/commit/GE documentado |
| Projetil/combate/prediction | `SpellRiseEditor` + DS quando aplicavel | DS+2 normal + lag/loss | sem hit client-authoritative |
| PlayerController/RPC/replicacao | `SpellRiseEditor` + revisao net | DS+2 normal + lag/loss | sem `FBitReader::SetOverflowed` |
| Persistencia/loot/economia | Editor + DS | DS+2 + reconexao quando aplicavel | auditoria server-side |
| UI/VFX/SFX | Editor quando necessario | smoke visual/local | sem dependencia em Dedicated Server |

## Observabilidade minima
Fluxos criticos devem usar categoria de log explicita, nunca `LogTemp`.

Registrar quando aplicavel:
- RPC rejeitado e motivo;
- ability activation fail e `FailureTags`;
- commit/cost/cooldown negado;
- target data rejeitado;
- overflow de replicacao;
- falha de auth/session;
- persistencia recusada, reconciliada ou salva.

## Live Ops / Rollback
Mudanca sensivel de gameplay, rede, economia ou persistencia deve ter:
- default seguro no servidor;
- caminho de rollback;
- compatibilidade com dados persistidos;
- flag/config/data asset para desabilitar quando viavel;
- nota em CHANGELOG quando alterar contrato live.

## Atributos canonicos
- Primarios validos: STR, AGI, INT e WIS.
- Nenhum novo codigo, asset, GameplayEffect, tag, UI ou formula deve depender de atributos fora da matriz canonica.
- Qualquer uso fora da matriz canonica deve ser classificado como divida de migracao ou bug.

## Foco ativo atual
1. Corrigir overflow de replicacao no PlayerController.
2. Fechar bootstrap/auth Steam em Dedicated Server.
3. Fechar persistencia/economia server-side de producao.
4. Consolidar building mode com budget de rede e matriz de RPC.
5. Avancar automacao multiplayer em gate continuo/CI.

## Formato de resposta
Quando aplicavel, responder em:
1. Problema
2. Causa provavel
3. Correcao exata
4. Server
5. Client
6. Riscos de authority / prediction / RPC / OnRep
7. Arquivos completos alterados ou patch exato
8. Checklist de teste

## Debug
Quando o pedido for debug:
- listar primeiro a causa mais provavel;
- depois a correcao exata;
- depois o risco de replicacao;
- depois o que testar em Standalone, Listen Server e Dedicated Server;
- incluir lag/loss simulado apenas quando tocar combate, prediction, RPC, projetil, atributos replicados ou outro sistema net-critical.

## Documentacao viva
Se a mudanca altera contrato de authority, replicacao, atributo, persistencia, RPC ou build:
- atualizar o documento fonte correspondente;
- registrar conflito encontrado;
- registrar decisao tecnica curta;
- atualizar BACKLOG/BUG_LOG/CHANGELOG quando aplicavel.

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
