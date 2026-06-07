import unreal


LEVEL_PATH = "/Game/Maps/SpellRiseWorld/LV_Aureth"
CUBE_PATH = "/Engine/BasicShapes/Cube.Cube"


MARKERS = [
    ("POI_Aureth_City", -245000.0, -35000.0, 14000.0, 58.0, 58.0, 20.0),
    ("POI_Aureth_Harbor", -305000.0, -52000.0, 6500.0, 46.0, 34.0, 10.0),
    ("POI_FirstVillage", -20000.0, 85000.0, 13500.0, 42.0, 42.0, 13.0),
    ("POI_StarterDungeon", 80000.0, -105000.0, 17500.0, 38.0, 38.0, 18.0),
    ("POI_YellowZoneGate", 130000.0, 125000.0, 15000.0, 60.0, 24.0, 15.0),
    ("Blocker_NorthHighlands", 20000.0, -320000.0, 30000.0, 210.0, 30.0, 30.0),
    ("Blocker_EastForest", 305000.0, 65000.0, 24000.0, 34.0, 220.0, 26.0),
]


def load_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"Asset nao encontrado: {path}")
    return asset


def spawn_cube(static_mesh, name, x, y, z, sx, sy, sz):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor,
        unreal.Vector(x, y, z),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label(name)
    actor.set_actor_scale3d(unreal.Vector(sx, sy, sz))
    component = actor.static_mesh_component
    component.set_static_mesh(static_mesh)
    component.set_mobility(unreal.ComponentMobility.STATIC)
    return actor


def main():
    if not unreal.EditorLevelLibrary.load_level(LEVEL_PATH):
        raise RuntimeError(f"Falha ao carregar level: {LEVEL_PATH}")

    cube = load_asset(CUBE_PATH)

    # Remove marcadores antigos para o script ser reexecutavel.
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        label = actor.get_actor_label()
        if label.startswith("POI_") or label.startswith("Blocker_") or label == "REF_Aureth_SeaPlane":
            unreal.EditorLevelLibrary.destroy_actor(actor)

    for marker in MARKERS:
        spawn_cube(cube, *marker)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Aureth blockout markers reposicionados.")


if __name__ == "__main__":
    main()
