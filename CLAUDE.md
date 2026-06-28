Você trabalha no projeto SpellRise em:
C:\Users\biM\Documents\Unreal Projects\SpellRise

Responda sempre em português BR, curto, direto, foco em produção. Trate o repo como live service multiplayer contínuo. Economize tokens ao máximo: sem repetição, sem contexto desnecessário, sem chamadas redundantes. Se o histórico ficar pesado, sugira novo chat com resumo objetivo.

---

## FONTE DE VERDADE (consulta obrigatória antes de alterar arquitetura, gameplay, rede, GAS, atributos, persistência ou build)

PROJECT_STATE.md → AGENTS.md → ARCHITECTURE.md → ATTRIBUTE_MATRIX.md → MULTIPLAYER_RULES.md → SECURITY_MODEL.md → COMBAT_DESIGN_PHILOSOPHY.md → REFERENCIAS_JOGOS.md → BACKLOG.md → BUG_LOG.md → CHANGELOG.md → ENGINEERING_PLAYBOOK.md

Código legado conflitando com docs → sinalize e siga os docs.
Nunca invente classe, asset, tag, API, função, fluxo ou regra sem verificar equivalente existente.

---

## CONTEXTO FIXO

- Unreal Engine 5.7 · Dedicated Server obrigatório
- Hardcore PvP MMORPG · Full Loot · Free Target · Skill-Based
- Sem auto-aim, sem tab target · Combate server-authoritative

---

## BUILD

- Engine: C:\UnrealSource\UnrealEngine apenas
- Default (sem target explícito): SpellRiseEditor Win64 Development
- Proibido: Engine Launcher, MSBuild, .sln, vcxproj, Visual Studio, Rider como validação
- Proibido: Clean, apagar Binaries/Intermediate/DDC ou recompilar engine sem pedido explícito
- NUNCA afirme build, teste, smoke ou validação sem executar e ver resultado

---

## ARQUITETURA OBRIGATÓRIA

- ASC e AttributeSets vivem no PlayerState; Character consome via ActorInfo
- ASC replication mode: Mixed
- Primários canônicos: STR, AGI, INT, WIS
  - VIG/FOC: apenas compatibilidade passiva — novo código, GE, tag, UI ou fórmula NÃO deve depender deles
- Cliente nunca decide: dano, recursos, atributos, morte, loot, persistência, custo, cooldown, hit final
- DS não pode depender de HUD, widget, câmera, input local ou UI

---

## SERVER vs CLIENT

SERVER resolve: dano, custo, cooldown, recursos, atributos, morte, loot, persistência, respawn, fall damage, catalyst, mutação de estado, transições. Valida: RPC, payload, contexto, ownership, rate-limit, anti-spam. Commita abilities, valida target data, spawna projéteis replicados, aplica GE.

CLIENT: coleta input, aim trace local como intenção, prediction apenas para UX, exibe UI/VFX/SFX. Nunca decide resultado autoritativo.

---

## GAS, COMBATE E PROJÉTIL

Fluxo obrigatório:
aim local → target data → validação server → commit → spawn replicado → hit/GE no servidor

Ao alterar abilities: informe onde inicia, commita, valida no servidor, prevê no cliente e replica.
Ao alterar atributos: informe impacto em AttributeSet, MMC, ExecCalc, GameplayEffect, UI e replicação.

---

## REDE E RPC

- Multicast = apresentação apenas, nunca decisão autoritativa
- OnRep = reconciliação/apresentação, nunca decisão final
- Ability code não depende só de HasAuthority() para ativação
- PlayerController = input, UX local, ponte RPC. Não armazena estado autoritativo, não replica cosmético, não decide gameplay, não depende de HUD/widget/câmera em DS

Nova replicação no PlayerController exige: justificativa, alternativa considerada, condição de replica, budget, risco de FBitReader::SetOverflowed.

Todo RPC novo/alterado declara: origem · owner esperado · payload máximo · limites por campo · validação contexto/ownership · rate-limit · comportamento em rejeição · categoria de log · justificativa.

Mudança net-critical informa: atores/props replicadas, condição, NetUpdateFrequency, RPC/s por jogador, payload aprox., risco burst, OnRep fora de ordem, impacto DS+2 normal e com lag/loss.

---

## ENGENHARIA

- C++ prioritário; Blueprint apenas para UI, animação, VFX, assets, config e data assets
- Preserve nomes, arquitetura e convenções salvo defeito real
- Variável replicada: UPROPERTY adequada + GetLifetimeReplicatedProps + condição + OnRep quando necessário
- Categorias explícitas de log; sem LogTemp em fluxo importante
- Sem debug FX/logs ruidosos/caminhos editor-only em runtime shipping
- Não remova replicação existente sem justificativa
- Prefira data-driven sem enfraquecer autoridade/arquitetura

---

## LOGS CRÍTICOS OBRIGATÓRIOS

RPC rejeitado + motivo · activation fail/FailureTags · commit/cost/cooldown negado · target data rejeitado · overflow de replicação · falha de auth/session · persistência recusada/reconciliada/salva.

---

## GATE POR CATEGORIA

| Categoria | Gate |
|---|---|
| C++ simples sem rede/GAS | Compile quando build pedida |
| UCLASS/UFUNCTION/UPROPERTY | Validar SpellRiseEditor + UHT |
| GAS/Ability/GE/ExecCalc/MMC | Revisar activation, commit, GE, prediction, replicação |
| Projétil/combate/prediction/PC/RPC | Validar DS+2 normal e lag/loss, budget, overflow |
| Persistência/loot/economia | Validar server-side, reconciliação e auditoria |
| UI/VFX/SFX | Sem dependência em Dedicated Server |

Mudança sensível de gameplay/rede/economia/persistência: default seguro no servidor, rollback, compatibilidade com dados persistidos, flag/config quando viável, atualizar CHANGELOG/BACKLOG/BUG_LOG se alterar contrato live.

---

## FORMATO DE RESPOSTA (quando aplicável)

1. Problema
2. Causa provável
3. Correção exata
4. Server
5. Client
6. Riscos: authority / prediction / RPC / OnRep
7. Arquivos alterados ou patch exato
8. Checklist de teste

---

## DEFINIÇÃO DE PRONTO

Compile-safe · GAS e replicação respeitados · Server/Client separados · DS preservado · sem trust indevido no cliente · riscos registrados · checklist objetivo · nenhuma validação alegada sem execução real.

---

## FOCO ATIVO

1. [CRÍTICO] Overflow de replicação no PlayerController
2. [CRÍTICO] Bootstrap/auth Steam em Dedicated Server
3. [CRÍTICO] Persistência/economia server-side de produção
4. [ALTO] Building mode: budget de rede e matriz de RPC
5. [MÉDIO] Automação multiplayer em gate contínuo/CI