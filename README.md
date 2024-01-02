# Dependency Analyser
The Dependency Analyser is a UE5 plugin to measure and analyse bottlenecks in [asset dependency](https://docs.unrealengine.com/5.3/en-US/referencing-assets-in-unreal-engine/) sizes. It allows for quick inspection of all assets in a project and identification of optimisation opportunities, thanks to its integration with the size map tool and the automated testing framework.

## Table of Contents
- [Introduction to the Dependency Analyser](#introduction-to-the-dependency-analyser)
  - [Usage instructions](#usage-instructions)
- [Dependency Analyser Tool](#dependency-analyser-tool)
- [Dependency Analyser Test Suite](#dependency-analyser-test-suite)

## Introduction to the Dependency Analyser
The Dependency Analyser plugin comprises of two main features:
* The [Dependency Analyser Tool](#dependency-analyser-tool), which allows for an overview of the project asset dependencies
* The [Dependency Analyser Test Suite](#dependency-analyser-test-suite), which uses the Automation System framework to include acceptance limits to asset dependency sizes

### Usage instructions
To use the Dependency Analyser plugin:
* Download this repository
* Move it to
  * the Plugins folder of your project, if you want to import it into a specific project
    * i.e. `LyraStarterGame\Plugins`
  * the Plugins folder of your engine, if you want to import it into any project
    * `C:\Program Files\Epic Games\UE_version\Engine\Plugins` on Windows
    * `/Users/Shared/Epic Games/UE_version/Engine/Plugins` on macOS
* Enable it by following the [Unreal Engine guidelines](https://docs.unrealengine.com/5.0/en-US/working-with-plugins-in-unreal-engine/)
* Navigate to `Tools > Miscellaneous > Dependency Analyser` to bring up the Dependency Analyser Tool.
* Navigate to `Tools > Test Automation` and select `DependencyAnalyser` to inspect and run the Dependency Analyser Test Suite.

## Dependency Analyser Tool
The Dependency Analyser Tool comprises of the following features:
1. Optional inclusion of soft references and dev folders into the results
2. Customisation of size in MB at which assets should be considered a warning (orange) or an error (red)
3. Export of results of the analysis into a CSV file. Pressing the export button brings up the directory where the CSV file is saved, and a log is printed in console with the relevant path
4. Individual inspection of resulting assets
   * Right-clicking brings up a context menu including the options `Open Size Map`, `Edit Asset` or `Find in Content Browser`
   * Double-clicking bring up the Size Map inspector
5. Searchable results by Name or Type
6. Sortable results by Name, Dependencies Count, Total Size, Type or Path

TODO add image breakdown

## Dependency Analyser Test Suite
If the project has the Functional Testing Editor plugin enabled, the Dependency Analyser offers a test suite to run a dependency analysis on all assets. Users can customise error and warning sizes, and whether warnings should fail a test or not. The test is executed per-asset, so individual sizes of asset for each failing test can be identified. Test runs can be customised by navigating to `Edit > Editor Preferences > Advanced > Dependency Analyser` and modifying `Warning Size in MB`, `Error Size in MB` and `Fail for Warnings`.

TODO add image
