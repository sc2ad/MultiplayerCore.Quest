#pragma once
#include "main.hpp"
#include "cpp-semver/shared/cpp-semver.hpp"

namespace MultiplayerCore::Utils {
    static const std::string ChromaID = "Chroma";
    static const std::string ChromaVersionRange = ">=2.5.7";

    static bool IsInstalled(std::string const& modName) {
        auto const modList = Modloader::getMods();
        auto modInfo = modList.find(modName);
        return (modInfo != modList.end());
    }

    static bool MatchesVersion(std::string const& modName, std::string const& versionRange) {
        auto const modList = Modloader::getMods();
        // Depending on when this is called, this call may not be necessary.
        // Ex, it could be after load in which case it would already have been loaded/not.
        // Or this function could be called a second time.
        auto modLoaded = Modloader::requireMod(modName);
        try {
            if (modLoaded) {
                auto modInfo = modList.find(modName);
                if (modInfo != modList.end()) {

                    getLogger().debug("Checking %s version '%s'", modName.c_str(), modInfo->second.info.version.c_str());
                    if (semver::satisfies(modInfo->second.info.version, versionRange)) {
                        getLogger().debug("%s version valid", modName.c_str());
                        return true;
                    }
                    getLogger().debug("%s version outdated", modName.c_str());
                    // mod is outdated
                }
            }
            return false;
        }
        catch (const semver::semver_error &e) {
            getLogger().error("SemVer Exception checking %s: %s", modName.c_str(), e.what());
            return false;
        }
    }
}