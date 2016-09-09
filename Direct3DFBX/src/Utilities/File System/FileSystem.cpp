/* ----------------------------------------------------------------------------
** Team Bear King
** © 2015 DigiPen Institute of Technology, All Rights Reserved.
**
** FileSystem.cpp
**
** Author:
** - Austin Brunkhorst - a.brunkhorst@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** --------------------------------------------------------------------------*/

#include <FileSystem.h>

namespace ursine
{
    namespace fs
    {		
        //std::string SafeFileName(const std::string &fileName, char replacement)
        //{
        //    auto sanitized = fileName;
		//
        //    std::string illegalChars = "\\/:?\"<>|";
		//
        //    for (auto &character : sanitized) 
        //    {
        //        if (illegalChars.find( character ) != std::string::npos)
        //            character = replacement;
        //    }
		//
        //    return sanitized;
        //}
		//
        //path MakeRelativePath(const path &from, const path &to)
        //{
        //    // start at the root path and while they are the same then do nothing then when they first
        //    // diverge take the remainder of the two path and replace the entire from path with ".."
        //    // segments.
        //    auto itFrom = from.begin( );
        //    auto itTo = to.begin( );
		//
        //    // loop through both
        //    while (itFrom != from.end( ) && itTo != to.end( ) && *itTo == *itFrom)
        //    {
        //        ++itTo;
        //        ++itFrom;
        //    }
		//
        //    path finalPath;
		//
        //    while (itFrom != from.end( ))
        //    {
        //        finalPath /= "..";
		//
        //        ++itFrom;
        //    }
		//
        //    while (itTo != to.end( ))
        //    {
        //        finalPath /= *itTo;
		//
        //        ++itTo;
        //    }
		//
        //    return finalPath;
        //}
		//
        //bool PathContainsFile(path directory, path file)
        //{
        //    // if directory ends with "/" and isn't the root directory, then the final
        //    // component returned by iterators will include "." and will interfere
        //    // with the std::equal check below, so we strip it before proceeding
        //    if (directory.filename( ) == ".")
        //        directory.remove_filename( );
		//
        //    // we're also not interested in the file's name.
        //    if (file.has_filename( ))
        //        file.remove_filename( );
		//
        //    // if directory has more components than file, then file can't possibly
        //    // reside in the directory
        //    auto directoryLength = std::distance( directory.begin( ), directory.end( ) );
        //    auto fileLength = std::distance( file.begin( ), file.end( ) );
		//
        //    if (directoryLength > fileLength)
        //        return false;
		//
        //    // this stops checking when it reaches directory.end(), so it's OK if file
        //    // has more directory components afterward. They won't be checked.
        //    return std::equal( directory.begin( ), directory.end( ), file.begin( ) );
        //}
    }
}