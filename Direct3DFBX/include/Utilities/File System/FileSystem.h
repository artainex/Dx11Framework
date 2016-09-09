/* ----------------------------------------------------------------------------
** Team Bear King
** © 2015 DigiPen Institute of Technology, All Rights Reserved.
**
** FileSystem.h
**
** Author:
** - Austin Brunkhorst - a.brunkhorst@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** --------------------------------------------------------------------------*/

#pragma once

#include <boost/filesystem.hpp>
#include <unordered_set>
#include <unordered_map>

#if defined(URSINE_DEPENDENCY_FileWatcher)

#include <FileWatcher.h>

#endif

namespace ursine
{
    namespace fs
    {
        using namespace boost::filesystem;

#if defined(URSINE_DEPENDENCY_FileWatcher)

        using namespace efsw;

#endif
        
        typedef boost::hash<path> PathHasher;
        typedef std::vector<path> PathList;
        typedef std::unordered_set<path, PathHasher> PathSet;

        template<typename T>
        using PathMap = std::unordered_map<path, T, PathHasher>;

        typedef directory_iterator DirectoryIterator;
        typedef recursive_directory_iterator RecursiveDirectoryIterator;

        std::string SafeFileName(const std::string &fileName, char replacement);

        path MakeRelativePath(const path &from, const path &to);
        bool PathContainsFile(path directory, path file);
    }
}