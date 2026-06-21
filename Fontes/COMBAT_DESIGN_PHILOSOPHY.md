# Combat Design Philosophy

## Identidade
- Skill > Gear
- Reaction > RNG
- Positioning > Burst
- Free target
- Sem auto-aim
- Sem tab target

## TTK alvo (1v1)
- Armadura leve: `5-8s`
- Armadura média: `8-12s`
- Armadura pesada: `12-18s`

## Ordem de dano
1. Base damage do `SetByCaller`.
2. Progressao autoritativa: base da ability escalada por nivel da escola e dano da arma escalado por nivel da arma.
3. Escala generica de dano.
4. Booster ofensivo: `+5%` por slot em melee, bow, spell ou divine.
5. Escala PvP por diferenca de level efetivo, capado em `65`.
6. Mitigacao por resistencia com penetracao.
7. Crit chance + crit multiplier.
8. Drains de stamina/mana pelo perfil de dano.

## Clamps de balance
- `CritChance <= 25%`
- `CritDamageMultiplier <= 2.0x`
- `ArmorPenetration <= 30%`
- `Resistance <= 75%`

## Controle de power creep
- baseline primário fixo em `20`;
- crescimento linear;
- clamps rígidos em crit, penetração e resist;
- sem scaling exponencial nos primários atuais.
- diferenca de level em PvP usa cap efetivo `65`; levels acima disso contam como build completa.

## Regras para novas mecânicas
- não introduzir aim assist;
- não transferir decisão de hit/dano para o cliente;
- não quebrar legibilidade de PvP;
- rework de balance deve provar aderência ao TTK alvo.
