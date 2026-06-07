# SpellRise - Planta 2D Dark Fantasy

## Objetivo
Referencia visual para transformar o greybox tecnico em um cenario dark fantasy jogavel, sem perder leitura de PvP free target.

## Arquivo visual
- `Fontes/MapDesign/SpellRise_DarkFantasy_MapPlan.svg`

## Ideia central
Uma regiao isolada de 2 km x 2 km com ruinas antigas no centro, biomas ao redor e rotas de risco conectando recursos raros.

## Zonas
- **Ruinas do Catalyst**: hotspot central de PvP, boss/evento e recurso raro.
- **Floresta Maldita**: norte, emboscada, cobertura, visibilidade controlada.
- **Crista dos Minerios**: leste, vantagem de altura, mineracao e risco de queda.
- **Pantano das Ervas**: oeste, caminhos estreitos, recurso noturno e nevoa baixa.
- **Costa dos Naufragios**: sul, extracao, fuga e conflito por loot.
- **Fortaleza Quebrada**: sudeste, gargalos, muralhas e defesa temporaria.

## Rotas
- Rotas principais largas para leitura de combate.
- Atalhos perigosos cruzando perto do centro.
- Rio/vale como elemento de navegacao e divisao natural do mapa.

## Regras
- O mapa nao decide gameplay autoritativo.
- Recursos, loot, boss e persistencia futuros devem ser server-side.
- Vegetacao e nevoa nao podem destruir a leitura do free target.
- Toda cobertura deve ter funcao de combate, rota ou risco.

## Proximo passo
Converter a planta 2D em terrain blockout organico no `LV_SpellRise_Greybox`, substituindo linhas retas por:
- relevo;
- ravinas;
- trilhas curvas;
- ruinas irregulares;
- gargalos naturais;
- zonas elevadas;
- bordas de mundo plausiveis.
