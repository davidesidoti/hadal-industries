# HadalIndustries – CLAUDE.md

## Project
Unreal Engine 5.7 game project. Three game modes: Horror, Shooter, Hadal (deep sea).

## Build
```bash
"D:/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" HadalIndustriesEditor Win64 Development -Project="D:/Github/hadal-industries/HadalIndustries.uproject" -WaitMutex
```
Build logs: `C:/Users/sidot/AppData/Local/UnrealBuildTool/Log.txt`

## Workflow
- Before implementing any new feature/system, search https://www.fab.com/ for existing free or <€10 assets/plugins first.

## UnrealMCP Plugin
Plugin at `Plugins/UnrealMCP/` (from chongdashu/unreal-mcp). Python server registered globally as MCP `unrealMCP` — already connected, no per-project config needed.

**UE 5.7 patches applied to upstream source (must re-apply if plugin is updated):**
- `ANY_PACKAGE` removed in UE 5.1 — replaced all `FindObject<UClass>(ANY_PACKAGE, *Name)` with `FindFirstObject<UClass>(*Name, EFindFirstObjectOptions::EnsureIfAmbiguous)` in `UnrealMCPBlueprintCommands.cpp` and `UnrealMCPBlueprintNodeCommands.cpp`
- `const int32 BufferSize` in `MCPServerRunnable.cpp` collides with UE template internals — renamed to `MCPBufferSize`
