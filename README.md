# LuaMachine
![LuaMachine Logo](Docs/LuaMachine_Logo.png?raw=true "LuaMachine Logo")

Unreal Engine 4 Plugin for adding Lua scripting to your projects.

If you want modders to customize your game/project, or you need to allow game designers to script parts of the logic, this plugin is for you.

![LuaMachine](Docs/Screenshots/SyntaxColor.PNG?raw=true "LuaMachine")

## How it works

Contrary to the other Unreal Engine 4 Lua plugins, this one does not try to expose the Unreal Engine 4 api, but completely hides it exposing to the user/scripter only the features the developer decided to include (via Blueprints or C++).

Currently Windows 64bit, Mac, Linux 64bit (both Runtime and Editor), Android and iOS are supported.

## Discord Channel

Join the LuaMachine Discord channel: https://discord.gg/eTXDfEU

## Marketplace

Albeit fully open source and free, you can find the plugin in the Unreal Engine Marketplace for about 30$. If you want to sponsor the project consider buying it.

## Quickstart

In this fast (and useless example) we will print a string generated by a Lua script.

Install the LuaMachine plugin by downloading it (for free) from the official releases page (choose the binary release for your Unreal Engine version and operating system) or from the Marketplace (about 30$): 

https://github.com/rdeioris/LuaMachine/releases/

https://www.unrealengine.com/marketplace/luamachine

Uncompress it in the Plugins/ directory of your project (create it if it does not exist) and restart the Unreal Editor

Open your favourite editor for lua scripting and create a file into the project Content/ directory (call it hello.lua) with the following code:

```lua
current_time = os.date()
return "Hello, i am a lua script, current time is: " .. current_time
```

Now you need to create a new LuaState, it is a special Blueprint representing a Lua Virtual Machine (you can have multple Lua virtual machines in the same project). Just add a new blueprint inheriting from LuaState and name as you want (QuickLuaState is a good candidate).

Finally open your Level Blueprint and add the following nodes:

![Quickstart](Docs/Screenshots/Quickstart.PNG?raw=true "Quickstart")

If you play the game, you will see the Lua string printed on top.

The LuaRunFile node will execute a lua file (relative to the Content/ directory) in the specified LuaState (QuickLuaState in our case) and will return the value returned by the script itself as a LuaValue UStruct. The plugin offers a bunch of operations you can apply to LuaValue's, in this example we simply converted the value to an Unreal String.

Time to something more complex, edit your hello.lua script:

```lua

function call_me_from_unreal(number)
  return number * 2
end

current_time = os.date()
return "Hello, i am a lua script, current time is: " .. current_time
```

Now the script will add the call_me_from_unreal function to the global table:

![Quickstart2](Docs/Screenshots/Quickstart2.PNG?raw=true "Quickstart2")

The LuaGlobalCall node calls a function from the global table passing arguments as an array of LuaValue's.

So, now we know how to call lua functions from Unreal, time to do the opposite.

Open the QuickLuaState Blueprint you created before. This Blueprint exposes lot of different fields to configure your Lua Virtual Machine:

![Quickstart3](Docs/Screenshots/Quickstart3.PNG?raw=true "Quickstart3")

The Table 'TMap' allows you to add items to the Lua global table. We will first try by adding a string and a number (float):

![Quickstart4](Docs/Screenshots/Quickstart4.PNG?raw=true "Quickstart4")

Note that we set the "Lua Filename" field to hello.lua, in this way we can avoid to call the LuaRunFile node as hello.lua will be executed during the spawn of the QuickLuaState.

Let's modify our Level Blueprint:

![Quickstart5](Docs/Screenshots/Quickstart5.PNG?raw=true "Quickstart5")

The hello.lua script is loaded automatically, while the LuaGetGlobal nodes, retrieve the specific fields from the Lua global table, and return them as LuaValue's

Time to expose an Unreal UFunction to Lua: just add a Function to QuickLuaState:

![Quickstart6](Docs/Screenshots/Quickstart6.PNG?raw=true "Quickstart6")

and expose it in the Table TMap:

![Quickstart7](Docs/Screenshots/Quickstart7.PNG?raw=true "Quickstart7")

