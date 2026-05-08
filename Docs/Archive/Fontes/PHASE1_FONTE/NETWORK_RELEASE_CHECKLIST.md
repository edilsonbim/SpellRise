# Network Release Checklist - SpellRise

## Objetivo
Checklist curto para aprovação de mudanças com impacto em rede, GAS, authority, replicação ou runtime multiplayer.

## 1. Contract check
- [ ] Server vs Client explícito
- [ ] Sem trust indevido no cliente
- [ ] GAS owner continua no `PlayerState`
- [ ] `ASC` continua em `Mixed`
- [ ] Dedicated Server continua independente de HUD/UI/câmera

## 2. RPC / Replication
- [ ] RPC novo/alterado tem origem permitida declarada
- [ ] Payload tem limites claros
- [ ] Validação de contexto existe no servidor
- [ ] Anti-spam/rate limit existe quando necessário
- [ ] Falha segura com log categorizado
- [ ] Replicação nova tem condição adequada
- [ ] `OnRep` só existe quando a reação local é necessária

## 3. Prediction
- [ ] O que é previsão local está claro
- [ ] O que o servidor confirma está claro
- [ ] Há estratégia de reconciliação
- [ ] Sem double commit
- [ ] Sem double fire
- [ ] Sem double damage

## 4. Projectile flow
- [ ] Client faz aim trace local
- [ ] Client envia target data
- [ ] Server valida target data
- [ ] Server commita
- [ ] Server spawna projétil replicado
- [ ] Hit/overlap aplica GE no servidor

## 5. Runtime scenarios
Executar:
- [ ] Standalone
- [ ] Listen Server
- [ ] Dedicated Server + 2 clientes
- [ ] Dedicated Server + 2 clientes com reconnect
- [ ] Dedicated Server + 2 clientes com lag/loss

## 6. Lag/Loss
Aplicar no cliente de teste:
- `Net PktLag=120`
- `Net PktLoss=5`

Aceite:
- [ ] sessão permanece estável
- [ ] sem regressão visível de OnRep
- [ ] sem falha de reconnect no cenário coberto

## 7. Focos ativos obrigatórios
- [ ] Sem `FBitReader::SetOverflowed`
- [ ] Sem `ReadFieldHeaderAndPayload` crítico no PlayerController path
- [ ] Sem regressão de bootstrap/auth do DS
- [ ] Sem regressão de persistência no recorte alterado

## 8. Resultado final
A mudança só está pronta quando:
- [ ] build/package aprovados
- [ ] smoke aprovado
- [ ] riscos de authority/prediction/RPC/OnRep registrados
- [ ] evidências de log anexadas
- [ ] causa raiz documentada em caso de falha
