# YDependencyAnalyser
UE4 plugin to measure and analyse bottlenecks in asset dependency sizes.

**Content:**
* Tool Usage
* Understanding the Tool
* Automated Testing Integration

# Tool Usage
The plugin can be enabled similarly to any other YAGER plugins, and it forms part of the YAGER.Plugins category.

![image](https://user-images.githubusercontent.com/17513294/209984867-753fa124-ec33-42b9-8b70-321126f70f48.png)

Once the plugin is enabled, the tool tab can be brought up from the main menu, via **Window > Developer Tools > Dependency Analyser**.

![image](https://user-images.githubusercontent.com/17513294/209984908-75406185-e3d3-4f15-8ff5-a68fd6128fe2.png)

# Understanding the Tool
The Dependency Analyser tab looks as follows. It allows to:
* **Mark whether to include soft references in the dependency analysis**
* **Mark whether to ignore developer folders in the asset list and in their dependency count**
* **Customise at what size (in MB) assets should be considered a warning (displayed in Orange)**
* **Customise at what size (in MB) assets should be considered an error (displayed in Red)**
* **Export the results of a dependency analysis into a CSV (pressing the button will also bring up the directory where the CSV file is saved, and a log will be printed with the relevant path)**
* **Search among the results (by Name or Type)**
* **Sort the results by Name, Dependencies Count, Total Size, Type or Path)**
* **Inspect a textual overview of the results**

Additionally, each result row can be individually double-clicked to open the Size Map analysis of the asset (**integration with UE Size Map tool**).

![image](https://user-images.githubusercontent.com/17513294/209985003-82436606-8195-4a22-ac91-1f2ccd2187ba.png)

# Automated Testing Integration
If the project has the Functional Testing Editor plugin enabled, the YDependencyAnalyser also offers a simple automated test that runs a dependency analysis on all assets.
The test fails if any asset dependency is larger than the max asset dependency size (500 MB), and it throws a warning if larger than the recommended asset dependency size (50 MB).

![image](https://user-images.githubusercontent.com/17513294/209985029-fa243915-110a-4a2d-99d6-d5c03398bdc2.png)
