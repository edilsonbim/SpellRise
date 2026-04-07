# SpellRise - Runbook de Deploy (AWS Windows/Linux)

Data de referencia: 2026-04-06

## 1. Objetivo

Padronizar operacao de servidor Dedicated para dois modos:
- Steam ON: autentica Steam e permite persistencia no banco.
- NoSteam: aceita cliente sem Steam e NAO persiste jogador/mundo.

## 2. Contrato atual do projeto

### 2.1 Steam ON (producao)
- `bRequireSteamAuthOnDedicatedServer=true`
- Cliente precisa entrar com `UniqueId` Steam valido.
- Persistencia pode usar `postgres` (recomendado) ou `file`.

### 2.2 NoSteam (teste/controlado)
- Server com `-nosteam` + `bEnableNoSteamTestingMode=true`
- Cliente pode conectar sem Steam.
- Persistencia e bloqueada em runtime:
  - `[Persistence][PreloadSkipped] Reason=nosteam_mode`
  - `[Persistence][ApplySkipped] Reason=nosteam_mode`
  - `[Persistence][SaveSkipped] Reason=nosteam_mode`
  - `[Persistence][LoadWorldSkipped] Reason=nosteam_mode`
  - `[Persistence][SaveWorldSkipped] Reason=nosteam_mode`

## 3. Banco de dados no servidor (modo Steam ON)

## 3.1 Recomendacao de arquitetura
- Game Server: EC2
- Banco: RDS PostgreSQL (nao co-localizar DB com servidor de jogo)

## 3.2 SQL inicial (RDS/PG)

```sql
CREATE DATABASE spellrise_db;
\c spellrise_db;

CREATE ROLE spellrise_app LOGIN PASSWORD 'TROCAR_SENHA_FORTE';
GRANT CONNECT ON DATABASE spellrise_db TO spellrise_app;

GRANT USAGE ON SCHEMA public TO spellrise_app;
GRANT CREATE ON SCHEMA public TO spellrise_app;

ALTER DEFAULT PRIVILEGES IN SCHEMA public
GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO spellrise_app;
```

Observacao: o provider cria tabelas automaticamente no primeiro boot valido.

## 3.3 Variavel de ambiente obrigatoria

`SR_PG_CONN` (no host do servidor):

```text
host=<RDS_ENDPOINT> port=5432 dbname=spellrise_db user=spellrise_app password=<SENHA> sslmode=require
```

## 4. Launch do servidor (Windows)

## 4.1 Steam ON + Postgres

```powershell
$env:SR_PG_CONN = "host=<RDS_ENDPOINT> port=5432 dbname=spellrise_db user=spellrise_app password=<SENHA> sslmode=require"

.\SpellRiseServer.exe /Game/Maps/Map2/Map2 -log -Port=7777 -NetDriverListenPort=7777 -unattended -NoCrashDialog -SRPersistenceProvider=postgres
```

## 4.2 NoSteam (sem persistencia)

```powershell
.\SpellRiseServer.exe /Game/Maps/Map2/Map2 -log -nosteam -Port=7777 -NetDriverListenPort=7777 -unattended -NoCrashDialog
```

## 5. Validacao de smoke (Windows local)

