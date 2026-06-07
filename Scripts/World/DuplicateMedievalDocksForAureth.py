import unreal


SOURCE_ROOT = "/Game/Hivemind/Medieval_Docks/Levels"
TARGET_ROOT = "/Game/Maps/SpellRiseWorld/Aureth/Medieval_Docks/Levels"


def main():
    unreal.EditorAssetLibrary.make_directory("/Game/Maps/SpellRiseWorld/Aureth")
    unreal.EditorAssetLibrary.make_directory("/Game/Maps/SpellRiseWorld/Aureth/Medieval_Docks")

    if not unreal.EditorAssetLibrary.does_directory_exist(SOURCE_ROOT):
        raise RuntimeError(f"Diretorio fonte nao encontrado: {SOURCE_ROOT}")

    if unreal.EditorAssetLibrary.does_directory_exist(TARGET_ROOT):
        unreal.log_warning(f"Destino ja existe; mantendo conteudo existente: {TARGET_ROOT}")
        return

    if not unreal.EditorAssetLibrary.duplicate_directory(SOURCE_ROOT, TARGET_ROOT):
        raise RuntimeError(f"Falha ao duplicar {SOURCE_ROOT} para {TARGET_ROOT}")

    unreal.EditorAssetLibrary.save_directory(TARGET_ROOT, only_if_is_dirty=False, recursive=True)
    unreal.log(f"Medieval Docks duplicado para Aureth: {TARGET_ROOT}")


if __name__ == "__main__":
    main()
