# Hadal Industries - MVP Plan

> Working title: Hadal Industries
> Genre: first-person 3D automation, exploration, light survival, industrial sci-fi
> Engine: Unreal Engine 5.7.x
> Target platform for MVP: Windows PC
> Repository purpose: technical and design context for humans and AI coding agents

---

## 1. One-line pitch

A first-person automation game set in the Mariana Trench where the player builds a modular deep-sea factory, explores increasingly dangerous depths, extracts resources, manages pressure and power, and uncovers the mystery behind previous failed expeditions.

---

## 2. Core fantasy

The player is not just building a factory underwater.

The player is building an industrial life-support system at the bottom of the world, where every machine must survive pressure, darkness, corrosion, wildlife, distance and corporate bullshit.

The game should feel like:

- Satisfactory for automation and factory satisfaction
- Subnautica for underwater wonder and fear
- SOMA for isolation and mystery
- Barotrauma for pressure, damage and systems failing at the worst possible moment
- Alien for industrial sci-fi mood

---

## 3. MVP goal

The MVP must prove the core loop:

1. Explore a dark underwater test zone.
2. Scan for resource nodes.
3. Build a small pressurized base.
4. Place machines on valid terrain.
5. Extract three resources.
6. Move resources through a simple logistics system.
7. Process resources into components.
8. Use components to unlock a depth upgrade.
9. Experience one environmental threat.
10. Reach a deeper zone and trigger the first narrative hook.

The MVP is successful if a player can play for 30 to 60 minutes and understand the promise of the full game.

---

## 4. MVP non-goals

Do not build these in the MVP:

- Multiplayer
- Full open world
- Full creature AI ecosystem
- Full story campaign
- Combat system
- Complex base interiors
- Full power grid simulation
- Procedural terrain generation
- Large item catalogue
- Advanced conveyor belt system
- Complex vehicle customization
- Full save migration/versioning
- Photo-realistic art pass

The MVP should be ugly but playable, not beautiful but dead. Classic indie survival strategy: duct tape first, beauty later.

---

## 5. Target experience

### Player perspective

First-person.

The player controls a pressure suit or small personal submersible drone hybrid. For MVP, use one controllable character with underwater movement.

### MVP tone

- Industrial
- Lonely
- Slightly horror
- Mostly functional
- Mystery in the background, not full horror upfront

### Session structure

A typical MVP session:

1. Player wakes inside the initial Deep Anchor module.
2. Mission Control gives objective: establish a test extraction line.
3. Player exits into the dark water.
4. Player scans nearby resources.
5. Player places an extractor on a manganese node.
6. Player builds power cable and cargo tube.
7. Player routes manganese to a fabricator.
8. Player crafts pressure plates.
9. Player upgrades a module to survive deeper pressure.
10. Player descends to a lower shelf.
11. Sonar detects an impossible signal.
12. Demo ends with a wreck light blinking in the distance.

---

## 6. MVP pillars

### Pillar 1: Depth is progression

The world should be structured vertically. Depth replaces traditional biome level gates.

Depth affects:

- Build validity
- Pressure damage
- Available resources
- Lighting and visibility
- Creature behavior
- Equipment requirements
- Narrative triggers

### Pillar 2: Automation under hostile conditions

Factories are not only about ratios. They must survive the environment.

Systems to simulate in MVP:

- Power
- Resource extraction
- Item transfer
- Machine recipes
- Pressure tier
- Machine damage from environmental events

### Pillar 3: Darkness as gameplay

The player should not see everything.

Visibility tools:

- Flashlight
- Build lights
- Sonar pulse
- Resource scanner
- Beacon markers

Darkness should not only be visual style. It should affect exploration, tension and navigation.

### Pillar 4: Scientific grounding with fictional escalation

Use real concepts as a base:

- Polymetallic nodules
- Hydrothermal vents
- Chemosynthetic ecosystems
- Extreme pressure
- Sonar mapping

Then add fictional late-game material:

- Hadalite
- Signal-corrupted machinery
- Bio-mineral growths

---

## 7. MVP world

