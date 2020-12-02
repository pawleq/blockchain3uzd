#include "header.h"
#include "transactions.cpp"

class Block
{
public:

    string hash;
    vector<transactions> trans;

};

vector<Block> blocks;
