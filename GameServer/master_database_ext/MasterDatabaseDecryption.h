#ifndef MASTER_DATABASE_DECRYPTION_H
#define MASTER_DATABASE_DECRYPTION_H

#include <ios>
#include <vector>

namespace MasterDatabase {
    std::vector<char> readAndDecryptFile(std::istream &stream, const unsigned char *key, const unsigned char *iv);
}

#endif
