# Hadal Industries — CLAUDE.md

Agent context for the **Hadal Industries** Unreal Engine 5.7 project. This file is loaded automatically by Claude Code on every session in this repo.

> **Read order for fresh sessions:** this file → `MVP_PLAN.md` (full design) → `~/.claude/projects/D--Github-hadal-industries/memory/MEMORY.md` and the most recent `project_m*_state.md` memory → `README.md` (player-facing).

---

## What this project is

A first-person 3D automation + exploration game set in the Mariana Trench. The MVP loop is:

> Explore → Scan → Build → Extract → Process → Upgrade → Descend → Discover

Single-player, Windows PC, UE 5.7. Core fantasy: build an industrial life-support system at the bottom of the world, where every machine must survive pressure, darkness, corrosion, wildlife, distance, and corporate bullshit.

The remote is `https://github.com/davidesidoti/hadal-industries`. Branch `main` is the only branch in use; commits are co-authored to Davide Sidoti `<sidotidavide@gmail.com>`.

---

## Milestone status (as of 2026-05-03)

| | Status | What works |
|---|---|---|
| **M0** Foundation | ✅ | `HI_*` C++ scaffolding, `LogHadalIndustries` log category, project module, `HI_GameMode/PlayerController/PlayerCharacter` shells, UnrealMCP plugin vendored. Variant_Horror/Shooter template removed. |
| **M1** Player + Interaction | ✅ | First-person camera + skeletal mesh, Enhanced Input (Move/Look/Jump/Vertical/Interact/Scan), swim mode via PhysicsVolume, `IHI_InteractableInterface`, `UHI_InteractionComponent` (camera line trace), `UHI_ScannerComponent` (sphere overlap pulse). `MVP_AbyssalTestSite` map populated with floor, lights, water volume, PlayerStart. |
| **M2** Inventory + Items | ✅ | `UHI_ItemDefinition` (UDataAsset), `UHI_InventoryComponent` (slot-based, `KnownItems` registry, MaxStackSize per item), `AHI_ResourceNode` harvests into player inventory, `Exec` console commands `GrantItem` and `DumpInventory`. `DA_Item_Manganese` is the first item. |
| **M3** Build system | ✅ | `UHI_BuildableDefinition` (UDataAsset with `BuildCost`, `ActorClass`, `PreviewMesh`), `AHI_BuildableActor` base, `UHI_BuildManagerComponent` with ghost preview (translucent `M_BuildGhost`, valid/invalid color tint), pivot-correct positioning, snap-to-grid (100 cm), collision sweep, cost consumption, demolish mode with red overlay highlight via material swap, 50% refund on demolish. 7 new IAs. Two buildables wired: Foundation Mk1 (free) and Extractor Mk1 (3× manganese). |
| **M4a** Machine base + Extractor | ✅ | `AHI_MachineBase` (Input/Output `UHI_InventoryComponent` subobjects, `FTimerHandle` production loop, virtual `TickProduction()`, `IsProductionAllowed()` accessor with stub `bPowerSatisfied`/`bPressureSafe`, drain Output via `E` through `IHI_InteractableInterface`, `OnPreDemolish` dumps Input+Output to player). `AHI_ExtractorMachine` (auto-binds nearest `AHI_ResourceNode` within `BindRadius=300cm`, auto-rebinds in `TickProduction`, calls `AHI_ResourceNode::TryConsumeYield`). `EHI_MachineState` enum. `BP_HI_Buildable_Extractor_Mk1` reparented to `AHI_ExtractorMachine`. Static `ActiveMachines` registry. |
| **M4b** Recipe + Fabricator | next | `UHI_RecipeDefinition` (UDataAsset), `URecipeProcessorComponent`, `AHI_FabricatorMachine`. Pressure Plate Mk1 first recipe. |
| M5 Power + Logistics | future | |
| M6 Pressure + Depth | future | `EHI_PressureTier` enum already in `HI_Types.h`. |
| M7 Threat + Creature | future | |
| M8 Narrative slice | future | |

---

## Source layout

C++ lives under `Source/HadalIndustries/`:

```
Public/
  Core/             HI_Log.h, HI_Types.h, HI_GameMode.h, HI_PlayerController.h, HI_PlayerCharacter.h
  Interaction/      HI_InteractableInterface.h, HI_InteractionComponent.h
  Scanning/         HI_ScannerComponent.h
  Resources/        HI_ResourceNode.h
  Inventory/        HI_ItemDefinition.h, HI_InventoryComponent.h
  BuildSystem/      HI_BuildableDefinition.h, HI_BuildableActor.h, HI_BuildManagerComponent.h
  (Power/Pressure/Recipes/Machines/Missions/Save are scaffolded as folders for later milestones)
Private/            mirror of Public/
HadalIndustries.{h,cpp,Build.cs}
HadalIndustries.Target.cs
HadalIndustriesEditor.Target.cs
```

`Build.cs` already includes `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `UMG`, `Slate`. **Do not** add modules unless the milestone genuinely needs them.

Content lives under `Content/HadalIndustries/` for project-specific assets (Maps/, Data/Items/, Data/Buildables/, Materials/) and `Content/Blueprints/` for the Blueprint child classes (BP_HI_GameMode, BP_HI_PlayerCharacter, BP_HI_ResourceNode_Manganese, Buildables/BP_HI_Buildable_*). The `Content/FirstPerson/`, `Content/Characters/Mannequins/`, `Content/DeepWaterStation/`, `Content/Input/` folders are template carryovers but actively referenced — keep them.

---

## Build workflow

**Editor closed → Build.bat (full build):**

```bash
"D:/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" HadalIndustriesEditor Win64 Development -Project="D:/Github/hadal-industries/HadalIndustries.uproject" -WaitMutex
```

Build logs: `C:/Users/sidot/AppData/Local/UnrealBuildTool/Log.txt`.

**Editor open → Live Coding (`Ctrl+Alt+F11` in the editor window):**
- Works for any change confined to `.cpp`, and for adding non-`UPROPERTY` member functions/variables.
- Does **not** work when adding new `UCLASS`, new `UPROPERTY` to an existing class, new components, or any change that alters the class layout. Those need a full rebuild with the editor closed.
- Do not call `Build.bat` while the editor is alive — it acquires the project mutex and fails. The error message specifically says "press Ctrl+Alt+F11 if iterating on code in the editor".

If `Build.bat` reports `Unable to build while Live Coding is active` even after closing the editor, check for orphan `UnrealEditor.exe` processes via `Get-Process UnrealEditor` and stop them.

---

## Working with the UnrealMCP plugin

The plugin is at `Plugins/UnrealMCP/` (vendored from `chongdashu/unreal-mcp`). The Python server is registered globally as MCP `unrealMCP` and is connected on every session — no per-project setup needed. UE 5.7 compatibility patches are already applied (re-apply them if upstream is ever pulled in fresh):

- `ANY_PACKAGE` removed in UE 5.1 — replaced all `FindObject<UClass>(ANY_PACKAGE, *Name)` with `FindFirstObject<UClass>(*Name, EFindFirstObjectOptions::EnsureIfAmbiguous)` in `UnrealMCPBlueprintCommands.cpp` and `UnrealMCPBlueprintNodeCommands.cpp`.
- `const int32 BufferSize` in `MCPServerRunnable.cpp` collides with UE template internals — renamed to `MCPBufferSize`.

### What the MCP plugin can do for you
- `get_actors_in_level` — query world state.
- `create_blueprint` for a small whitelist of parent classes (`AActor`, `APawn`, `ACharacter`, `AGameModeBase`, `UActorComponent`).
- `compile_blueprint`.
- `spawn_actor` / `set_actor_transform` for `StaticMeshActor` and `DirectionalLight`.
- A handful of UMG node creation calls.

### What it cannot do — switch to a manual editor workflow
- **`set_blueprint_property` does not work for inherited UPROPERTY** (only sees BP-added variables). It also rejects `ObjectProperty` (e.g. `UInputAction*`, `UInputMappingContext*`) and `ClassProperty` (`TSubclassOf<>`). Net effect: you cannot wire any of the project's `HI_*` components from MCP. **Always have the user assign these in Class Defaults manually.**
- **`create_blueprint` parent fallback**: any parent class outside the whitelist silently becomes `AActor`. Confirmed broken with `HI_PlayerCharacter`, `HI_GameMode`, `HI_ResourceNode`. **Have the user create those BPs via Content Browser → Blueprint Class → All Classes → search C++ class name.**
- **`spawn_actor` whitelist**: works for `StaticMeshActor` and `DirectionalLight`. Fails on `PlayerStart`, `SkyLight`, `SkyAtmosphere`, `ExponentialHeightFog`, `PhysicsVolume`, almost anything else. **Use the editor's Place Actors panel.**
- **`spawn_blueprint_actor` is unreliable** (timeouts observed).

When you hit one of these limits, do **not** loop on the error — switch immediately to a guided manual workflow with the user, give them precise click-by-click instructions.

---

## Conventions

- **Prefix `HI_`** on every project-specific type: `AHI_PlayerCharacter`, `UHI_InventoryComponent`, `FHI_ItemStack`, `EHI_PressureTier`, `IHI_InteractableInterface`.
- **Lowercase snake_case** for gameplay IDs in data assets: `res_manganese_nodule`, `item_pressure_plate_mk1`, `build_extractor_mk1`.
- **Single log category** `LogHadalIndustries` (declared in `Public/Core/HI_Log.h`). Use it consistently. **Do not redeclare** the category — it used to live in `HadalIndustries.h` and was deduplicated; do not reintroduce that decl.
- **Pattern**: C++ base class → BP child for visuals → Data Asset for tuning → Component for reusable logic → Subsystem for world-level coordination. See MVP_PLAN sez. 21, 25, 29.
- **Parameter naming**: `Interactor` not `Instigator` for interaction interface params — `AActor::Instigator` exists and shadows the parameter (`error C4458`). The interaction interface uses `Interactor`; preserve that.
- **Includes for collision queries**: any file using `FOverlapResult` needs `#include "Engine/OverlapResult.h"` in UE 5.7. The forward declaration in `WorldCollision.h` is not enough.
- **Pivot correctness**: when spawning meshes at runtime (e.g. ghost preview), engine `BasicShapes` have pivot at corner — use `LocalBox.GetCenter()` to position the geometric center, not the actor location.