### Map name

`MVP_AbyssalTestSite`

### Map size

Small vertical slice.

Recommended dimensions:

- Horizontal playable area: 600 m x 600 m
- Vertical range: 0 m to -1200 m relative to initial anchor
- Actual Unreal units: choose 1 Unreal unit = 1 cm, but use gameplay depth as an abstract value stored separately

### Zones

#### Zone A: Deployment Shelf

Depth: 0 to 300 m in gameplay terms

Purpose:

- Tutorial
- Initial base
- First extraction
- Safe building area

Resources:

- Manganese Nodules
- Basalt

Threat level: none or very low

#### Zone B: Thermal Scar

Depth: 300 to 800 m

Purpose:

- First power expansion
- Introduce sulfur and hydrothermal vent
- Introduce heat hazard

Resources:

- Sulfur
- Copper Sulfide
- Thermal energy source

Threat level: medium

#### Zone C: Signal Drop

Depth: 800 to 1200 m

Purpose:

- End of MVP
- First narrative anomaly
- First wreck discovery
- First creature event

Resources:

- Hadalite Trace, only collectible as sample

Threat level: scripted event

---

## 8. Core gameplay loop

### Macro loop

Explore -> Scan -> Build -> Extract -> Process -> Upgrade -> Descend -> Discover

### Micro loop

1. Player receives an objective.
2. Player scans terrain.
3. Player identifies resource node.
4. Player places extractor.
5. Player connects power.
6. Player connects logistics.
7. Machine starts producing.
8. Player routes output to storage or processor.
9. Player crafts required upgrade.
10. Player unlocks deeper construction tier.

---

## 9. MVP resources

### Resource list

| Resource ID | Display name | Source | Used for |
|---|---|---|---|
| `res_manganese_nodule` | Manganese Nodule | Nodules on seabed | Basic plates, pressure casing |
| `res_basalt` | Basalt | Rock deposits | Foundations, concrete substitute |
| `res_copper_sulfide` | Copper Sulfide | Thermal Scar deposits | Cables, circuits |
| `res_sulfur` | Sulfur | Hydrothermal vents | Chemical processor, batteries |
| `res_hadalite_trace` | Hadalite Trace | Signal Drop anomaly | Narrative sample, end demo trigger |

### Processed items

| Item ID | Display name | Recipe |
|---|---|---|
| `item_pressure_plate_mk1` | Pressure Plate Mk1 | Manganese Nodule x3 + Basalt x2 |
| `item_copper_wire` | Copper Wire | Copper Sulfide x2 |
| `item_basic_circuit` | Basic Circuit | Copper Wire x2 + Pressure Plate Mk1 x1 |
| `item_thermal_cell` | Thermal Cell | Sulfur x2 + Copper Wire x1 |
| `item_depth_module_mk1` | Depth Module Mk1 | Pressure Plate Mk1 x5 + Basic Circuit x2 + Thermal Cell x1 |

---

## 10. MVP machines

### Deep Anchor

The initial hub.

Responsibilities:

- Player spawn
- Main storage
- Manual crafting
- Objective terminal
- Surface communication
- Tier unlocks

MVP behavior:

- Static actor placed in map
- Has inventory
- Has mission terminal interaction
- Can unlock Depth Module Mk1 when required items are deposited

### Resource Extractor Mk1

Extracts from a valid resource node.

Inputs:

- Power

Outputs:

- Raw resource item

Rules:

- Must be placed on compatible node
- Must be within allowed pressure tier
- Must have power connection
- Produces one item every N seconds

### Cargo Tube Mk1

Simplified logistics link between two machines.

MVP simplification:

- Do not simulate physical items moving yet
- Use a timed transfer tick
- One source, one destination
- Transfer rate: items per minute

Future visual polish:

- Add capsules moving through transparent tubes

### Fabricator Mk1

Processes resources into components.

Inputs:

- Power
- Recipe ingredients

Outputs:

- Crafted item

Rules:

- One selected recipe
- Internal input inventory
- Internal output inventory
- Progress bar

### Thermal Generator Mk1

