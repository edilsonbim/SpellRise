import unreal


LEVEL_PATH = "/Game/Hivemind/Medieval_Docks/Levels/PL_Medieval_Docks"


def main():
    if not unreal.EditorLevelLibrary.load_level(LEVEL_PATH):
        raise RuntimeError(f"Falha ao carregar {LEVEL_PATH}")

    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    rows = []
    min_x = min_y = min_z = float("inf")
    max_x = max_y = max_z = float("-inf")

    for actor in actors:
        label = actor.get_actor_label()
        cls = actor.get_class().get_name()
        origin, extent = actor.get_actor_bounds(False)
        if extent.x <= 0 and extent.y <= 0 and extent.z <= 0:
            continue

        ax0, ay0, az0 = origin.x - extent.x, origin.y - extent.y, origin.z - extent.z
        ax1, ay1, az1 = origin.x + extent.x, origin.y + extent.y, origin.z + extent.z
        min_x, min_y, min_z = min(min_x, ax0), min(min_y, ay0), min(min_z, az0)
        max_x, max_y, max_z = max(max_x, ax1), max(max_y, ay1), max(max_z, az1)
        rows.append((extent.x * extent.y * extent.z, label, cls, origin, extent))

    rows.sort(reverse=True, key=lambda r: r[0])

    unreal.log("SPELLRISE_DOCKS_BOUNDS_BEGIN")
    unreal.log(f"ActorCount={len(actors)}")
    unreal.log(f"BoundsMin=({min_x:.1f},{min_y:.1f},{min_z:.1f})")
    unreal.log(f"BoundsMax=({max_x:.1f},{max_y:.1f},{max_z:.1f})")
    unreal.log(f"BoundsSize=({max_x - min_x:.1f},{max_y - min_y:.1f},{max_z - min_z:.1f})")
    for _, label, cls, origin, extent in rows[:80]:
        unreal.log(
            f"Actor label='{label}' class='{cls}' "
            f"origin=({origin.x:.1f},{origin.y:.1f},{origin.z:.1f}) "
            f"extent=({extent.x:.1f},{extent.y:.1f},{extent.z:.1f})"
        )
    unreal.log("SPELLRISE_DOCKS_BOUNDS_END")


if __name__ == "__main__":
    main()
