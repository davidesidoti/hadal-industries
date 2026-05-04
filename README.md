# Hadal Industries

Hadal Industries is a first-person 3D automation and exploration game set in the Mariana Trench. The player is deployed by Pelagos Dynamics to build a deep-sea automated extraction network, manage pressure and power, explore hostile abyssal zones, and uncover what happened to previous failed expeditions.

This repository contains the Unreal Engine 5.7 MVP vertical slice. See [`MVP_PLAN.md`](MVP_PLAN.md) for the full design document.

---

## Status

Working towards an MVP whose loop is *Explore → Scan → Build → Extract → Process → Upgrade → Descend → Discover*.

| Milestone | Status |
|---|---|
| M0 — Foundation (modules, log, base classes) | ✅ |
| M1 — Player + Interaction (FP camera, swim, scanner) | ✅ |
| M2 — Inventory + Items (UDataAsset items, slot inventory) | ✅ |
| M3 — Build system (ghost preview, snap, demolish) | ✅ |
| M4a — Machine base + Extractor (auto-bind, timer production) | ✅ |
| M4b — Recipe + Fabricator (Pressure Plate Mk1) | ✅ |
| M5 — Power + Logistics | next |
| M6 — Pressure + Depth | |
| M7 — Threat + Creature | |
| M8 — Narrative slice | |

The map `MVP_AbyssalTestSite` is the active sandbox. Spawn into a flooded volume; use the player to harvest a `BP_HI_ResourceNode_Manganese` placeholder and build foundations and extractors.

---

## Requirements

- **Unreal Engine 5.7.x** installed at `D:/Epic Games/UE_5.7` (default for this project; adjust paths if different).
- **Visual Studio 2022 or newer** with C++ Game Development workload (UE 5.7 ships toolchains compatible up to VS 2026).
- **Git LFS** for `.uasset`, `.umap`, `.fbx`, `.psd`, `.jpg` (already configured in `.gitattributes`).

---

## Cloning

```bash
git clone https://github.com/davidesidoti/hadal-industries.git
cd hadal-industries
git lfs pull
```

The plugin `Plugins/UnrealMCP/` is vendored in-tree and includes UE 5.7 compatibility patches (`ANY_PACKAGE` removal and a `BufferSize` rename); no extra fetch needed.

---

## Building

When the editor is **closed**, use the build batch:

```bash
"D:/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" HadalIndustriesEditor Win64 Development -Project="D:/Github/hadal-industries/HadalIndustries.uproject" -WaitMutex
```

Build logs land in `C:/Users/sidot/AppData/Local/UnrealBuildTool/Log.txt`.

When the editor is **open**, prefer **Live Coding** with `Ctrl+Alt+F11`. It recompiles in-place without losing editor state. Live Coding cannot add new `UCLASS` declarations or change the layout of existing classes (new `UPROPERTY`, new components) — those still require closing the editor and running `Build.bat`.

---

## Running

1. Open `HadalIndustries.uproject` in UE 5.7.
2. The default startup level is `Content/HadalIndustries/Maps/MVP_AbyssalTestSite`.
3. Press **Play** (PIE).

---

## Controls (PIE)

### Locomotion
| Action | Key |
|---|---|
| Move | WASD |
| Look | Mouse |
| Jump (above water) / Ascend (in water, hold) | Space |
| Descend (in water, hold) | Left Ctrl |

### Interaction
| Action | Key |
|---|---|
| Interact / Harvest resource node | E |
| Sonar scan pulse | Q |

### Build mode
| Action | Key |
|---|---|
| Toggle build mode | B |
| Cycle buildable | Mouse Wheel |
| Rotate ghost (15° step) | R |
| Toggle snap-to-grid (100 cm) | Left Shift |
| Confirm placement | Left Mouse Button |
| Cancel build mode | Esc |

### Demolish mode
| Action | Key |
|---|---|
| Toggle demolish mode | X |
| Demolish targeted buildable (refund 50%) | Left Mouse Button |
| Cancel demolish mode | Esc |

### Debug console (`~`)
| Command | Description |
|---|---|
| `GrantItem <ItemId> <Qty>` | Add items to player inventory. Example: `GrantItem res_manganese_nodule 10` |
| `DumpInventory` | Log current inventory to Output Log |
| `GrantToTargetMachine <ItemId> <Qty>` | Line-trace forward; if hit is a machine, add items to its `InputInventory`. Unblocks fabricator testing pre-logistics. Example: `GrantToTargetMachine res_basalt 5` |

Filter the Output Log by `LogHadalIndustries` to see project events.

---

## Project structure