Generates power near a hydrothermal vent.

Inputs:

- Valid vent source nearby

Outputs:

- Power units

Rules:

- Must be placed within vent radius
- Heat hazard around vent damages player if too close
- Produces more power than basic generator

### Sonar Beacon

Reveals nearby resources and points of interest.

Inputs:

- Power

Outputs:

- Map markers

Rules:

- Pulse every N seconds
- Adds temporary or permanent markers
- Can detect Signal Drop anomaly at deeper zone

---

## 11. MVP build system

### Build mode

Player can open build mode and place ghost previews.

Inputs:

- Toggle build mode
- Rotate preview
- Confirm placement
- Cancel placement
- Cycle build category

### Placement rules

A buildable can be placed only if:

- Terrain below is valid
- There is enough space
- Player has required items
- Current depth is within machine pressure limit
- Optional: slope is below max slope angle

### Build categories

For MVP:

- Foundation
- Power
- Extraction
- Logistics
- Processing
- Utility

### Required buildables

| Buildable | Category |
|---|---|
| Foundation Panel Mk1 | Foundation |
| Power Cable Mk1 | Power |
| Resource Extractor Mk1 | Extraction |
| Cargo Tube Mk1 | Logistics |
| Fabricator Mk1 | Processing |
| Thermal Generator Mk1 | Power |
| Sonar Beacon Mk1 | Utility |
| Flood Light Mk1 | Utility |

---

## 12. Pressure system

### Concept

Every machine has a pressure rating. Every zone has a pressure demand.

If zone pressure is higher than machine rating:

- Machine works at reduced efficiency for a short grace period
- Machine then starts taking damage
- At zero durability, it shuts down

### MVP pressure tiers

| Tier | Max gameplay depth | Notes |
|---|---:|---|
| `pressure_tier_0` | 300 m | Starting shelf |
| `pressure_tier_1` | 800 m | Requires Depth Module Mk1 |
| `pressure_tier_2` | 1200 m | End of MVP, limited access only |

### MVP simplification

Do not calculate real-world pressure. Use depth bands.

### Required component

`UPressureComponent`

Properties:

- `PressureTier`
- `MaxSafeDepth`
- `CurrentDepth`
- `bIsOverPressure`
- `OverPressureDamagePerSecond`
- `GracePeriodSeconds`

---

## 13. Power system

### Concept

Power is local-network based.

MVP simplification:

- Machines connect to nearest powered network through cable actors
- Power network calculates total generation and total consumption
- If consumption exceeds generation, machines receive reduced efficiency

### Required classes

- `APowerProducer`
- `APowerConsumer`
- `APowerCable`
- `UPowerComponent`
- `UPowerNetworkSubsystem`

### Power rules

- Deep Anchor provides starter power
- Thermal Generator adds power
- Cables connect machines
- No batteries in MVP unless easy to add

### MVP UI

Show:

- Power available
- Power consumed
- Network status: stable, overloaded, disconnected

---

## 14. Inventory and item system

### MVP approach

Use C++ structs and Data Assets or Data Tables for item definitions.

### Item definition fields

`FItemDefinition`

- `ItemId`
- `DisplayName`
- `Description`
- `Icon`
- `MaxStackSize`
- `bIsRawResource`
- `bIsCraftedComponent`

### Inventory slot

`FInventorySlot`

- `ItemId`
- `Quantity`

### Inventory component

`UInventoryComponent`

Responsibilities:

- Add item
- Remove item
- Check if item exists
- Transfer item
- Get available space
- Serialize for save game

### Inventories needed

- Player inventory
- Deep Anchor inventory
- Machine input inventory
- Machine output inventory

---

## 15. Recipe system

### Recipe definition fields

`FRecipeDefinition`

- `RecipeId`
- `DisplayName`
- `InputItems`
- `OutputItems`
- `CraftTimeSeconds`
- `RequiredMachineType`

### Recipe processor

`URecipeProcessorComponent`

Responsibilities:

- Validate ingredients
- Consume ingredients
- Advance crafting progress
- Produce output
- Pause if no power
- Pause if output inventory full

