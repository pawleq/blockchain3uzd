#include "functions.cpp"

using namespace std;

int main ()
{
    generate_user();
    generate_transactions(user);
    generate_100trans(trans);
    merkle(trans);
    //mining(trans, user);
}