```
hadal-industries/
├── Config/                          # DefaultEngine.ini, DefaultGame.ini, ...
├── Content/
│   ├── Blueprints/                  # BP_HI_GameMode, BP_HI_PlayerCharacter, BP_HI_ResourceNode_Manganese, BP_HI_ResourceNode_Basalt
│   │   └── Buildables/              # BP_HI_Buildable_Foundation_Mk1, BP_HI_Buildable_Extractor_Mk1, BP_HI_Buildable_Fabricator_Mk1
│   ├── Characters/                  # Mannequin (template carryover, used by player BP)
│   ├── DeepWaterStation/            # Deep-water environment art (Fab vendored)
│   ├── HadalIndustries/
│   │   ├── Data/
│   │   │   ├── Items/               # DA_Item_Manganese, DA_Item_Basalt, DA_Item_PressurePlateMk1
│   │   │   ├── Recipes/             # DA_Recipe_PressurePlateMk1
│   │   │   └── Buildables/          # DA_Buildable_FoundationMk1, DA_Buildable_ExtractorMk1, DA_Buildable_FabricatorMk1
│   │   ├── Maps/                    # MVP_AbyssalTestSite
│   │   └── Materials/               # M_BuildGhost
│   └── Input/
│       ├── Actions/                 # IA_Move, IA_Look, IA_Jump, IA_Vertical, IA_Interact, IA_Scan, IA_ToggleBuild, IA_RotatePreview, IA_CycleBuildable, IA_ConfirmPlace, IA_CancelPlace, IA_ToggleSnap, IA_Demolish
│       ├── IMC_Default.uasset
│       └── IMC_MouseLook.uasset
├── Plugins/
│   └── UnrealMCP/                   # chongdashu/unreal-mcp + UE 5.7 compat patches
├── Source/
│   └── HadalIndustries/
│       ├── HadalIndustries.{h,cpp,Build.cs}
│       ├── Public/
│       │   ├── BuildSystem/         # HI_BuildableDefinition, HI_BuildableActor, HI_BuildManagerComponent
│       │   ├── Core/                # HI_Log, HI_Types, HI_GameMode, HI_PlayerController, HI_PlayerCharacter
│       │   ├── Interaction/         # HI_InteractableInterface, HI_InteractionComponent
│       │   ├── Inventory/           # HI_ItemDefinition, HI_InventoryComponent
│       │   ├── Machines/            # HI_MachineBase, HI_ExtractorMachine, HI_FabricatorMachine
│       │   ├── Recipes/             # HI_RecipeDefinition, HI_RecipeProcessorComponent
│       │   ├── Resources/           # HI_ResourceNode
│       │   └── Scanning/            # HI_ScannerComponent
│       └── Private/                 # mirror of Public/
├── CLAUDE.md                        # AI agent guide for this repo
├── MVP_PLAN.md                      # Full MVP design document
└── README.md                        # this file
```

Empty future-scope folders (`Public/Power`, `Public/Pressure`, `Public/Save`, `Public/Missions`) will be populated milestone by milestone.

---

## Coding conventions

- **Prefix `HI_`** on every project-specific type: `AHI_PlayerCharacter`, `UHI_InventoryComponent`, `FHI_ItemStack`, `EHI_PressureTier`, `IHI_InteractableInterface`.
- **Lowercase snake_case** for gameplay IDs in data: `res_manganese_nodule`, `item_pressure_plate_mk1`, `recipe_depth_module_mk1`, `build_extractor_mk1`.
- **Log category**: `LogHadalIndustries` (declared in `Source/HadalIndustries/Public/Core/HI_Log.h`). Use `UE_LOG` for actionable signals — avoid spam.
- **Architecture pattern**: C++ base class for behaviour, BP child for visuals/tuning, Data Asset for static configuration, Component for reusable logic, Subsystem for world-level coordination. See MVP_PLAN sez. 29–30.
- **C++ in core systems** (Inventory, BuildSystem, Recipes, Power, Pressure, Machines, Save). **BP for visual/UI/audio wiring** of those systems.

---

## Working with AI agents

This project is developed alongside Claude Code. The agent reads [`CLAUDE.md`](CLAUDE.md) and [`MVP_PLAN.md`](MVP_PLAN.md) for context. Memory persists in `~/.claude/projects/D--Github-hadal-industries/memory/`.

When you want to bring an agent up to speed on a fresh session: point it at `MVP_PLAN.md` first, then the latest `project_m*_state.md` memory, then `CLAUDE.md`.

---

## Reference links

- [Unreal Engine 5.7 release notes](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-7-release-notes)
- [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
- [Data-driven gameplay](https://dev.epicgames.com/documentation/unreal-engine/data-driven-gameplay-elements-in-unreal-engine)
- [NOAA Mariana Trench](https://www.ncei.noaa.gov/news/planet-postcard-mariana-trench)
- [USGS deep-ocean mineral resources](https://pubs.usgs.gov/publication/fs20253017/full)