### MVP recipes

- Manganese Nodule + Basalt -> Pressure Plate Mk1
- Copper Sulfide -> Copper Wire
- Copper Wire + Pressure Plate -> Basic Circuit
- Sulfur + Copper Wire -> Thermal Cell
- Pressure Plate + Basic Circuit + Thermal Cell -> Depth Module Mk1

---

## 16. Exploration and scanning

### Scanner modes

MVP scanner has one mode:

- Resource scan

Future modes:

- Geological scan
- Biological scan
- Signal scan
- Structural scan

### Scanner behavior

When used:

- Sends pulse sphere around player
- Highlights nearby resource nodes
- Adds markers to HUD
- Plays sonar sound
- Briefly lights suspended particles or terrain silhouettes

### Sonar Beacon behavior

When powered:

- Repeats scan in fixed radius
- Reveals resources on local map
- Detects anomaly if player has reached Zone C objective

---

## 17. Environmental threat MVP

### Threat name

`Thermal Surge`

### Purpose

Introduce the idea that the factory is vulnerable.

### Trigger

After player builds the first Thermal Generator.

### Behavior

- Warning from Deep Anchor
- Hydrothermal vent intensity increases
- Nearby machines take heat stress
- One cargo tube or cable gets damaged
- Player must repair it using Pressure Plate Mk1 or Copper Wire

### Repair mechanic

Interact with damaged machine while holding required item.

### MVP implementation

A scripted event manager can trigger this. Do not build a full dynamic event system yet.

---

## 18. Creature MVP

### Creature name

`Abyssal Stalker`

### Role

Atmosphere and light threat, not combat.

### Behavior

- Appears at edge of light cone
- Avoids direct player approach
- Gets attracted by active flood lights
- Emits sonar-distorting pulse
- Triggers end-demo scare near Signal Drop

### MVP implementation

Use simple AI:

- Patrol spline
- Move to light source if active
- Retreat if player gets close
- Trigger visual/audio event
- No damage needed in first MVP

---

## 19. Narrative MVP

### Core premise

The player works for Pelagos Dynamics, a deep-sea industrial research corporation.

The mission is officially about mineral extraction and automation research.

The hidden purpose is Hadalite detection.

### Characters

#### Player

A field automation engineer sent as a solo operator.

#### Mission Control

Corporate voice. Calm, friendly, manipulative.

#### HADAL-3 Logs

Recovered messages from a previous expedition.

### MVP story beats

1. Wake up in Deep Anchor.
2. Establish first production line.
3. Build Thermal Generator.
4. Detect unstable readings.
5. Unlock Depth Module Mk1.
6. Descend to Signal Drop.
7. Find wreckage from HADAL-3.
8. Collect Hadalite Trace.
9. Sonar receives repeating pattern.
10. Demo ends with message: "Signal source is below authorized depth."

### Example Mission Control tone

"Operator, your productivity is within acceptable survival margins. Continue deployment."

Yes, the corporation should sound like it would put a KPI on your oxygen consumption.

---

## 20. UI MVP

### HUD elements

- Crosshair
- Depth meter
- Pressure warning
- Oxygen/energy placeholder, optional
- Current objective
- Hotbar
- Selected buildable
- Interaction prompt
- Power network warning

### Build UI

- Build categories
- Selected machine info
- Required materials
- Pressure tier warning
- Invalid placement reason

### Machine UI

Each machine panel should show:

- Machine name
- Power status
- Pressure status
- Inventory
- Current recipe if applicable
- Production progress
- Output blocked warning

### Scanner UI

- Pulse effect
- Highlight resources
- Marker labels
- Distance indicator

---

## 21. Technical architecture

### Recommended Unreal approach

Use C++ for core systems and expose configuration to Blueprint.

Use Blueprint for:

- Visual setup
- Prototyping interactions
- UI layout
- Audio/VFX event wiring
- Simple scripted sequences

Use C++ for:

- Inventory
- Items
- Recipes
- Power networks
- Pressure
- Machine base classes
- Save game
- Build validation
- Interaction interfaces

