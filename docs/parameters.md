# Launch parameters in RACHEL
The nodes' behavior can be controlled through launch parameters. These are decided during startup of the RACHEL program, and are afterwards read-only
parameters to the nodes' `run` functions. Do not use launch parameters to pass any kind of data to nodes that needs to be updated throughout the 
execution of the `run` function. For such purposes, you can use topics, or by using other thread-safe data (e.g. using `std::atomic`).

RACHEL parameters support all data types supported by JSON, like strings, booleans, floating point numbers and integers. In fact, RACHEL 
uses JSON objects as a convenient storage for parameters, and a JSON file is used to load them.

Each node can define its default parameters by overriding the `virtual void set_default_params(nlohmann::json& params);` function. In this function, 
the node has access to a JSON object which can be written to, such as `params["/some_node/some_param"] = some_value`. The convention is to use
parameter names formatted as `/<node name>/<param name>` for any parameters that are specific to some node, and you may use the `param_name`
function on your node object to automatically convert e.g. `"~/some_param"` to `"/some_node/some_param"`, if the node is named `"some_node"`.

Then, one may optionally create a parameter JSON file which allows parameters to be overwritten when the program is about to be run.
The default location of this JSON file is `/etc/rachel/params.json`, but any location can be specified with the environment variable
`RACHEL_PARAMS_FILE`. Note that if `RACHEL_PARAMS_FILE` is set, and points to a file that doesn't exist, RACHEL will throw an exception. 
This is to avoid silently ignore e.g. a typo in the file path. If the environment variable isn't set, though, the default file does not have to
exist; in that case, the default values will be used instead.

## Example
A `.cpp` file for a node called `some_node` could look something like this:

```cpp
#include "some_node.hpp"

SomeNode some_node("some_node"); // this string value here determines the node name, used by the param_name function

const std::string SOME_NUMBER_PARAM = some_node.param_name("~/some_number"); // evaluates to "/some_node/some_number"

void SomeNode::set_default_params(nlohmann::json& params) { // the input params object is empty here
    params[SOME_NUMBER_PARAM] = 0; // sets the default to 0, could be overwritten by user's parameter json file
}

void SomeNode::run(const nlohmann::json& params) // note that the reference must be const here, parameters can not be written during run
{
    int x = params[SOME_NUMBER_PARAM]; // loads the parameter, here as a copy

    // if you don't intend to modify it, you can use it as a constant reference
    const int& y = params[SOME_NUMBER_PARAM];

    while (main_loop_condition()) {
        // ... do something
    }
}
```

## Comparison with ROS
In ROS, there is a parameter server, keeping track of all parameters. This includes the ability to "subscribe" to parameters, using a caching system.
The implementation is quite complicated, considering it's typically used for very basic values. Since RACHEL runs in a single process, the parameter
system can be greatly simplified. By enforcing that parameters are read-only as soon as the nodes start, thread safety is guaranteed and this simplifies
the implementation even further.