---

## Search Fab before writing new code

Before implementing any new feature/system, search [https://www.fab.com/](https://www.fab.com/) for existing free or sub-€10 assets/plugins. Present results to the user and only proceed with custom code if nothing reusable exists. This already saved time on the deep-water environment art (`Content/DeepWaterStation/` is a Fab vendor).

---

## Commit & push

- Author all commits with `--author="Davide Sidoti <sidotidavide@gmail.com>"`. The local git config already sets this.
- Commits include `Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>` trailer.
- Conventional commit prefixes in use: `feat(m0)`, `feat(m1)`, `feat(m2)`, `feat(m3)`, `feat(m4a)`, `chore(m1)`. Continue this scheme for future milestones.
- Always commit with a HEREDOC body (multi-paragraph). Example layout used so far: short subject, blank line, bulleted summary of what changed.
- LFS is configured for `.uasset`, `.umap`, `.fbx`, `.psd`, `.jpg`. The first push of a milestone may be GB-scale — that is fine.
- **Never** commit unless the user has explicitly asked.

---

## M4 status

**M4a (Machine base + Extractor) shipped 2026-05-03.** Code lives at:

```
Public/Machines/HI_MachineBase.h           # AHI_MachineBase : AHI_BuildableActor + IHI_InteractableInterface
Public/Machines/HI_ExtractorMachine.h      # AHI_ExtractorMachine : AHI_MachineBase
Private/Machines/HI_MachineBase.cpp
Private/Machines/HI_ExtractorMachine.cpp
```

**`AHI_MachineBase` actual shape** — Input/Output `UHI_InventoryComponent` subobjects, `FTimerHandle` production loop (NOT actor Tick) calling virtual `TickProduction()` every `ProductionInterval` seconds. `IsProductionAllowed()` accessor reads `bPowerSatisfied`/`bPressureSafe` stub bools (replaced by component-driven checks in M5/M6). Implements `IHI_InteractableInterface` so `E` drains Output → Interactor inventory. Overrides `AHI_BuildableActor::OnPreDemolish` to dump Input + Output back to player on demolish. Static `ActiveMachines` registry for HUD/scanner/save reuse. `EHI_MachineState` enum lives in `Core/HI_Types.h`.

**`AHI_ExtractorMachine` actual shape** — `BindRadius=300cm`. BeginPlay calls `TryBindNearestNode` (filters `UGameplayStatics::GetAllActorsOfClass<AHI_ResourceNode>` by squared distance). `TickProduction` re-scans when `BoundNode` is invalid (handles the "place after node depleted" case). Yield consumption goes through `AHI_ResourceNode::TryConsumeYield(int32, FName&)` — encapsulates the `RemainingYield--` write. If output is full, the unit is credited back to the node so yield isn't lost.

**Demolish refund + dump-back.** `UHI_BuildManagerComponent::DemolishTargeted` now calls `Target->OnPreDemolish(PlayerInv)` before `Destroy()`, so machines return their inventory contents to the player in addition to the 50% build-cost refund.

### M4b (Recipe + Fabricator) — next slice

```
Public/Recipes/HI_RecipeDefinition.h        # UHI_RecipeDefinition : UDataAsset
Public/Recipes/HI_RecipeProcessorComponent.h # URecipeProcessorComponent : UActorComponent
Public/Machines/HI_FabricatorMachine.h       # AHI_FabricatorMachine : AHI_MachineBase
```

`UHI_RecipeDefinition` fields: `FName RecipeId`, `FText DisplayName`, `TArray<FHI_ItemStack> Inputs`, `Outputs`, `float CraftTimeSeconds`, `FName RequiredMachineType`. `URecipeProcessorComponent` validates inputs against InputInventory, consumes on craft begin, advances progress, deposits outputs on completion. `AHI_FabricatorMachine` selects a recipe (`UHI_RecipeDefinition* SelectedRecipe`); `TickProduction` delegates to the processor.

**Assets needed for M4b:**
- `DA_Item_Basalt`, `DA_Item_PressurePlateMk1`.
- `DA_Recipe_PressurePlateMk1` (3× manganese + 2× basalt → 1× pressure plate, 4 s).
- `BP_HI_ResourceNode_Basalt` (placeholder).
- `BP_HI_Buildable_Fabricator_Mk1` + `DA_Buildable_FabricatorMk1`.

**Architectural note for M4b implementer:**
- Live Coding will reject the new `UCLASS` declarations + new component subobjects (`URecipeProcessorComponent` on `AHI_FabricatorMachine`). Plan for one full rebuild with editor closed at the start of M4b.
- M5 (Power) will plug in via a `UHI_PowerConsumerComponent` attached to `AHI_MachineBase`, replacing the `bPowerSatisfied` stub behind `IsProductionAllowed()`.

---

## Quick recipes

**"I just opened the project and nothing works"** → `Build.bat` (editor must be closed first). Then open `HadalIndustries.uproject`. Default startup map is `MVP_AbyssalTestSite`.

**"I added a new C++ method, no UPROPERTY changes"** → Live Coding (`Ctrl+Alt+F11`) in the editor, watch the Output Log for "Patch creation successful". 5–10 s.

**"I added a new UPROPERTY / new UCLASS / new component"** → close the editor, run `Build.bat`, reopen.

**"I want to wire a new Input Action into BP_HI_PlayerCharacter"** → MCP cannot do this. Open the BP, Class Defaults → Hadal | Input → assign in Details. Compile + Save.

**"I want to spawn a non-trivial actor in MVP_AbyssalTestSite"** → MCP cannot do `PlayerStart`/`PhysicsVolume`/`SkyLight`/etc. Use Place Actors panel manually.

---

## Memory

Project memory lives at `~/.claude/projects/D--Github-hadal-industries/memory/`. The index is `MEMORY.md`. After each milestone, archive the previous milestone state file and write the new one (`project_m{N}_state.md`) with current code locations + verified behaviour.