### Key Unreal systems to use

- Gameplay Framework: GameMode, PlayerController, Pawn or Character
- Actor Components for reusable behavior
- Enhanced Input for player controls
- Data Assets or Data Tables for item and recipe definitions
- SaveGame classes for persistence
- World Partition later, but MVP can start with one persistent level

---

## 22. Proposed source structure

```text
Source/HadalIndustries/
  Public/
    Core/
      HI_Types.h
      HI_GameMode.h
      HI_PlayerController.h
      HI_PlayerCharacter.h
    Interaction/
      HI_InteractableInterface.h
      HI_InteractionComponent.h
    Inventory/
      HI_ItemTypes.h
      HI_ItemDefinition.h
      HI_InventoryComponent.h
    Recipes/
      HI_RecipeDefinition.h
      HI_RecipeProcessorComponent.h
    BuildSystem/
      HI_BuildableDefinition.h
      HI_BuildManagerComponent.h
      HI_BuildableActor.h
      HI_PlacementValidator.h
    Power/
      HI_PowerComponent.h
      HI_PowerProducerComponent.h
      HI_PowerConsumerComponent.h
      HI_PowerCableActor.h
      HI_PowerNetworkSubsystem.h
    Pressure/
      HI_PressureComponent.h
    Machines/
      HI_MachineBase.h
      HI_ExtractorMachine.h
      HI_FabricatorMachine.h
      HI_ThermalGenerator.h
      HI_SonarBeacon.h
    Resources/
      HI_ResourceNode.h
    Scanning/
      HI_ScannerComponent.h
    Missions/
      HI_MissionTypes.h
      HI_MissionSubsystem.h
    Save/
      HI_SaveGame.h
      HI_SaveSubsystem.h
  Private/
    Same folders as Public
```

---

## 23. Proposed content structure

```text
Content/HadalIndustries/
  Blueprints/
    Player/
    Machines/
    Buildables/
    Resources/
    Environment/
    Creatures/
  Data/
    Items/
    Recipes/
    Buildables/
    Missions/
  UI/
    Widgets/
    Materials/
  Maps/
    MVP_AbyssalTestSite
  Materials/
    Environment/
    Industrial/
    UI/
  Meshes/
    Blockout/
    Machines/
    Props/
  Audio/
    Sonar/
    Ambience/
    Machines/
    VoiceTemp/
  VFX/
    Particles/
    Niagara/
```

---

## 24. Data definitions

### Item Data Asset

Create `UHI_ItemDefinition`.

Fields:

```cpp
FName ItemId;
FText DisplayName;
FText Description;
TSoftObjectPtr<UTexture2D> Icon;
int32 MaxStackSize;
bool bIsRawResource;
bool bIsCraftedComponent;
```

### Recipe Data Asset

Create `UHI_RecipeDefinition`.

Fields:

```cpp
FName RecipeId;
FText DisplayName;
TArray<FHI_ItemStack> Inputs;
TArray<FHI_ItemStack> Outputs;
float CraftTimeSeconds;
FName RequiredMachineType;
```

### Buildable Data Asset

Create `UHI_BuildableDefinition`.

Fields:

```cpp
FName BuildableId;
FText DisplayName;
TSubclassOf<AActor> ActorClass;
TArray<FHI_ItemStack> BuildCost;
float MaxSafeDepth;
FName RequiredPressureTier;
float PowerConsumption;
bool bRequiresFoundation;
bool bRequiresResourceNode;
FName CompatibleResourceType;
```

---

## 25. Class responsibilities

### `AHI_PlayerCharacter`

Handles:

- Movement
- Camera
- Interaction trace
- Scanner component
- Build manager component
- Inventory component

Should not handle:

- Recipe logic
- Power logic
- Machine production logic

### `UHI_InventoryComponent`

Handles item storage and transfer.

Must support:

- Add stack
- Remove stack
- Has items
- Consume items
- Transfer to another inventory
- Save/load data export

### `AHI_MachineBase`

Base for all machines.

Components:

