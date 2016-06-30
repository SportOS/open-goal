# Open Goal

Open Goal is a project created by avid FIFA and Football Manager players aiming to blend core gameplay concepts from both into a single game. This repository contains a basic Unreal Engine 4 implementation of some of our ideas.

Our mission is more focused on making well-crafted game mechanics that aren't as "gamified" as, say, FIFA, but without making it a literal simulation of real-life soccer. We consider well-crafted mechanics to be more important than graphics, animations, and timely releases. (Said every unfunded open source project, ever.)

### Quick Download
We don't have a quick download available because the game isn't in a playable state yet.

### The Team
We are two developers, Andriy Katkov and Dennis Lysenko, working on this project alongside school/other projects in our spare time. Both of us have been working on a startup for two years, where we got the original impetus to create this project.

### Contributing
If you have any interest in contributing the quickest way to get in touch is to just open an issue and we'll both get emails about it. Forking independently is not recommended because of how unfortunately annoying merges can be with UE4 asset and blueprint files.

### Installation
At the moment this isn't even close to stable or playable, but you can install it and try it out by downloading Unreal Engine 4 at https://www.unrealengine.com. 

Mac:
After you install Unreal Engine and clone the project, open the .uproject file in Unreal Engine. After the editor opens, you can do File > Build Xcode Project to create the Xcode project. After that, I close the editor and then open the newly generated .xcworkspace file in Xcode. Then, when I build with the target "OSF", the editor will open and I can build, compile changes made in Xcode, and run the application. 

Windows:
After you install Unreal Engine and clone the project, open the project directory in explorer. Right click the .uproject file and click "Generate Visual Studio project files". You can then open the .sln file in Visual Studio. You can build the "DebugGame" solution and run it to launch the Unreal Editor. Now, you can build, compile changes made in Visual Studio, and run the application.

Note: The Windows instructions were tested with Visual Studio 2015 Update 3 which caused a compilation issue as described here: https://answers.unrealengine.com/questions/442881/blank-c-project-wont-build-4123.html. The suggested answer was able to fix the issue while the Unreal team works on making Update 3 compatible.

We're not really sure what to do on Linux, so probably just follow the instructions under *Contributing* above and get in touch with us so we can help you out with the rest.
