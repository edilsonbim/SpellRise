# Changelog

## Unreleased
### Build / Tooling
- Build policy consolidada em Unreal Source.
- Compatibilidade UE 5.7 ajustada em pontos críticos de build.
- Dependência de `CommonUI` declarada corretamente no plugin Narrative.

### Gameplay / Networking
- Combat log autoritativo com transporte nativo em C++.
- Hardening de RPCs críticos de gameplay e respawn.
- Validação server-side para target data de projétil.
- Fluxo de death -> full loot -> respawn fechado no recorte atual.
- Hook determinístico de QA para morte em smoke multiplayer.
- Building mode isolado em componente dedicado no `PlayerController`.

### Persistence / Observability
- Persistence foundation v1 ativa com snapshots de personagem e inventário.
- Combat log durável owner-centric em `PlayerState`.
- Gate local de smoke multiplayer com reconnect e lag/loss.

### Known Active Risks
- overflow de replicação no `PlayerController`;
- bootstrap/auth Steam em DS;
- persistência de produção ainda incompleta;
- contrato final de rede do building mode ainda pendente.
