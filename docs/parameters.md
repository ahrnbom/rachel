# Launch parameters in RACHEL
The nodes' behavior can be controlled through launch parameters. These are decided during startup of the RACHEL program, and are afterwards read-only
to the nodes' `run` functions. For any data which will need to be updated, use another mechanism like subscriptions. 

RACHEL parameters support all data types supported by JSON, like strings, booleans, floating point numbers and integers. In fact, RACHEL 
uses JSON objects as a convenient storage for parameters, and a JSON file is used to load them.

Each node can define its default parameters by overriding the `virtual void set_default_params(nlohmann::json& params);` function. In this function, 
the node has access to a JSON object which can be written to, such as `params["/some_node/some_param"] = some_value`. Then, 
one may optionally create a parameter JSON file which allows parameters to be overwritten when the program is about to be run.
The default location of this JSON file is `/etc/rachel/params.json`, but any location can be specified with the environment variable
`RACHEL_PARAMS_FILE`. Note that if `RACHEL_PARAMS_FILE` is set, and points to a file that doesn't exist, RACHEL will throw an exception. 
This is to avoid silently ignore e.g. a typo in the file path. If the environment variable isn't set, though, the default file does not have to
exist; in that case, the default values will be used instead.

The JSON file is treated as a JSON patch, as per [this specification](https://datatracker.ietf.org/doc/html/rfc7386).


## Comparison with ROS
In ROS, there is a parameter server, keeping track of all parameters. This includes the ability to "subscribe" to parameters, using a caching system.
The implementation is quite complicated, considering it's typically used for very basic values. Since RACHEL runs in a single process, the parameter
system can be greatly simplified. By enforcing that parameters are read-only as soon as the nodes start, thread safety is guaranteed and this simplifies
the implementation even further.