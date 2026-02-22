# Revisão rápida do código (SpellRise)

## Erros e riscos encontrados

1. **Função morta e lógica inconsistente em checagem de dano**
   - A função `SR_CanApplyDamage` em `SpellRiseGameplayAbility.cpp` não era utilizada e, além disso, retornava `true` em praticamente todos os cenários (inclusive com `bAllowFriendlyFire=false`).
   - Isso podia mascarar bugs de regra de combate e gerar confusão de manutenção.
   - **Ação aplicada:** removida para evitar código morto e comportamento enganoso.

2. **Risco de falha silenciosa de tags de gameplay no cálculo de dano**
   - Em `ExecCalc_Damage.cpp`, vários `RequestGameplayTag(..., false)` dependem de tags configuradas externamente.
   - Se uma tag faltar, o cálculo usa fallback/default, o que pode esconder problemas de configuração.
   - **Sugestão:** adicionar validação centralizada na inicialização (ex.: `UGameInstance`) para garantir que tags críticas existam.

3. **Muitos números mágicos em MMCs de atributos**
   - Em `MMC_MaxHealth.cpp`, `MMC_MaxMana.cpp` e `MMC_MaxStamina.cpp`, há constantes embutidas em fórmulas (ex.: `100.f`, `12.f`, `80.f`, `14.f`).
   - **Sugestão:** mover para `constexpr` nomeados ou `DataAsset/DataTable` para facilitar balanceamento sem recompilar.

4. **Uso de BOM UTF-8 em alguns arquivos C++**
   - Alguns arquivos iniciam com BOM (ex.: `SpellRiseGameplayAbility.cpp` e `SpellRisePlayerState.cpp`).
   - **Sugestão:** padronizar para UTF-8 sem BOM para reduzir ruído em diffs e problemas em ferramentas externas.

## Melhorias sugeridas (prioridade)

- **Alta:** validação de tags críticas no boot do jogo.
- **Média:** externalizar fórmulas e constantes de balanceamento para dados.
- **Média:** criar testes automatizados para cálculos de dano/atributos (cenários com/sem tag, crítico, resistência, true damage).
- **Baixa:** limpeza de estilo/encoding e padronização de comentários bilíngues.
