//
// Created by Simon Maurer on 19.04.18.
//

#include <iostream>

#include <console-common/console-plugin-base.h>

// Your new plugin needs to derive from ConsolePluginBase.
// (Alternatively, you can derive from ConsolePluginBaseWithPlotter if you need
// RViz plotting abilities for your VI map.)
class PatchExtractionPlugin : public common::ConsolePluginBase {
public:
    // Every plugin needs to implement a getPluginId function which returns a
    // string that gives each plugin a unique name.
    std::string getPluginId() const override {
        return "patch_extraction_plugin";
    }

    // The constructor takes a pointer to the Console object which we can forward
    // to the constructor of ConsolePluginBase.
    PatchExtractionPlugin(common::Console *console)
            : common::ConsolePluginBase(console) {
        // You can add your commands in here.
        addCommand(
                {"extract_patches"},  // Map "hello_world" and "hello" to this
                // command.
                [this]() -> int {  // Function to call when this command is entered.
                    // This function can do anything you want. Check the other plugins
                    // under ~/maplab_ws/src/maplab/console-plugins for more examples.

                    // Here, we just print a message to the terminal.
                    std::cout << "Patch extraction in progres.." << std::endl;

                    // Every console command returns an integer, you can take one from
                    // the CommandStatus enum. kSuccess returns everything is fine.
                    // Other commonly used return values are common::kUnknownError and
                    // common::kStupidUserError.
                    return common::kSuccess;
                },

                // This is the description of your command. This will get printed when
                // you run `help` in the console.
                "This command extracts images or patches of corresponding keypoint pairs/triplets reflecting same 3d points of a loaded map. Parameters are: --mode/-M {images, pairs, triplets}, --output_dir/-O, --patch_size/-P",

                // This specifies the execution method of your command. For most
                // commands, it is sufficient to run them in sync with
                // common::Processing::Sync.
                common::Processing::Sync);
    }
};

// Finally, call the MAPLAB_CREATE_CONSOLE_PLUGIN macro to create your console
// plugin.
MAPLAB_CREATE_CONSOLE_PLUGIN(PatchExtractionPlugin);