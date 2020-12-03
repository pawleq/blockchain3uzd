#include "header.h"
#include "user.cpp"
#include "hash.cpp"
#include "block.cpp"

string alphaNumericStrings(int length)
{
    static const char alphanum[] = "0123456789" "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string randomString("", length);
    for (int i = 0; i < length; i++)
    {
        randomString[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return randomString;
}
vector<users> generate_user()
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<unsigned> rndkey(0, 15);
    uniform_int_distribution<unsigned> rndwallet(1000, 1000000);

    static const char symbols[] = "0123456789" "abcdef";

    for (int i = 0; i < USERAMOUNT; i++)
    {
        users temp;
        temp.UserName = alphaNumericStrings(NAMELENGTH);
        for(int j = 0; j < KEYLENGTH; j++)
        {
            temp.key = temp.key + symbols[rndkey(gen)];
        }
        temp.balance = rndwallet(gen);
        user.push_back(temp);
        //cout <<user[i].UserName<<" "<<user[i].key<<" "<<user[i].balance<<endl;
    }
    return user;
}
vector<transactions> generate_transactions(vector<users> user)
{
    transactions transaction;
    for(int i = 0; i < TRANSAMOUNT; i++)
    {
        transaction.sId = user.at(rand()%(USERAMOUNT)).key;
        do
        {
            transaction.rId = user.at(rand()%(USERAMOUNT)).key;
        }
        while (transaction.sId == transaction.rId);
        transaction.amount = rand()%(USERAMOUNT);
        transaction.id = hashing(transaction.sId + transaction.rId + to_string(transaction.amount));
        trans.push_back(transaction);
        //cout <<trans[i].sId<<" "<<trans[i].rId<<" "<<trans[i].amount<<" "<<trans[i].id<<endl;
    }
    return trans;
}
vector<transactions> generate_100trans(vector<transactions>& trans) //to place 100 different trans to a block
{
    vector<transactions> returnTransactions;
    if (trans.size() > 100)
    {
        int nums[trans.size()];
        for (int i = 0; i < trans.size(); i++)
            nums[i] = i;
        shuffle(nums, nums + trans.size(), std::mt19937(std::random_device()()));
        for (int i = 0; i < 100; i++)
        {
            returnTransactions.push_back(trans.at(i));
        }
    }
    else
    {
        returnTransactions = trans;
    }
    return returnTransactions;
}
bc::hash_digest create_merkle(bc::hash_list& merkle)
{
    if (merkle.empty())
        return bc::null_hash;
    else if (merkle.size() == 1)
        return merkle[0];
    while (merkle.size() > 1)
    {
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());
        assert(merkle.size() % 2 == 0);
        bc::hash_list new_merkle;
        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            bc::data_chunk concat_data(bc::hash_size * 2);
            auto concat = bc::serializer<
                          decltype(concat_data.begin())>(concat_data.begin());
            concat.write_hash(*it);
            concat.write_hash(*(it + 1));
            bc::hash_digest new_root = bc::bitcoin_hash(concat_data);
            new_merkle.push_back(new_root);
        }
        merkle = new_merkle;
        cout << "Current merkle hash list:" << endl;
        for (const auto& hash: merkle)
            cout << " " << bc::encode_base16(hash) << endl;
        cout << endl;
    }

    return merkle[0];
}
string generate_hash(const string& prevBlockHash, vector<transactions> trans, long long int nonceTo)
{

    bc::hash_list tx_hashes; //sukuriamas naujas hash listas, kur storinsim trans hashus
    for (const auto& transaction : trans)
    {
        char chars[65];
        strcpy(chars, transaction.id.c_str()); //convertinam transaction id stringus i char lista
        tx_hashes.push_back(bc::hash_literal(chars)); //pridedam konvertuotus id i hash lista
    }
    const bc::hash_digest merkle_root = create_merkle(tx_hashes); //hashu listui panaudojam merkle funkcija kuri returnina viena root hasha
    auto merkleHash = bc::encode_base16(merkle_root); //endodinam merke roota gauta

    string newBlockHash;
    time_t timestamp = std::time(nullptr);
    string version = "version1";
    string difficultyTarget = "0";
    long long int nonce = 0;
    for (long long int i = nonce; i <= nonceTo; i++)
    {
        newBlockHash = hashing(
                           prevBlockHash + to_string(timestamp) + version + merkleHash + to_string(i) + difficultyTarget);
        if (newBlockHash.substr(0, difficultyTarget.size()) == difficultyTarget)
        {
            return newBlockHash;
        }
    }
    return "";
}
int main()
{
    vector<users> user = generate_user();
    vector<transactions> trans = generate_transactions(user);
    TransactionBox transactionBox;
    Block block;
    int index;
    int nums[5] = {0, 1, 2, 3, 4};
    while(!trans.empty())
    {
        vector<TransactionBox> transactionBoxes;
        for (int i = 0; i < 5; i++)
        {
            transactionBox.trans = generate_100trans(trans);
            transactionBoxes.push_back(transactionBox);
        }

        shuffle(nums, nums + 5, std::mt19937(std::random_device()()));

        string hash;
        long long int nonceTo = TRANSAMOUNT;

        int i = 0;
        while (hash.empty())
        {
            while (i < 5 && hash.empty())
            {
                if (blocks.empty())
                {
                    hash = generate_hash("0", transactionBoxes.at(i).trans, nonceTo);
                }
                else
                {
                    hash = generate_hash(blocks.back().hash, transactionBoxes.at(i).trans, nonceTo);
                }
                i++;
            }

            if (!hash.empty())
            {
                index = i;
            }
            else
            {
                nonceTo += TRANSAMOUNT;
            }
        }
        for (auto & transaction : transactionBoxes.at(index).trans)
        {

            int sId = find_if(user.begin(), user.end(),
                              [transaction](const users &user)
            {
                return user.key == transaction.sId;
            }) -
            user.begin();

            int rId = find_if(user.begin(), user.end(), [transaction](const users&user)
            {
                return user.key == transaction.rId;
            }) - user.begin();

            int transactionNumber = find_if(trans.begin(), trans.end(),
                                            [transaction](const transactions &transactionHere)
            {
                return transactionHere.id == transaction.id;
            }) - trans.begin();

            string transactionId = hashing(transaction.sId + transaction.rId + to_string(transaction.amount));

            if (user.at(sId).balance >= transaction.amount && transaction.id == transactionId)
            {
                user.at(sId).balance -= transaction.amount;
                user.at(rId).balance += transaction.amount;
            }
            else
            {
                transaction.bad = true;
            }

            trans.erase(trans.begin() + transactionNumber);
        }
        cout <<blocks.size()+1<< " Mined block : " << hash << endl;
        block.hash = hash;
        block.trans = transactionBoxes.at(index).trans;
        blocks.push_back(block);
    }
}