## 5.1 NoSteam (esperado PASS)

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Scripts\Run-Smoke-DS.ps1 -NoSteam -RequirePersistence -ReconnectClient1 -WithLagLoss -PktLag 120 -PktLoss 1
```

Esperado no summary:
- `OverallPass=True`
- `PersistencePreloadSkippedNoSteamCount >= 1`
- `PersistenceApplySkippedNoSteamCount >= 1`
- `PersistenceSaveSkippedNoSteamCount >= 1`

## 5.2 Steam ON (requer clientes Steam reais)

O smoke por `UnrealEditor-Cmd` local tende a entrar com `UniqueId=NULL` e falhar com `incompatible_unique_net_id`.
Para validar Steam ON:
1. Subir DS Steam ON.
2. Conectar clientes com Steam autenticada (build cliente empacotado, nao `-nosteam`).
3. Validar logs de `PreLoginAccept` e persistencia (`PreloadOk/PreloadMiss`, `SaveCharacterOk`).

## 6. Migracao AWS para Linux (recomendado)

## 6.1 Vantagens
- Menor custo por vCPU.
- Menor overhead operacional para server headless.
- Deploy/rollback simples por CLI (`rsync`, `systemd`).
- Melhor padrao para CI/CD de Dedicated Server.

## 6.2 Build Linux Server (Unreal Source)

Pre-requisito: toolchain Linux para UE 5.7 configurada no host de build.

Comando base (Windows host com Unreal Source):

```powershell
& "C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun `
  -project="C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" `
  -noP4 -server -serverplatform=Linux -serverconfig=Development `
  -cook -allmaps -build -stage -pak -archive `
  -archivedirectory="C:\Builds\SpellRiseLinuxServer"
```

## 6.3 Estrutura no Linux

```bash
sudo useradd -r -m -d /opt/spellrise -s /bin/bash spellrise
sudo mkdir -p /opt/spellrise/server /opt/spellrise/releases /opt/spellrise/shared
sudo chown -R spellrise:spellrise /opt/spellrise
```

## 6.4 Arquivo de ambiente

`/opt/spellrise/shared/spellrise.env`:

```bash
SR_PG_CONN="host=<RDS_ENDPOINT> port=5432 dbname=spellrise_db user=spellrise_app password=<SENHA> sslmode=require"
```

## 6.5 Service systemd (Steam ON)

`/etc/systemd/system/spellrise.service`:

```ini
[Unit]
Description=SpellRise Dedicated Server
After=network.target

[Service]
User=spellrise
WorkingDirectory=/opt/spellrise/server
EnvironmentFile=/opt/spellrise/shared/spellrise.env
ExecStart=/opt/spellrise/server/SpellRiseServer.sh /Game/Maps/Map2/Map2 -log -Port=7777 -NetDriverListenPort=7777 -unattended -SRPersistenceProvider=postgres
Restart=always
RestartSec=5
LimitNOFILE=1048576

[Install]
WantedBy=multi-user.target
```

Ativar:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now spellrise
sudo systemctl status spellrise
```

## 6.6 Deploy de nova versao por CLI

Do seu PC:

```bash
rsync -avz --delete ./WindowsServerOrLinuxServerPackage/ ubuntu@<EC2_IP>:/opt/spellrise/releases/build_<TAG>/
ssh ubuntu@<EC2_IP> "sudo -u spellrise ln -sfn /opt/spellrise/releases/build_<TAG> /opt/spellrise/server && sudo systemctl restart spellrise && sudo systemctl --no-pager status spellrise"
```

Rollback:

```bash
ssh ubuntu@<EC2_IP> "sudo -u spellrise ln -sfn /opt/spellrise/releases/build_<TAG_ANTERIOR> /opt/spellrise/server && sudo systemctl restart spellrise"
```

## 7. Checklist rapido de release

1. Build Source (`SpellRiseEditor`, `SpellRiseClient`, `SpellRiseServer`) sem erro.
2. Smoke NoSteam PASS (DS+2 reconnect + lag/loss).
3. Smoke Steam ON com cliente Steam real PASS.
4. Sem `FBitReader::SetOverflowed`.
5. Logs de auth/persistencia no modo correto do release.
6. Backup/snapshot do banco antes da virada.

## 8. Evidencia desta etapa (2026-04-06)

- Build: PASS (`SpellRiseEditor Win64 Development`)
- Smoke NoSteam: PASS
  - Summary: `Saved/Logs/SmokeAuto/2026-04-06_13-27-50/Smoke_Summary.txt`
- Smoke Steam local por EditorCmd: FAIL esperado por `UniqueId=NULL` / `incompatible_unique_net_id`
  - Summary: `Saved/Logs/SmokeAuto/2026-04-06_13-28-59/Smoke_Summary.txt`
