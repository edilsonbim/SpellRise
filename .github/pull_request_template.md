# SpellRise PR Checklist

## Escopo
- [ ] Descrevi o problema e a causa provavel.
- [ ] Listei arquivos/sistemas alterados.
- [ ] Confirmei que nao inventei classe, asset, tag, API ou funcao sem verificar equivalente existente.
- [ ] Atualizei docs fonte quando alterei contrato de authority, rede, GAS, atributos, build ou persistencia.

## Server authority
- [ ] Dano, custo, cooldown, recurso, morte, loot, persistencia e resultado final continuam server-authoritative.
- [ ] Dedicated Server nao depende de HUD, widget, camera, input local ou UI.
- [ ] Cliente envia intencao, nao resultado final.

## GAS / combate
- [ ] ASC e AttributeSets continuam no PlayerState.
- [ ] Alteracoes de ability separam activation, target data, commit, GE, cue, cosmetico local e cleanup.
- [ ] Fluxo de projetil preservado: aim local -> target data -> validacao server -> commit -> spawn replicado -> hit/GE no servidor.
- [ ] Impacto em AttributeSet/MMC/ExecCalc/GE/UI/replicacao foi declarado quando aplicavel.

## RPC / replicacao
- [ ] RPC novo/alterado declara origem permitida, owner, payload, validacao, rate-limit e rejeicao segura.
- [ ] Variavel replicada nova tem UPROPERTY, GetLifetimeReplicatedProps, condicao e OnRep quando necessario.
- [ ] PlayerController nao recebeu estado autoritativo de gameplay.
- [ ] Budget de rede declarado para mudanca net-critical.
- [ ] Risco de OnRep fora de ordem ou race foi avaliado.

## Observabilidade
- [ ] Fluxos criticos usam categoria explicita, nao LogTemp.
- [ ] Rejeicoes de RPC/target data/commit/cost/cooldown registram motivo quando aplicavel.
- [ ] Falhas de auth/session/persistencia registram motivo auditavel quando aplicavel.

## Validacao
- [ ] Build/validacao usou apenas C:\UnrealSource\UnrealEngine.
- [ ] Nao usei MSBuild, SpellRise.sln, vcxproj, Visual Studio ou Rider como validacao.
- [ ] Standalone validado quando aplicavel.
- [ ] Listen Server validado quando aplicavel.
- [ ] Dedicated Server + 2 clientes validado quando aplicavel.
- [ ] DS+2 com lag/loss validado para mudanca net-critical quando aplicavel.
- [ ] Sem `FBitReader::SetOverflowed`.

## Evidencia
- [ ] Colei summary/log do gate executado.
- [ ] Declarei testes nao executados e motivo.
- [ ] Declarei riscos residuais.
- [ ] Declarei plano de rollback quando aplicavel.
