#include "DefHeader.h"
#include "FileHash.h"

bool common::File::Hashing::hash128bit(LPCWSTR file, XXH128_hash_t* ptrHash) {
    // Open File
    ScopedHandle hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (!hFile) {
        return false;
    }

    // Get File size and allocate temporary buffer
    DWORD fileSize = GetFileSize(hFile, NULL);
    void* fileBuffer = malloc(fileSize);
    EXPP_ASSERT(fileBuffer, "Memory allocation failed!");

    // Read file 
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, fileBuffer, fileSize, &fileSize, NULL)) {
        free(fileBuffer);
        return false;
    }

    // Hash File content
    *ptrHash = XXH3_128bits(fileBuffer, fileSize);

    // Free memory
    free(fileBuffer);

    // Passed
    return fileSize > 0;
}