calling get_player_location() from lua, will trigger the QuickLuaState's GetPlayerPawnLocation(). Note that not all of the UFunction's can be exposed: if they take or return values they must be LuaValue's

Let's update hello.lua

```lua
function call_me_from_unreal(number)
    location = get_player_location()
    print(location.x, location.y, location.z)
    return number * 2 + location.x + location.y + location.z
end

current_time = os.date()
return "Hello, i am a lua script, current time is: " .. current_time
```

and the Level Blueprint:

![Quickstart8](Docs/Screenshots/Quickstart8.PNG?raw=true "Quickstart8")

Please check the print() function: it is automatically mapped to the Unreal Output Log by the plugin.

Now you should have an idea of the plugin potential, continue reading for more infos.

## LuaState

A LuaState (ULuaState C++ class) represents a single Lua virtual machine (there can be multiple, each one isolated from the others). This is a "singleton" as each LuaState class can have a single instance for the whole process life. (Note that while in the Editor, LuaState's are constantly recreated whenever you enter PIE mode to simplify development)

Having multiple LuaState's allows clean separation of domains: as an example you may want a state for game configuration, another one for game logic and one for the modders. More complex examples include having states dedicated to specific Actors, GameModes or Levels.

LuaState's are loaded on-demand, so you can create dozens of them but they will not start Lua VMs until you need them.

LuaState's can load and execute scripts automatically if you specify the fields "LuaCodeAsset" and/or "LuaFilename". You can load scripts (both in text and bytecode format) from your filesystem (via "LuaFilename"), or from a specific asset exposed by the plugin, named LuaCode (you can specify it with the "LuaCodeAsset" field). The advantage of using filesystem scripts is that you can change them even after the project has been packaged (unless you package them too), while LuaCode assets are directly built in your pak files (as bytecode by default).

### LuaState Properties

* LuaCodeAsset: specify a LuaCode asset to automatically load an execute on spawn
* LuaFileName: specify a script path (relative to Content/) to load and execute on spawn
* Table: TMap<FString, FLuaValue> allows adding FLuaValue's to the Lua VM global table
* RequireTable: TMap<FString, ULuaCode> allows to map LuaCode assets to specific name, so you can call require('name') from your code
* LuaOpenLibs: if true, automatically load the lua standard library on spawn
* AddProjectContentDirToPackage: if true, when doing require('name') will search for 'name.lua' in the Content/ directory
* AppendProjectContentDirToPackage: TArray<FString> allows specifying a list of Content/ subdirectories to search for packages (while doing require('name'))
* OverridePackagePath: (advanced users) allows to modify package.path
* OverridePackageCPath: (advanced users) allows to modify package.cpath
* LogError: enable/disable logging of Lua errors
  
### LuaState Events

A single event is exposed by ULuaState: "LuaError"

If defined, it will be triggered whenever the Lua VM generates an error. The Error message is passed as an argument. This is really useful for adding in-game consoles, or to catch specific errors.

### LuaState in C++

You can define your LuaState's as C++ classes, this is handy for exposing functions that would be hard to define with blueprints:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "LuaState.h"
#include "AdvancedLuaState.generated.h"

/**
 * 
 */
UCLASS()
class LUATEST420_API UAdvancedLuaState : public ULuaState
{
	GENERATED_BODY()
public:
	UAdvancedLuaState();

	UFUNCTION()
	FLuaValue GetPlayerPawnLocation();

	UFUNCTION()
	FLuaValue GetSimpleString();
	
};
```

```cpp
#include "AdvancedLuaState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UAdvancedLuaState::UAdvancedLuaState()
{
	Table.Add("simple_string", FLuaValue("Test string"));
	Table.Add("get_player_location", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UAdvancedLuaState, GetPlayerPawnLocation)));
	Table.Add("get_simple_string", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UAdvancedLuaState, GetSimpleString)));
}

FLuaValue UAdvancedLuaState::GetPlayerPawnLocation()
{
	FVector Location = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	FLuaValue Table = CreateLuaTable();
	Table.SetField("x", FLuaValue(Location.X));
	Table.SetField("y", FLuaValue(Location.X));
	Table.SetField("z", FLuaValue(Location.X));

	return Table;
}

