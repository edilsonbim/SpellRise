# Pipeline - Progressao de Combate

## Onde vive
- Componente: `USpellRiseProgressionComponent`.
- Owner: `ASpellRisePlayerState`.
- Replicacao: owner-only.
- Nivel valido: `1..100`.

## Tags de arma
- `Progression.Weapon.TwoHandSword` (2H Sword)
- `Progression.Weapon.TwoHandHammer` (2H Hammer)
- `Progression.Weapon.TwoHandAxe` (2H Axe)
- `Progression.Weapon.Sword`
- `Progression.Weapon.Hammer`
- `Progression.Weapon.Axe`
- `Progression.Weapon.Bow`
- `Progression.Weapon.Shield`
- `Progression.Weapon.Staff`

## Tags de escola
- `Progression.School.Melee`
- `Progression.School.Archery`
- `Progression.School.Fire`
- `Progression.School.Cold`
- `Progression.School.Acid`
- `Progression.School.Shock`
- `Progression.School.Divine`
- `Progression.School.Curse`
- `Progression.School.Generic`

## Regra
- GA declara `WeaponProgressionTag` e `SchoolProgressionTag`.
- GA declara `DamageChannelTag` e `DamageTypeTag`.
- GA marca `bUsesEquippedWeaponDamage` quando a arma equipada entra no calculo.
- GA nao deve passar nivel de arma/escola manualmente.
- GA migrada nao deve fazer cast manual da weapon para mandar dano da arma.
- Servidor consulta o componente por tag.
- Cliente usa a replicacao apenas para UI.
- Dano final continua no servidor.

## Dano base da arma
- Atributo: `EquippedWeaponBaseDamage`.
- SetByCaller do GE de equipamento: `Data.EquippedWeaponBaseDamage`.
- O `WeaponComponent` aplica/remover o GE listado na `WeaponDefinition`.

| Arma | Base |
|---|---:|
| 2H Sword | 36 |
| 2H Hammer | 39 |
| 2H Axe | 36 |
| Sword | 27 |
| Hammer | 29 |
| Axe | 27 |
| Bow | 35 |
| Staff | 30 |
