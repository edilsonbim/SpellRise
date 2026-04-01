# Build and Release Playbook - SpellRise

## Objetivo
Padronizar build, cook, package e validação inicial de Client/Server para ambiente de produção técnica.

## Política de engine
- Usar somente `C:\UnrealSource\UnrealEngine`
- Não validar este projeto com engine do launcher
- Fechar o editor antes da build quando necessário

## Artefatos alvo
- `SpellRiseEditor`
- `SpellRiseClient`
- `SpellRiseServer`

## Sequência operacional recomendada
### 1. Sanidade pré-build
Validar:
- branch/commit definido
- config crítica conhecida
- mapa de entrada correto
- perfil de online/auth correto para o cenário
- sem editor/processo preso que cause lock de DLL

### 2. Build
Executar:
- `SpellRiseEditor Win64 Development`
- `SpellRiseClient Win64 Development`
- `SpellRiseServer Win64 Development`

Aceite:
- build verde
- sem warning crítico novo no recorte

### 3. Cook e package
Gerar no mesmo ciclo:
- package do Client
- package do Server

Regras:
- mesmo commit
- mesmo conjunto de configs de runtime
- sem trocar ini/manualmente entre os dois artefatos
- sem misturar build antiga de um lado e nova do outro

### 4. Paridade obrigatória Client/Server
Conferir:
- mapa padrão
- travel/entry map
- assets críticos do fluxo testado
- config de online/auth
- config de network relevante
- cook/package do mesmo recorte

### 5. Validação inicial pós-package
Executar pelo menos:
- boot do Client
- boot do Server
- conexão Client -> Server
- spawn inicial
- PlayerController/HUD/local runtime sem erro crítico

## Comando padrão de build
```powershell
C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat SpellRiseEditor Win64 Development "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" -WaitMutex -NoHotReloadFromIDE
```

## Quando a mudança tocar multiplayer/runtime
Além da build/package, executar também:
- DS+2 normal
- DS+2 reconnect
- DS+2 lag/loss

## Bloqueadores de release técnica
- package Client sem package Server correspondente
- package de commits diferentes
- mismatch de cook/config entre Client e Server
- overflow/serialization error no PlayerController path
- falha de bootstrap Steam/auth no DS quando o cenário exigir Steam
- erro crítico de login, spawn, restart ou HUD bind no runtime local

## Checklist curto de fechamento
- [ ] Build Editor OK
- [ ] Build Client OK
- [ ] Build Server OK
- [ ] Package Client OK
- [ ] Package Server OK
- [ ] Paridade Client/Server validada
- [ ] Boot inicial OK
- [ ] Conexão inicial OK
- [ ] Logs arquivados
- [ ] Gate de smoke executado quando aplicável
