# Animation Pipeline

## Padrao canonico
- Runtime locomotion: Game Animation Sample / UE5 Manny-Quinn.
- Visual final do player: `VisualOverride`, incluindo MetaHuman.
- MetaHuman nao e esqueleto autoritativo de gameplay; e camada visual.
- Animacoes UE5 Manny/Quinn sao a fonte primaria.
- Animacoes UEFN sao biblioteca secundaria e devem entrar retargetadas.

## Contrato runtime
- `ASpellRiseCharacterBase`, capsule, `CharacterMovement`, GAS e PlayerState continuam sendo a fonte de gameplay.
- `VisualOverride` e apenas apresentacao, equipamento visual e sockets de attach.
- AnimBP consome estados/tags/velocidade; nao decide dano, hit, morte, loot, stun ou posicao final.
- Root motion so pode ser cosmetico ou validado pelo servidor por ability/movimento autoritativo.

## Retarget
- Toda animacao externa deve ser retargetada para o padrao UE5 Manny-Quinn ou para o MetaHuman via retargeter explicito.
- Retargeters MetaHuman existentes devem ser usados para saida visual:
  - `RTG_metahuman`
  - `RTG_metahuman_base_skel_AnimBP`
  - `RTG_UEFN_to_Metahuman_nrw`
  - `RTG_UEFN_to_Metahuman_ovw`
- Nao misturar UE5, UEFN e MetaHuman diretamente no mesmo AnimBP sem camada de retarget clara.

## Colisao do visual
- Mesh viva em `VisualOverride` deve usar `CharacterMesh` ou `NoCollision`.
- Mesh viva deve ignorar `Pawn` e `Visibility`.
- Capsule e o dono do movimento/colisao de gameplay.
- Ragdoll/morte pode trocar a mesh para perfil `Ragdoll`.

## Aceitacao
- Dedicated Server nao depende de HUD, camera, MetaHuman ou AnimBP visual.
- Player MetaHuman via `VisualOverride` toca locomotion do Game Animation Sample.
- UE5 Manny/Quinn e UEFN so entram no runtime por retarget validado.
- Ataques, projeteis, morte, loot e ragdoll continuam server-authoritative.
