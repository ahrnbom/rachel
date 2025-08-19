# RACHEL: Robotics, Automatic Control, Hardware Emulation Library

## Project status
- [x] written design principles
- [x] working concept of subscription and publishing of topics across nodes
- [x] logging support
- [ ] kill unresponsive nodes at shutdown after timeout
- [x] launch parameters
- [ ] transform tree implementation
- [ ] serialization support for some commonly used types, to/from file and network
- [ ] some basic form of ROS integration
- [ ] basic visualization support
- [ ] first alpha release: basic functionality implemented, some things work, needs a lot more polish
- [ ] first beta release: no obvious known bugs, suitable for testing, main functionality documented sufficiently for common use 
- [ ] first general release

## Documentation
* [Design principles](https://github.com/ahrnbom/rachel/blob/main/docs/design.md)
* [Launch parameters](https://github.com/ahrnbom/rachel/blob/main/docs/parameters.md)

### Getting started
RACHEL has the following dependencies: 
* [CMake](https://cmake.org/)
* [spdlog](https://github.com/gabime/spdlog)
* [nlohmann json](https://github.com/nlohmann/json)
* [Eigen](https://libeigen.gitlab.io/docs/)
* (Optional) [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

On Fedora, you can install these with `sudo dnf install cmake spdlog-devel json-devel eigen3-devel`. 
If you want to use pre-configured code formatting for C++, also run `sudo dnf install clang-tools-extra`.

After installing dependencies, you want to configure `CMakeLists.txt` to compile and link the nodes you want. 
Then, you want to modify `main.cpp` to launch the nodes you want. 
Finally, you can run `./run.sh -r` to compile and run the program, optionally adding the flag `-f` to do automatic code formatting with `clang-format`.