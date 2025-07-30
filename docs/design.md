# Design principles of RACHEL

1. Don't reinvent the wheel, only introduce RACHEL specific types and interfaces when they add value
    - Examples: RACHEL is compiled via standard CMake (ROS uses custom tools built on top of CMake), a transform is represented by an `Eigen::Isometry3d` (ROS uses its custom `geometry_msgs::TransformStamped` object, which is commonly converted to/from Eigen anyway), logging is handled directly by `spdlog` (ROS has custom macros that call `spdlog` under the hood).
2. Don't be invasive
    - Example: A RACHEL application is just an application and RACHEL is a library used to build it, nothing more (ROS is an "operating system", by which they mean it's a library so invasive it basically takes over your entire OS if you install it). RACHEL is flexible enough that it could live inside of a ROS node. 
3. Dare to make assumptions if they drastically simplify things
    - Example: RACHEL does not by default support any languages other than C++, and it does not by default support publishing/subscribing topics over then network. By omitting these features, RACHEL has lower complexity and is often drastically faster than ROS. 

## What RACHEL is 
RACHEL is a C++ library used to make robotics applications. Unlike ROS, the end result of using RACHEL is an application, a single binary which does the thing you want it to do. 

A developer making a RACHEL application writes **nodes**. Each node is a subclass of `rachel::Node`, and defines a `run` function which runs in a separate thread from other nodes. This is different from ROS, where each node runs in a separate process. Nodes communicate with other nodes primarily by publishing and subscribing to **topics**, which can be of _any_ type, and RACHEL makes sure that once one node has published a value to a topic, any other nodes subscribing to that node will get a copy of that value without significant delay. Unlike ROS, there is no serialization step involved during normal topic updates, and RACHEL simply copies the data from one thread to another while making sure the threads never mess up each others' data. 

This does put certain responsibilities on the developer. It is fairly easy to build a multithreaded application where one thread is trying to read a piece of memory while another thread is writing to it, leading to bugs and crashes. When using RACHEL's provided tools for communication between nodes, this can never happen, but there are no guard rails preventing the developer from manually pulling in one node's memory into another node. By assuming that the developer will not do this, the design of RACHEL is simplified a great deal, and a lot of overhead can be avoided, in accordance with design principle 3. 

While ROS seems to want to introduce as many layers of abstraction as possible, RACHEL takes a different approach. We prefer to use well established existing libraries, and when we do need to standardize something for RACHEL, we try to do this in a way that doesn't hide the underlying implementation, and provides full access to what runs under the hood. For example, a timestamps and time deltas are represented in RACHEL using `std::chrono`, as 
```C++
using Time = std::chrono::time_point<std::chrono::steady_clock>;
using TimeDelta = Time::duration;
```
This has several benefits. First, it means that the implementation of RACHEL can stay relatively lightweight and simple, focusing on the structures that actually matters for robotics implementations. Second, it makes skills learned when working with RACHEL more transferrable to and from other C++ tools. 