- Inventory input
- Inventory output
- Power consumer
- Pressure component
- Interaction component

Methods:

- Start machine
- Stop machine
- Tick production
- Get status
- Repair damage

### `AHI_ExtractorMachine`

Specialized machine.

Responsibilities:

- Bind to resource node
- Produce raw resource over time
- Stop if node invalid, no power, output full or pressure unsafe

### `AHI_FabricatorMachine`

Specialized machine.

Responsibilities:

- Select recipe
- Process ingredients
- Produce output

### `AHI_PowerNetworkSubsystem`

World subsystem.

Responsibilities:

- Register producers
- Register consumers
- Register cables
- Build or refresh local networks
- Compute generation and consumption
- Broadcast power state changes

### `UHI_PressureComponent`

Reusable component.

Responsibilities:

- Read current depth from actor location or zone volume
- Compare with safe depth
- Apply stress state
- Notify owner when pressure unsafe

---

## 26. Save system MVP

Save only what matters.

### Save data

- Player location
- Player inventory
- Built machine instances
- Machine inventories
- Machine selected recipe
- Machine health
- Unlocked tiers
- Completed mission steps

### Do not save yet

- Creature state
- Temporary sonar pulses
- VFX
- Audio state
- Complex power network cache

Rebuild power network after load.

---

## 27. Milestones

### Milestone 0: Repository foundation

Goal: project can compile and open.

Tasks:

- Create Unreal C++ project
- Add base folder structure
- Add `.gitignore`
- Add Git LFS tracking for `.uasset`, `.umap`, `.wav`, `.fbx`, `.png`
- Add README
- Add this MVP plan
- Add coding conventions document

Acceptance criteria:

- Fresh clone opens in Unreal
- Project compiles
- No DerivedDataCache, Intermediate or Saved folders committed

### Milestone 1: Player and interaction

Goal: player can move, look, interact and scan.

Tasks:

- Implement first-person underwater movement
- Implement Enhanced Input mappings
- Implement interactable interface
- Implement interaction trace
- Implement scanner pulse
- Add temporary scan highlight material

Acceptance criteria:

- Player can move in all directions
- Player can interact with Deep Anchor
- Scanner highlights resource nodes within radius

### Milestone 2: Inventory and data

Goal: items and inventories work.

Tasks:

- Implement item stack struct
- Implement item definitions
- Implement inventory component
- Create initial item assets
- Add debug inventory UI
- Add console/debug function to grant items

Acceptance criteria:

- Items can be added and removed
- Inventory stacks correctly
- Deep Anchor has storage
- Player can transfer items to Deep Anchor

### Milestone 3: Build system

Goal: player can place buildables.

Tasks:

- Implement build manager
- Implement buildable definitions
- Implement ghost preview
- Implement placement validation
- Implement material cost consumption
- Add foundation and basic machine placement

Acceptance criteria:

- Player can place a foundation
- Player can place an extractor on a valid node
- Invalid placement shows reason
- Materials are consumed on build

### Milestone 4: Machines and production

Goal: first production chain works.

Tasks:

- Implement machine base
- Implement extractor
- Implement fabricator
- Implement recipe processor
- Create initial recipes
- Add machine UI panel

Acceptance criteria:

- Extractor produces Manganese Nodules
- Fabricator crafts Pressure Plates
- Machine pauses when output is full
- Machine pauses when no power

### Milestone 5: Power and logistics

Goal: machines need power and can move items.

Tasks:

- Implement power producer/consumer components
- Implement power cable actor
- Implement power network subsystem
- Implement cargo tube transfer
- Add basic power UI

Acceptance criteria:

- Machines only work when powered
- Deep Anchor powers nearby network
- Thermal Generator adds power
- Cargo Tube transfers items from extractor to fabricator/storage

### Milestone 6: Pressure and depth

Goal: depth matters.

Tasks:

- Implement pressure zones or depth calculation
- Implement pressure component
- Add pressure warnings
- Add Depth Module Mk1 unlock
- Gate Zone B or C behind pressure upgrade

Acceptance criteria:

