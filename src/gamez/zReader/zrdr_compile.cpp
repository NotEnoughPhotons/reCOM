#include "zrdr.h"

#include "SDL3/SDL.h"

bool CRdrFile::Compile()
{
    CFileIO writer;

    if (!writer.Open("data/zrdr/compiled/test.rdr", "wb"))
    {
        SDL_Log("Error in compiling zReader file! Cannot open file for editing.");
        return false;
    }

    // Get the total number of characters (with null terminator)
    u32 numChars = 0;

    for (const char* str : m_strings)
        numChars += SDL_strlen(str) + 1;

    // Write the number of characters to the header
    writer.fwrite(&numChars, sizeof(u32));

    // Write the total size of the string table object
    u32 tableObjectSize = sizeof(this);
    writer.fwrite(&tableObjectSize, sizeof(u32));

    // Begin writing the string table entries
    for (const char* str : m_strings)
    {
        char buf[256];
        memset(buf, 0, 256);
        sprintf_s(buf, 256, "%s\0", str);
        writer.fwrite(buf, SDL_strlen(str) + 1);
    }

    // Add any padding bytes if necessary
    // Alignment is 16 bytes for PS2 GameZ files
    s32 padding = 16 - writer.ftell() % 16;
	
    char* padChars = new char[padding];
    memset(padChars, 0, padding);
    writer.fwrite(padChars, padding);
    delete[] padChars;
    padChars = nullptr;

    // Recursively add the tag tree
    if (!CompileRecursive(array, writer))
    {
        SDL_Log("Error in zReader compilation! Recursive error");
        writer.Close();
        return false;
    }

    writer.Close();
	
    return true;
}

bool CRdrFile::CompileRecursive(_zrdr* tag, CFileIO& writer)
{
    if (!tag)
        return false;
    
    writer.fwrite(tag, sizeof(_zrdr));

    
    
    return true;
}