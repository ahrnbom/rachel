#pragma once

#include <functional>
#include <fstream>
#include <cstdlib>

#include <nlohmann/json.hpp>

namespace rachel
{
    using param_t = const nlohmann::json;

    /*
        Parameters in RACHEL are read-only as soon as the `run` functions of the nodes start. This allows thread safe access whenever possible.
        To enforce this, a simple class is used which protects the raw data, allowing it to be modified during loading and then freezes it, providing
        only a constant pointer to the nodes to use.

        Parameters are first loaded from each launched node's `set_default_params` function, and then overwritten by a JSON file, if it exists.
        The file is searched for in whatever path is provided in the environment variable `RACHEL_PARAMS_FILE`, or else the default
        location `/etc/rachel/params.json`.
    */
    class Parameters
    {
    private:
        nlohmann::json params;
        bool finalized = false;
        const std::string err_msg = "Tried to modify parameters after finalizing. If you want to share values between nodes at runtime, after the intial parameter loading, there are other mechanisms for that, like topics.";

    public:
        void update_params(const nlohmann::json& new_params) {
            if (finalized) {
                throw std::runtime_error(err_msg);
            }

            params.update(new_params, true);
        }

        void load_from_file()
        {
            if (finalized)
            {
                throw std::runtime_error(err_msg);
            }

            std::string file_location = "/etc/rachel/params.json";
            bool accept_file_not_found = true;
            if (const char *new_file_loc = std::getenv("RACHEL_PARAMS_FILE"))
            {
                file_location = new_file_loc;
                accept_file_not_found = false;
            }

            std::ifstream file(file_location);
            if (file.is_open())
            {
                nlohmann::json new_data = nlohmann::json::parse(file);
                params.update(new_data, true);
            }
            else if (!accept_file_not_found)
            {
                std::stringstream ss;
                ss << "Environment variable RACHEL_PARAMS_FILE specified that parameters file should exist in " << file_location << " but it was not found, or failed to load.";
                throw std::runtime_error(ss.str());
            }
        }

        void finalize()
        {
            finalized = true;
        }

        param_t* get() const
        {
            return finalized ? &params : nullptr;
        }
    };
}