- Machines warn when placed too deep
- Over-pressure machines degrade or shut down
- Depth Module Mk1 unlocks deeper operation
- Player can reach the MVP end zone only after upgrade

### Milestone 7: Environmental event and creature

Goal: first threat exists.

Tasks:

- Implement Thermal Surge event
- Implement repair interaction
- Implement Abyssal Stalker simple AI
- Add sonar distortion event
- Add audio and VFX placeholders

Acceptance criteria:

- Thermal Surge damages a cable or cargo tube
- Player can repair damaged object
- Stalker appears near light or Signal Drop
- Event creates tension without requiring combat

### Milestone 8: Narrative vertical slice

Goal: MVP has a beginning, middle and end.

Tasks:

- Implement mission subsystem
- Add objective UI
- Add Mission Control text/audio placeholders
- Add HADAL-3 wreck
- Add Hadalite Trace pickup
- Add end-demo trigger

Acceptance criteria:

- Player receives objectives in order
- Mission progresses based on actual gameplay actions
- Wreck discovery triggers story log
- Hadalite pickup ends MVP with final message

---

## 28. GitHub issues backlog

Use these as initial GitHub issues.

### Core

- Create Unreal C++ project and folder structure
- Add Git LFS and Unreal `.gitignore`
- Implement base player character
- Implement Enhanced Input actions
- Implement interaction interface
- Implement debug logging category

### Inventory

- Implement `FHI_ItemStack`
- Implement `UHI_ItemDefinition`
- Implement `UHI_InventoryComponent`
- Create item data assets
- Create debug inventory widget

### Build system

- Implement `UHI_BuildableDefinition`
- Implement build ghost preview
- Implement placement validation
- Implement build cost consumption
- Implement invalid placement feedback

### Machines

- Implement `AHI_MachineBase`
- Implement extractor
- Implement fabricator
- Implement machine UI
- Implement output blocked state

### Power

- Implement power components
- Implement cable actor
- Implement power network subsystem
- Implement network overload state
- Implement thermal generator

### Logistics

- Implement cargo tube actor
- Implement source/destination binding
- Implement timed item transfer
- Implement blocked transfer state

### Pressure

- Implement pressure component
- Implement depth zones
- Implement pressure damage
- Implement Depth Module Mk1 unlock

### Scanning

- Implement scanner pulse
- Implement resource highlight
- Implement sonar beacon
- Implement anomaly detection trigger

### Narrative

- Implement mission subsystem
- Implement objective UI
- Add intro mission
- Add Thermal Surge mission
- Add HADAL-3 wreck mission
- Add MVP ending

---

## 29. Claude Code instructions

### General rules for agents

When working on this repo:

1. Always read `MVP_PLAN.md` first.
2. Prefer small, compilable changes.
3. Do not implement future-scope features unless explicitly requested.
4. Keep gameplay systems modular.
5. Put core logic in C++.
6. Expose tuning values to Blueprint or Data Assets.
7. Do not hardcode item names inside machines.
8. Avoid circular dependencies between systems.
9. Add clear debug logs for new systems.
10. Update this document if scope changes.

### What Claude Code should do well

Claude Code should focus on:

- Creating C++ classes
- Creating headers and source files
- Defining structs and interfaces
- Adding Unreal components
- Creating subsystem logic
- Writing clean TODOs for Blueprint/editor setup
- Updating README and documentation

### What humans should do in Unreal Editor

Humans should handle:

- Creating maps
- Placing actors
- Assigning meshes
- Creating Blueprint subclasses
- Creating UI layouts
- Tuning materials
- Setting up VFX
- Checking gameplay feel

### Preferred implementation style

Use this pattern:

- C++ base class for behavior
- Blueprint child class for visuals
- Data Asset for configuration
- Component for reusable logic
- Subsystem for world-level coordination

Example:

- `AHI_MachineBase` in C++
- `BP_Extractor_Mk1` as Blueprint child
- `DA_Buildable_Extractor_Mk1` for cost and build metadata
- `UHI_PowerConsumerComponent` attached to machine
- `UHI_PowerNetworkSubsystem` manages network state

