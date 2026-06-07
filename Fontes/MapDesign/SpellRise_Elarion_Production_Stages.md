# Elarion - Estagios de Producao

## Decisao macro
- Mundo final: **64 km x 64 km**.
- Estrutura: pangeia central chamada **Elarion** + quatro ilhas externas.
- Producao por regioes.
- Lancamento pode ocorrer com areas bloqueadas, desde que os bloqueios sejam diegeticos: tempestade liminar, pontes quebradas, mares perigosos, portais inativos, muralhas naturais, colapso arcano ou autoridade de Refugio.

## Documentos operacionais
- `Fontes/MapDesign/SpellRise_Map_Production_Playbook.md`
- `Fontes/MapDesign/SpellRise_Map_Conversation_Log.md`
- `Fontes/MapDesign/SpellRise_World_Scale.md`

## Nomes das quatro ilhas
| Ilha | Clima | Fantasia | Papel futuro |
|---|---|---|---|
| **Frostvein** | Boreal/gelo | Aurora morta, gelo liminar, tempo quebrado | Ilha de alto risco, dragões da Aurora Morta, minério frio |
| **Glassmere** | Deserto/cristal | Dunas, sal, miragens, vidro arcano | Dragões de Vidro, ruínas soterradas, ilusões |
| **Rootwold** | Selva/pântano | Floresta antiga, serpes de raiz, veneno | Madeira rara, alquimia, ameaça natural |
| **Ashkar** | Vulcânica/cinzas | Lava antiga, metal negro, fortalezas queimadas | Dragões de Cinza, metal raro, guerra de guildas |

## Estagio 0 - Fundacao macro
Objetivo: travar a escala e a leitura do mundo.

Entregas:
- Canvas 64 km x 64 km.
- Pangeia e ilhas posicionadas.
- Mar reservado para navegacao futura.
- Mapa satelite como referencia.
- Zonas macro: verde, amarela, vermelha e preta.
- Assentamentos v01 definidos.

Estado: em andamento.

## Estagio 1 - Regiao inicial de Aureth
Objetivo: primeiro recorte jogavel de producao.

Escopo:
- Noroeste da pangeia.
- Cidade racial: **Aureth**.
- Bioma: floresta sombria + costa recortada.
- Zona verde maior para jogador casual.
- Primeira faixa amarela com coleta, vilas e risco moderado.
- Uma dungeon inicial simples, instanciada, com recompensa limitada.
- Primeiro portal antigo visualmente presente, mas restrito.

Tamanho:
- Regiao representada: **8 km x 8 km**.
- Area jogavel inicial: **4 km x 4 km a 6 km x 6 km**, conforme teste.

Bloqueios:
- floresta densa;
- ponte destruida;
- tempestade liminar;
- costa perigosa;
- portal inativo.

## Estagio 2 - Fronteira de Aureth
Objetivo: transformar a regiao inicial em loop MMO real.

Entregas:
- Expansao da zona amarela.
- Primeiras cidades de cla proximas.
- Hamlets disputaveis.
- Recursos intermediarios.
- Entrada de dungeon vermelha bloqueada por progressao/milestone.
- Rotas de caravana e escolta.

Risco:
- PvP parcial.
- Drop de bag.
- Primeiras emboscadas relevantes.

## Estagio 3 - Kharven e as Alturas
Objetivo: abrir segunda cidade racial e bioma de montanha.

Escopo:
- Cidade racial: **Kharven**.
- Montanhas, gargalos, minas e cliffs.
- Recursos metalicos.
- Cidadelas futuras.
- Rotas elevadas com risco de emboscada.

Gameplay:
- crafting metalurgico;
- mineração;
- controle de passagem;
- primeiras guerras por cidade de cla.

## Estagio 4 - Mournfall e os Brejos
Objetivo: abrir terceira cidade racial e bioma de pântano.

Escopo:
- Cidade racial: **Mournfall**.
- Pântanos, rios lentos, nevoa baixa e costa baixa.
- Alquimia, ervas raras e feras liminares.
- Rotas perigosas com baixa visibilidade.

Gameplay:
- coleta de reagentes;
- dungeons de cripta;
- criaturas de raiz;
- risco de emboscada por terreno.

## Estagio 5 - Drakholm e as Cinzas
Objetivo: abrir quarta cidade racial e a região queimada.

Escopo:
- Cidade racial: **Drakholm**.
- Ashlands, metal raro, fortalezas quebradas e memoria de guerras antigas.
- Regiões vermelhas mais proximas.

Gameplay:
- cerco;
- metal raro;
- cidadelas;
- primeiras ameaças de Dragões de Cinza.

## Estagio 6 - Coração Negro
Objetivo: liberar o centro preto da pangeia.

Escopo:
- Zona de Colapso principal.
- Ruínas da Primeira Âncora.
- Full loot sem punição.
- Bosses, Catalisadores raros e recursos máximos.
- Cidadelas centrais de alto valor.

Regras:
- Sem zona segura.
- Sem recompensa alta fora de risco alto.
- Eventos server-authoritative.

## Estagio 7 - Ilhas externas por teleporte
Objetivo: abrir ilhas antes do sistema naval completo.

Ordem sugerida:
1. **Frostvein**
2. **Glassmere**
3. **Rootwold**
4. **Ashkar**

Entrada:
- Teleportes antigos espalhados pela pangeia.
- Validação server-side de cooldown, combate recente, peso/carga e destino.

## Estagio 8 - Navegação
Objetivo: transformar o mar em gameplay real.

Entregas:
- Barcos.
- Portos.
- Rotas marítimas.
- Pirataria.
- Leviatãs de Névoa.
- Ilhas menores.
- Bloqueios naturais substituídos por risco real.

## Gate de cada estágio
Antes de liberar uma região:
- mapa carrega em Dedicated Server;
- sem dependência de HUD/widget/câmera;
- zonas de risco validadas no servidor;
- recursos e loot server-side;
- dungeons instanciadas quando aplicável;
- sem overflow de replicação;
- teste Standalone;
- teste Listen Server;
- teste Dedicated Server + 2 clientes;
- lag/loss quando houver teleporte, combate, dungeon, loot, RPC ou sistema net-critical.

## Critério de lançamento
O jogo pode lançar com:
- Aureth completa;
- parte da Fronteira de Aureth;
- zonas bloqueadas de forma diegética;
- mapa macro visível;
- promessa de liberação por regiões.

Não lançar com:
- mundo grande vazio;
- zonas de risco client-authoritative;
- recurso raro em zona verde;
- dungeon valiosa sem risco;
- bloqueio por parede invisível sem justificativa visual.
