# Persistence Release Checklist

## Uso
Checklist operacional para qualquer mudança que toque save/load, inventário persistente, economia, reconnect ou auth/session.

## 1. Pré-check
- [ ] Mudança descreve escopo de persistência afetado.
- [ ] Server vs Client está explícito.
- [ ] Fonte de verdade revisada: `PROJECT_STATE.md`, `ARCHITECTURE.md`, `SECURITY_MODEL.md`, `BACKLOG.md`, `ROADMAP.md`.
- [ ] Caminho DB-first e fallback estão documentados para a mudança.
- [ ] Impacto em auth/session está declarado.

## 2. Contrato técnico
- [ ] Cliente não escreve estado final autoritativo.
- [ ] Servidor valida contexto de save/load.
- [ ] Revisão/check otimista ou equivalente existe.
- [ ] Falha parcial tem rollback ou rejeição segura.
- [ ] Logs categorizados existem para sucesso e falha.

## 3. Build e package
- [ ] Build feita pela Unreal Source.
- [ ] Client package válido.
- [ ] Server package válido.
- [ ] Paridade Client/Server conferida no recorte alterado.

## 4. Cenários obrigatórios
### Runtime básico
- [ ] login/load normal
- [ ] save normal
- [ ] reconnect após save

### Casos críticos
- [ ] disconnect durante mutação de inventário
- [ ] morte + full loot + save
- [ ] reconnect após morte/full loot
- [ ] conflito de revisão
- [ ] falha do provider primário
- [ ] auth inválida ou degradada

## 5. Modos de rede
- [ ] Standalone
- [ ] Listen Server
- [ ] Dedicated Server + 2 clientes
- [ ] Dedicated Server + 2 clientes com reconnect
- [ ] Dedicated Server + 2 clientes com lag/loss

## 6. Evidências mínimas
- [ ] logs de `LoadSucceeded/Rejected`
- [ ] logs de `SaveSucceeded/Rejected`
- [ ] logs de rollback quando aplicável
- [ ] evidência do modo operacional: normal/degradado/bloqueado
- [ ] commit/build avaliados registrados

## 7. Reprovação imediata
- [ ] cliente consegue influenciar inventário/economia persistente final
- [ ] reconnect duplica ou perde item
- [ ] save confirma antes da persistência autoritativa
- [ ] fallback silencioso para storage secundário
- [ ] flow sensível aceita identidade inválida quando deveria bloquear
- [ ] ausência de telemetria mínima

## 8. Definição de pronto
- [ ] comportamento determinístico em reconnect
- [ ] sem estado híbrido inválido após falha parcial
- [ ] auth/session gate coerente com o ambiente
- [ ] sem regressão de dedicated server
- [ ] checklist completo com evidências