---

## 30. Coding conventions

### Prefixes

Use project prefix `HI`.

Examples:

- `AHI_MachineBase`
- `UHI_InventoryComponent`
- `FHI_ItemStack`
- `EHI_PressureTier`
- `IHI_InteractableInterface`

### IDs

Use lowercase snake case for gameplay IDs.

Examples:

- `res_manganese_nodule`
- `item_pressure_plate_mk1`
- `recipe_depth_module_mk1`
- `build_extractor_mk1`

### Logging

Create log category:

```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogHadalIndustries, Log, All);
```

Use useful logs, not spam.

Good:

```cpp
UE_LOG(LogHadalIndustries, Warning, TEXT("Extractor %s stopped: output inventory full"), *GetName());
```

Bad:

```cpp
UE_LOG(LogHadalIndustries, Warning, TEXT("Tick"));
```

### Blueprint exposure

Expose values that designers need to tune.

Do not expose internal state unless useful for UI or debugging.

---

## 31. Acceptance criteria for full MVP

The MVP is complete when:

- Player can start in Deep Anchor.
- Player can move underwater.
- Player can scan and discover resources.
- Player can place at least 5 buildable types.
- Player can extract Manganese Nodules.
- Player can process resources into Pressure Plates.
- Player can build a powered production chain.
- Player can use Cargo Tubes to move items.
- Player can build a Thermal Generator near a vent.
- Player can unlock Depth Module Mk1.
- Player can descend to the Signal Drop zone.
- Player can experience Thermal Surge.
- Player can repair damaged infrastructure.
- Player can encounter the Abyssal Stalker.
- Player can find HADAL-3 wreckage.
- Player can collect Hadalite Trace.
- Player can trigger the MVP ending.
- Game can save and load basic player/base state.
- Project can be cloned, compiled and opened without missing source files.

---

## 32. First 10 development tasks

This is the best order to start:

1. Create Unreal C++ project.
2. Add Git LFS and `.gitignore`.
3. Implement player movement and input.
4. Implement interaction interface.
5. Implement inventory component.
6. Implement item and recipe definitions.
7. Implement resource node actor.
8. Implement scanner component.
9. Implement machine base and extractor.
10. Implement build placement for extractor and foundation.

Do not start with creatures, story, VFX or fancy water shader.

Those are dessert. We need the damn pasta first.

---

## 33. Suggested README summary

```md
# Hadal Industries

Hadal Industries is a first-person 3D automation and exploration game set in the depths of the Mariana Trench.

The player is deployed by Pelagos Dynamics to build a deep-sea automated extraction network, manage pressure and power, explore hostile abyssal zones, and uncover what happened to previous failed expeditions.

This repository contains the Unreal Engine MVP vertical slice focused on:

- Underwater first-person exploration
- Resource scanning
- Modular base construction
- Resource extraction and processing
- Simple power networks
- Cargo tube logistics
- Pressure-based depth progression
- A small narrative end-demo sequence
```

---

## 34. Reference links

These references are useful for design and implementation context.

- Unreal Engine 5.7 release notes: https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-7-release-notes
- Unreal Gameplay Framework: https://dev.epicgames.com/documentation/unreal-engine/gameplay-framework-in-unreal-engine
- Unreal Components: https://dev.epicgames.com/documentation/unreal-engine/components-in-unreal-engine
- Unreal Enhanced Input: https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine
- Unreal World Partition: https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine
- Unreal Data Driven Gameplay Elements: https://dev.epicgames.com/documentation/unreal-engine/data-driven-gameplay-elements-in-unreal-engine
- Unreal SaveGame: https://dev.epicgames.com/documentation/unreal-engine/saving-and-loading-your-game-in-unreal-engine
- NOAA Mariana Trench overview: https://www.ncei.noaa.gov/news/planet-postcard-mariana-trench
- NOAA hydrothermal vents: https://oceanservice.noaa.gov/facts/vents.html
- USGS deep-ocean mineral resources: https://pubs.usgs.gov/publication/fs20253017/full
