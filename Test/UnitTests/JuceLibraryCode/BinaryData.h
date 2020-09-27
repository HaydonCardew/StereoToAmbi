/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   README_md;
    const int            README_mdSize = 1682;

    extern const char*   CMakeLists_txt;
    const int            CMakeLists_txtSize = 12314;

    extern const char*   CONTRIBUTORS;
    const int            CONTRIBUTORSSize = 1408;

    extern const char*   LICENSE;
    const int            LICENSESize = 1475;

    extern const char*   README_md2;
    const int            README_md2Size = 9886;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
