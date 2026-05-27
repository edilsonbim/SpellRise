import unreal

assets = [
    "/Game/UI/ChatSystem/W_Chat",
    "/Game/UI/Widgets/W_CastTimeBar",
    "/Game/UI/Widgets/PauseMenu/WB_PauseMenu",
    "/Game/UI/Widgets/W_Bow_Crosshair",
    "/Game/UI/InventorySystem/Interfaces/BPI_Interact",
    "/Game/Core/Player/SandboxCharacter_Mover1",
]

registry = unreal.AssetRegistryHelpers.get_asset_registry()
options = unreal.AssetRegistryDependencyOptions(
    include_soft_package_references=True,
    include_hard_package_references=True,
    include_searchable_names=True,
    include_soft_management_references=True,
    include_hard_management_references=True,
)

for asset in assets:
    referencers = registry.get_referencers(asset, options)
    unreal.log("REFERENCERS {}:".format(asset))
    for ref in sorted(str(r) for r in referencers):
        unreal.log("  {}".format(ref))
