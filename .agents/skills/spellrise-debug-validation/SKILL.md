---
name: spellrise-debug-validation
description: Use para debug e validacao em SpellRise/UE5.7, incluindo logs, repro minimo, falhas de gameplay, GAS, RPC, replicacao, Dedicated Server, overflow, persistencia e checks de regressao.
---

# SpellRise Debug Validation

Adaptacao SpellRise-safe do workflow publico `ue5-debug-validation`.

## Ordem de debug
1. Liste a causa mais provavel.
2. Diga a correcao exata.
3. Aponte risco de replicacao.
4. Liste o que testar em Standalone, Listen Server e Dedicated Server.
5. Inclua lag/loss quando tocar combate, prediction, RPC, projetil, atributos replicados ou sistema net-critical.

## Captura minima
- Passos de reproducao.
- Modo de execucao: Standalone, Listen Server, Dedicated Server.
- Mapa, pawn, PlayerState/Controller envolvidos.
- Logs por categoria relevante.
- Primeiro ponto onde o fluxo diverge.
- Resultado esperado vs observado.

## Hipoteses
Classifique por dominio:
- dados/assets;
- Blueprint;
- C++;
- GAS;
- RPC;
- replicacao/OnRep;
- Dedicated Server/headless;
- persistencia/economia;
- config/build.

## Regras
- Nao misture instrumentacao ampla com correcao funcional sem necessidade.
- Nao use `LogTemp` em fluxo importante.
- Preserve evidencia antes de mitigar.
- Prefira menor fix que isola a primeira transicao quebrada.
- Nao afirmar build, teste ou smoke sem executar e ver resultado.

## Validacao
- C++ simples: compile quando build for pedida.
- UCLASS/UFUNCTION/UPROPERTY: `SpellRiseEditor` com UHT quando build for pedida.
- GAS/Ability/GE/ExecCalc/MMC: revisar activation, commit, GE, prediction e replicacao.
- PlayerController/RPC/replicacao: avaliar overflow e budget de rede.
- Mudancas net-critical: DS+2 normal e lag/loss quando possivel.

## Saida esperada
1. Problema
2. Causa provavel
3. Correcao exata
4. Server
5. Client
6. Riscos de authority / prediction / RPC / OnRep
7. Arquivos alterados ou patch exato
8. Checklist de teste

