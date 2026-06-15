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
2. Progressao autoritativa: nivel da GA, dano/skill da arma quando aplicavel, nivel da escola e atributo primario.
3. Escala generica de dano.
4. Mitigacao por resistencia com penetracao.
5. Crit chance + crit multiplier.
6. Drains de stamina/mana pelo perfil de dano.

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

## Regras para novas mecânicas
- não introduzir aim assist;
- não transferir decisão de hit/dano para o cliente;
- não quebrar legibilidade de PvP;
- rework de balance deve provar aderência ao TTK alvo.