FLuaValue UAdvancedLuaState::GetSimpleString()
{
	return FLuaValue("Hello World");
}

```

GET_FUNCTION_NAME_CHECKED is an Unreal Macro for sanitizing UFunction names. Albeit not necessary (the argument is the FName of the function) it is highly suggested to use it to avoid silent errors.

You can call LuaState's offered api using the ULuaBlueprintFunctionLibrary class.

Check its docs here: [LuaBlueprintFunctionLibrary](Docs/LuaBlueprintFunctionLibrary.md)

## LuaValue

LuaValue's are the way Unreal communicates with a specific Lua virtual machine. They contains values that both Lua and your project can use.

This a list of the currently supported values:

* Nil (nil/NULL value)
* Bool (boolean value)
* Integer (integer/int32 value)
* Number (float value)
* String (string/FString value)
* Function (reference to a lua function)
* Table (reference to a lua table)
* UObject (pointer to an Unreal UObject)
* UFunction (pointer to an Unreal UFunction)
* Thread (reference to a lua thread/coroutines)

Note that tables are passed by reference, so technically you can update the same table from both lua and Unreal.

Check [LuaBlueprintFunctionLibrary](Docs/LuaBlueprintFunctionLibrary.md) for infos on how to use the FLuaValue api.

## Shortcut for specifying field names

As a shortcut for reaching long paths in tables, you can use the dot as the separator for field names.

As an example the LuaGetGlobal node, can access the package.path item by simply passing it "package.path" as the field name.

## LuaComponent

ULuaComponent allows to map a table to an ActorComponent

Check its docs here: [LuaComponent](Docs/LuaComponent.md)

## LuaCodeBox/ULuaMultiLineEditableTextBox

This is a UMG widget for showing/editing lua code with syntax colouring:

![LuaCodeBox](Docs/Screenshots/LuaCodeBox.PNG?raw=true "LuaCodeBox")

The syntax highlighter is available in the FLuaMachineSyntaxHighlighterTextLayoutMarshaller class (so you can reuse it
for building more advanced Slate/UMG widgets)

## Lua Threads/Coroutines

Lua Threads/Coroutines are fully supported

Check dedicated docs here: [LuaCoroutines](Docs/LuaCoroutines.md)

## Packaging

You have various way to use Lua scripting facilities in your packaged project:

### LuaCode for everything

Transform your scripts (just drag them in the content browser) to LuaCode assets and eventually fix the RequireTable. By default they will be byte-compiled. It is highly suggested to "sign" the pak file to avoid pak file modifications. This method does not permit customization of scripts after the build.

### Scripts file packaging

You can include your scripts in the pak file automatically by specifying the directory containing them in the package settings:

![Packaging](Docs/Screenshots/Packaging.PNG?raw=true "Packaging")

Signing the pak file could be a good thing again, in addition to this byte-compiling the scripts could be accomplished with the 'luac' command (included in lua distributions)

### Scripts file inclusion

This is for allowing easy customization of your scripts after the packaging. Basically you include the scripts directories in your Content directory:

![Packaging2](Docs/Screenshots/Packaging2.PNG?raw=true "Packaging2")

## Android deployment

The plugin supports Android systems starting from api 24. Before deployng ensure you have force the right minimal api version:

![Android24](Docs/Screenshots/Android24.PNG?raw=true "Android24")

Cooked LuaCode assets are converted to 32bit size_t at runtime.

## iOS deployment

Nothing special, just ensure to package script files (instead of simply including them)

## Functional Tests

The projects includes various functional tests (all written as blueprints, so they could be useful as examples too).

Just enable the plugin content visualization from the content browser and from the Automator tool execute the Project related tests.

## Tutorials

https://github.com/rdeioris/LuaMachine/blob/master/Tutorials/SimpleDialogueSystem.md

## Commercial Support

Commercial support is offered by Unbit (based in Rome, Italy), just contact info at unbit dot it or follow/direct message @unbit on twitter

## Sponsors

AIV, Accademia Italiana Videogiochi (http://www.aiv01.it). They are using it for Lua lessons in the Game Design classes.

The LuaMachine logo has been designed by Nicolas Martel (aka. Nogitsu)



