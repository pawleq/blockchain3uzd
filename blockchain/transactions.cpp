#include "header.h"

class transactions
{
    public :
        string id;
        string sId;
        string rId;
        double amount;
        bool bad = false;
};
    vector<transactions> trans;

class TransactionBox {
    public:
        vector<transactions> trans;
};
