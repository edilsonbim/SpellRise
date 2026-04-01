# Pipeline Operacional Real - SpellRise

## Objetivo
Transformar as fases de documentação em um pipeline executável, alinhado com prática AAA de multiplayer: build/package par, validação de runtime, smoke dedicado, persistência rastreável e combat lab com evidência.

## O que este pacote resolve
- converte o gate de release em script acionável;
- cria validação explícita de package parity entre Client e Server;
- encadeia um gate dedicado para persistência;
- cria um harness operacional para o combat lab;
- centraliza logs em `Saved/Logs/Automation`.

## Princípios
- Unreal Source é obrigatória.
- Client e Server precisam nascer do mesmo ciclo.
- DS+2 continua sendo o gate mínimo real.
- reconnect e lag/loss continuam obrigatórios para net-critical.
- persistência continua server-authoritative e DB-first como alvo.
- nenhuma automação substitui authority review, RPC review e leitura dos logs.

## Estrutura recomendada
- copiar os scripts para `Scripts/`
- manter este documento em `Fontes/`
- usar `Run-SpellRise-ReleaseGate.ps1` como ponto de entrada padrão
- usar `Run-SpellRise-PersistenceGate.ps1` quando tocar save/load/auth/session
- usar `Run-SpellRise-CombatLab.ps1` quando tocar combate, fórmula, prediction ou projectile flow

## Fluxo recomendado de equipe
1. BuildGuardian resolve build local.
2. Especialista do domínio fecha correção.
3. ReplicationSentinel revisa authority/RPC/OnRep.
4. `Run-SpellRise-ReleaseGate.ps1` valida package + parity + smoke.
5. `Run-SpellRise-PersistenceGate.ps1` entra quando a mudança toca persistência.
6. `Run-SpellRise-CombatLab.ps1` fecha o recorte de combate/balance/regressão.

## Limites atuais
Este pacote já é operacional, mas ainda depende de:
- `Scripts/Run-Smoke-Gate.ps1` existir no projeto;
- maps/perfis online corretos do ambiente real;
- hooks de automação específicos do jogo para cenários mais profundos de persistência e combat lab.

## Próximo ganho recomendado
Depois de colocar estes scripts no projeto, o próximo passo de maior valor é integrar o `Run-SpellRise-ReleaseGate.ps1` em um gate local padronizado e depois em CI.
