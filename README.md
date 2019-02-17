# VR 3D Modeling (Unreal Project)

HOW TO SETUP:
1. Find the .Uproject exe and right click to generate the visual studio project
2.Once Done, Double Click to start project

NOTE: Some items are missing from this version, videos for tutorial and meshes that are imported for the
sphere and torus, other meshes are generated procedurally

ABOUT THE PROJECT:
	UI Widgets:
	Widgets for the UI can be found in the UI folder found in the content folder

	VR Controls
	VR Blueprints are found in the VR Integrator folder that can be found on the Unreal Marketplace (Has been Modified to work with this project)
	https://www.unrealengine.com/marketplace/vr-integrator-radial-and-dockable-menus

	Main Source folder found at root directory includes a static file function library (Exposed in Blueprints) created to show all folders and files in a directory provided

	Two Plugins are included:
		Interactions Plugin: File Location - VRModeling_ThesisProject\Plugins\Interactions
			Includes all classes and interfaces for interactions with the world

		ASSIMP Plugin: File Location - VRModeling_ThesisProject\Plugins\Assimp
			AssimpMeshComponent includes the Assimp library to allow exporting and importing of meshes
			EdtableMeshComponent (Subclasses Assimp Component) includes all functionst that modify a mesh created by this component
			Other Components that allow interaction with the mesh points

			Meshes included that can be created (Exposed as Blueprints from the Editablemeshcomponent) but are not listed on the create menu
			Rectangle
			Cube
			Disk
			Cylinder
			Pipe
			Tetrahedron
			Octohredren
			Others, that may not be mentioned

Full Project Link w/o setup needed
https://drive.google.com/open?id=1QO6odH2Jc3DIDJ7LoJiS89VsgYoXgyun

For any questions email:
dariusbigbee12@gmail.com 

More detailed explanantions to come in the future