# Revisao de Codigo (SpellRise) - Snapshot 2026-03-25

## Findings Atuais (ordenado por risco)

1. **[Media] Cobertura automatizada de rede ainda incompleta em CI**
   - O projeto tem gate local de smoke DS+2 (normal e lag/loss), mas ainda falta gate equivalente em pipeline de CI para bloquear regressao antes de merge.
   - Risco: regressao de authority/prediction/RPC pode voltar entre ciclos sem detecao precoce automatizada.
   - Acao recomendada: promover `Scripts/Run-Smoke-Gate.ps1` para job de CI com artefatos de log obrigatorios.

2. **[Media] Persistencia de producao ainda em transicao**
   - A base atual esta funcional em provider JSON server-side, mas os marcos de economia + DB operacional ainda estao em aberto.
   - Risco: limites de escalabilidade/auditoria para ambiente de producao.
   - Acao recomendada: concluir fases 3.3/3.5 com criterios de saida do `ROADMAP.md`.

3. **[Baixa] Divida de balanceamento pendente**
   - Passagem de balance para resistencia/penetracao/crit ainda aberta no backlog.
   - Risco: desalinhamento de TTK alvo em cenarios extremos de build.
   - Acao recomendada: fechar rodada de tuning com telemetria comparada aos alvos do `COMBAT_DESIGN_PHILOSOPHY.md`.

## Itens Resolvidos no Ciclo Atual
- Warning de plugin `NarrativeInventory` sobre dependencia `CommonUI` corrigido no `.uplugin`.
- Warning C4996 de `FString::LeftInline` corrigido para assinatura compativel com UE 5.7.
- Build `SpellRiseEditor Win64 Development` revalidado com sucesso apos as correcoes.

## Observacoes
- Nao foram encontrados erros de compilacao no snapshot atual.
- Regras de authority (server-authoritative para dano/recursos/morte/loot) permanecem alinhadas com os documentos canonicos.
