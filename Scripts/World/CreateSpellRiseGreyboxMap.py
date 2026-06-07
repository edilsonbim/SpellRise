import unreal


MAP_PATH = "/Game/Maps/SpellRiseWorld/LV_SpellRise_Greybox"
CUBE_PATH = "/Engine/BasicShapes/Cube.Cube"


def load_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"Asset nao encontrado: {path}")
    return asset


def spawn_cube(static_mesh, name, location, scale, material=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor,
        unreal.Vector(location[0], location[1], location[2]),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label(name)
    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    component = actor.static_mesh_component
    component.set_static_mesh(static_mesh)
    component.set_mobility(unreal.ComponentMobility.STATIC)
    if material:
        component.set_material(0, material)
    return actor


def spawn_marker(static_mesh, name, location, radius_m, height_m):
    # Markers usam cubos simples para manter o greybox leve e editavel.
    return spawn_cube(
        static_mesh,
        name,
        (location[0], location[1], height_m * 50.0),
        (radius_m, radius_m, height_m),
    )


def create_map():
    unreal.EditorLoadingAndSavingUtils.new_blank_map(False)

    cube = load_asset(CUBE_PATH)

    # 2 km x 2 km jogaveis. A escala parte do cubo padrao de 100 cm.
    spawn_cube(cube, "GB_2km_Playable_Floor", (0.0, 0.0, -25.0), (2000.0, 2000.0, 0.5))

    # Bordas altas para leitura rapida do limite do greybox.
    spawn_cube(cube, "GB_North_Border", (0.0, 100000.0, 500.0), (2000.0, 20.0, 10.0))
    spawn_cube(cube, "GB_South_Border", (0.0, -100000.0, 500.0), (2000.0, 20.0, 10.0))
    spawn_cube(cube, "GB_East_Border", (100000.0, 0.0, 500.0), (20.0, 2000.0, 10.0))
    spawn_cube(cube, "GB_West_Border", (-100000.0, 0.0, 500.0), (20.0, 2000.0, 10.0))

    # POIs pensados para full loot PvP: risco, rotas de fuga e conflito por recurso.
    spawn_marker(cube, "POI_Central_Ruins_Catalyst", (0.0, 0.0, 0.0), 90.0, 8.0)
    spawn_marker(cube, "POI_North_Forest_Ambush", (0.0, 62000.0, 0.0), 120.0, 5.0)
    spawn_marker(cube, "POI_East_Mining_Ridge", (65000.0, 10000.0, 0.0), 100.0, 12.0)
    spawn_marker(cube, "POI_South_Coast_Extraction", (-15000.0, -68000.0, 0.0), 110.0, 4.0)
    spawn_marker(cube, "POI_West_Swamp_RareHerbs", (-65000.0, -5000.0, 0.0), 95.0, 3.0)

    # Rotas principais. Mantidas largas para testar leitura de combate free target.
    spawn_cube(cube, "Route_NorthSouth_Main", (0.0, 0.0, 15.0), (16.0, 1500.0, 0.3))
    spawn_cube(cube, "Route_EastWest_Main", (0.0, 0.0, 20.0), (1500.0, 16.0, 0.3))
    spawn_cube(cube, "Route_RiskShortcut_NE_SW", (0.0, 0.0, 30.0), (12.0, 1500.0, 0.3)).set_actor_rotation(
        unreal.Rotator(0.0, 0.0, 42.0),
        False,
    )
    spawn_cube(cube, "Route_RiskShortcut_NW_SE", (0.0, 0.0, 35.0), (12.0, 1500.0, 0.3)).set_actor_rotation(
        unreal.Rotator(0.0, 0.0, -42.0),
        False,
    )

    # Obstaculos de combate em torno da ruina central.
    for index, point in enumerate([
        (-18000.0, 12000.0, 0.0),
        (16000.0, 14000.0, 0.0),
        (-14000.0, -16000.0, 0.0),
        (19000.0, -12000.0, 0.0),
        (0.0, 23000.0, 0.0),
        (0.0, -24000.0, 0.0),
    ], start=1):
        spawn_cube(cube, f"GB_Central_Cover_{index:02d}", (point[0], point[1], 200.0), (8.0, 18.0, 4.0))

    # Spawns separados para smoke DS+2 sem vantagem imediata.
    player_start_a = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(-45000.0, -45000.0, 120.0),
        unreal.Rotator(0.0, 45.0, 0.0),
    )
    player_start_a.set_actor_label("PlayerStart_SouthWest")

    player_start_b = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(45000.0, 45000.0, 120.0),
        unreal.Rotator(0.0, -135.0, 0.0),
    )
    player_start_b.set_actor_label("PlayerStart_NorthEast")

    light = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0.0, 0.0, 50000.0),
        unreal.Rotator(-45.0, -35.0, 0.0),
    )
    light.set_actor_label("Lighting_Directional_Sun_Greybox")

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SkyAtmosphere,
        unreal.Vector(0.0, 0.0, 0.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    ).set_actor_label("Lighting_SkyAtmosphere_Greybox")

    unreal.EditorAssetLibrary.make_directory("/Game/Maps/SpellRiseWorld")
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not unreal.EditorLoadingAndSavingUtils.save_map(world, MAP_PATH):
        raise RuntimeError(f"Falha ao salvar mapa: {MAP_PATH}")

    unreal.log(f"SpellRise greybox salvo em {MAP_PATH}")


if __name__ == "__main__":
    create_map()
