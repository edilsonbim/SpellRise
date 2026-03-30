# Validacao - Combat Log no Chat (2026-03-16)

## Escopo
Validar o pacote de combat log com chat:
- Mensagem para quem causou dano.
- Mensagem para quem recebeu dano.
- Bloqueio de envio manual no canal `Combat`.

## Pre-checks executados
- Build `SpellRiseEditor Win64 Development`: OK.
- Smoke startup `-game` em `/Game/Maps/Map2/Map2`: OK.

## Ambiente recomendado (PIE)
1. Abrir mapa: `/Game/Maps/Map2/Map2`.
2. Em Play Settings:
- Net Mode: `Play As Listen Server`.
- Number of Players: `2` (depois repetir com `3`).
- Run Under One Process: ligado (mais rapido para QA funcional).
3. Garantir GameMode de gameplay ativo (preferencialmente `BP_SpellRiseGameMode`).

## Casos de teste
### CT-01 - Dano causado aparece para atacante
1. Iniciar PIE com 2 players.
2. No Player A, acertar o Player B com habilidade/projetil.
3. Abrir chat no A, trocar para canal `Combat`.
Resultado esperado:
- A recebe uma linha no `Combat` no formato:
  - `Voce causou <N> de dano em <NomeDoAlvo>.`

### CT-02 - Dano recebido aparece para alvo
1. Reaproveitar o hit do CT-01.
2. No Player B, abrir chat no canal `Combat`.
Resultado esperado:
- B recebe uma linha no `Combat` no formato:
  - `Voce recebeu <N> de dano de <NomeDaFonte>.`

### CT-03 - Player nao envolvido nao recebe esse evento
1. Iniciar PIE com 3 players.
2. A causar dano em B.
3. Verificar o canal `Combat` do Player C.
Resultado esperado:
- C nao deve receber a mensagem desse hit entre A e B.

### CT-04 - Bloqueio de mensagem manual no canal Combat
1. Em qualquer player, selecionar canal `Combat`.
2. Tentar enviar texto manual pelo chat.
Resultado esperado:
- Envio deve ser bloqueado no servidor.
- Feedback esperado no cliente:
  - `Canal Combat e somente leitura.`

### CT-05 - Outros canais continuam enviando normal
1. Selecionar `Global` (ou Party/Guild).
2. Enviar mensagem manual.
Resultado esperado:
- Mensagem deve ser enviada normalmente.

### CT-06 - Self-damage (se aplicavel)
1. Causar dano em si mesmo (queda/efeito de area proprio).
2. Verificar canal `Combat` do mesmo player.
Resultado esperado:
- Apenas uma entrada de recebido para o evento:
  - `Voce recebeu <N> de dano de <NomeDaFonte>.`

## Logs uteis para diagnostico
- Bloqueio manual no `Combat`:
  - `[Chat] Blocked SendChatToSERVER to combat channel ...`
- Falha no append reflexivo do chat:
  - `[Chat] Could not append combat log via chat component ...`

## Criterio de aceite
- CT-01, CT-02, CT-03, CT-04 e CT-05 aprovados.
- Sem crash, sem erro de rede, sem regressao de number pop.

## Observacao tecnica
A integracao com chat usa bridge reflexiva (funcao/fields do BP de chat). Se nomes de funcao/campos forem renomeados nos assets, o append pode falhar e precisa ajuste no C++.
