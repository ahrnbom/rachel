# RACHEL: Robotics, Automatic Control, Hardware Emulation Library

## Project status
- [x] written design principles
- [x] working concept of subscription and publishing of topics across nodes
- [x] logging support
- [ ] kill unresponsive nodes at shutdown after timeout
- [ ] launch parameters
- [ ] transform tree implementation
- [ ] serialization support for some commonly used types, to/from file and network
- [ ] some basic form of ROS integration
- [ ] basic visualization support
- [ ] first alpha release: basic functionality implemented, some things work, needs a lot more polish
- [ ] first beta release: no obvious known bugs, suitable for testing, main functionality documented sufficiently for common use 
- [ ] first general release

## Documentation
* [Design principles](https://github.com/ahrnbom/rachel/blob/main/docs/design.md)

### Getting started
RACHEL has the following dependencies: 
* [CMake](https://cmake.org/)
* [spdlog](https://github.com/gabime/spdlog)

On Fedora, you can install these with `sudo dnf install cmake spdlog-devel`. 