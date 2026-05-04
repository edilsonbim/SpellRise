# Engineering Playbook — SpellRise

## Objetivo
Padronizar execução técnica em fluxo AAA enxuto para build, revisão de authority, replicação e smoke multiplayer.

## Política fixa de engine
- Toda build e toda validação devem usar apenas a Unreal Source em `C:\UnrealSource\UnrealEngine`.
- Não usar engine do launcher para build, smoke ou validação final.
- Quando o usuário pedir build sem especificar outro target, rodar somente o target `SpellRiseEditor Win64 Development` do projeto.
- Usar `Build.bat` da Unreal Source não significa rebuildar a engine; o alvo deve ser sempre do projeto (`SpellRiseEditor`, `SpellRiseServer` ou `SpellRiseClient`).
- Não usar `MSBuild`, `SpellRise.sln`, `Intermediate/ProjectFiles/SpellRise.vcxproj` ou build acionado por IDE como validação do agente, porque isso pode disparar fluxo de projeto gerado e compilar targets indesejados.
- O comando padrão de build do editor é `C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat SpellRiseEditor Win64 Development "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" -WaitMutex -NoHotReloadFromIDE`.
- Não adicionar target manual de `ShaderCompileWorker` ao comando de build do agente.
- Não rodar build completo se `UnrealEditor.exe` ou `LiveCodingConsole.exe` estiverem ativos. Fechar o editor ou desativar Live Coding antes da validação.
- Para mudança de header com `UCLASS`, `UFUNCTION` ou `UPROPERTY`, rodar validação curta de UHT/target do projeto e reportar bloqueio se Live Coding impedir a compilação.
- Escolher o target pelo escopo: `SpellRiseEditor` para UHT/editor, `SpellRiseServer` para Dedicated Server e `SpellRiseClient` apenas quando houver impacto específico no cliente.
- Não executar `Clean`, apagar `Binaries`, `Intermediate`, `DerivedDataCache` ou disparar rebuild da engine sem pedido explícito.

## Pipeline padrão
1. **Build**
2. **Revisão funcional do escopo** (`GAS`, rede, input ou UI)
3. **Revisão de replicação/RPC**
4. **Smoke multiplayer**
5. **Registro de evidências**

## Stages recomendados
- `build`
- `gas`
- `net`
- `input`
- `smoke`

## Script operacional
- `Scripts/Run-Agent-Orchestrator.ps1`
- Relatórios em `Saved/Logs/AgentOrchestrator/<RunId>/`

### Modos
- `checklist`: imprime prompts e tarefas de stage.
- `run`: executa build/smoke e gera tarefas para revisão manual quando necessário.

## Agentes operacionais
### BuildGuardian
Use quando houver falha de compilação, warning crítico ou dúvida de toolchain.

### GASAuthorityGuard
Use para ability, commit, prediction, cue, GE, ExecCalc e fluxo autoritativo.

### ReplicationSentinel
Use para RPC, ownership, `OnRep`, relevância, overflow de payload e regressão de rede.

### InputContextArchitect
Use para `Enhanced Input`, IMC, conflito de input e modos de jogo/construção/UI.

### SmokeDSVerifier
Use para validar regressão em `Standalone`, `Listen`, `DS+2` e lag/loss.

## Gate obrigatório para mudança de combate/rede
### Build
- `SpellRiseEditor Win64 Development` compila.
- Sem warning crítico novo no recorte.

### RPC
Para cada RPC novo/alterado, registrar:
- origem permitida;
- payload esperado + limites;
- validação de contexto;
- anti-spam;
- falha segura + log categorizado.

### Network Budget
Toda feature de combate/rede deve declarar:
- `NetUpdateFrequency` dos atores relevantes;
- max RPC/s por player;
- payload max por RPC;
- taxa/tamanho de target data.

### Smoke mínimo
1. `Standalone`
2. `Listen Server`
3. `Dedicated Server + 2 clientes`
4. `Dedicated Server + 2 clientes` com:
   - Perfil A: `Net PktLag=120` e `Net PktLoss=1`
   - Perfil B: `Net PktLag=180` e `Net PktLoss=3`

## Casos mínimos de smoke
- cast / commit / cooldown / custo;
- projétil autoritativo completo;
- morte / loot / respawn;
- reconexão básica;
- quando aplicável: building mode e input gating.

## Evidência mínima para fechamento
- log ou summary por cenário;
- resultado PASS/FAIL;
- causa raiz se falhar;
- próximo passo objetivo.

## Gate de rede para CI (obrigatório em mudança net-critical)
Mudanças em combate, GAS, projétil, atributos replicados, morte/loot/respawn, building mode ou PlayerController devem:
- rodar `DS+2` normal + perfis A/B de lag/loss;
- registrar `TryActivate` success/fail com `FailureTags`;
- registrar RPC rejeitado por motivo;
- falhar pipeline em:
  - qualquer `FBitReader::SetOverflowed`;
  - trust indevido no cliente (dano/custo/cooldown/morte/loot);
  - regressão funcional autoritativa.

## DoD de replicação (PR checklist)
- escopo de authority/server/client explícito;
- RPC novo/alterado com validação + anti-spam;
- variável replicada nova com condição e impacto de `OnRep`;
- evidência de teste em `Standalone`, `Listen` e `DS+2` (normal + lag/loss).

## Referências oficiais
- GAS: documentação oficial da Epic.
- Lyra: documentação oficial da Epic.
- Narrative: documentação oficial do plugin usado no projeto.
