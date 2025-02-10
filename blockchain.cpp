#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <openssl/sha.h>

using namespace std;

class Transaction {
public:
    string sender;
    string receiver;
    double amount;

    Transaction(string s, string r, double a) : sender(s), receiver(r), amount(a) {}
};

class Block {
private:
    int index;
    time_t timestamp;
    vector<Transaction> transactions;
    string previousHash;
    string hash;
    int nonce;

    string calculateHash() const {
        stringstream ss;
        ss << index << timestamp << transactions.size() << previousHash << nonce;
        string input = ss.str();
        
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)input.c_str(), input.size(), digest);
        
        stringstream hashss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            hashss << hex << setw(2) << setfill('0') << (int)digest[i];
        }
        
        return hashss.str();
    }

public:
    Block(int idx, const vector<Transaction>& txs, const string& prevHash)
        : index(idx), transactions(txs), previousHash(prevHash) {
        timestamp = time(nullptr);
        nonce = 0;
        hash = calculateHash();
    }

    void mineBlock(int difficulty) {
        string target(difficulty, '0');
        while(hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
        }
        cout << "Block mined: " << hash << endl;
    }

    string getHash() const { return hash; }
    string getPreviousHash() const { return previousHash; }
    vector<Transaction> getTransactions() const { return transactions; }
};

class Blockchain {
private:
    vector<Block> chain;
    int difficulty;
    vector<Transaction> pendingTransactions;

public:
    Blockchain() {
        difficulty = 4;
        chain.emplace_back(Block(0, vector<Transaction>(), "0"));
    }

    void addTransaction(const Transaction& tx) {
        pendingTransactions.push_back(tx);
    }

    void minePendingTransactions() {
        Block newBlock(chain.size(), pendingTransactions, chain.back().getHash());
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
        pendingTransactions.clear();
    }

    bool isChainValid() const {
        for(size_t i = 1; i < chain.size(); i++) {
            const Block& current = chain[i];
            const Block& previous = chain[i-1];

            if(current.getHash() != current.calculateHash()) return false;
            if(current.getPreviousHash() != previous.getHash()) return false;
        }
        return true;
    }

    void printChain() const {
        for(const Block& block : chain) {
            cout << "Block:" << endl;
            cout << "Index: " << block.getIndex() << endl;
            cout << "Hash: " << block.getHash() << endl;
            cout << "Previous Hash: " << block.getPreviousHash() << endl;
            cout << "Transactions: " << block.getTransactions().size() << endl;
            cout << endl;
        }
    }
};

int main() {
    Blockchain bc;

    // Add some transactions
    bc.addTransaction(Transaction("Alice", "Bob", 5.0));
    bc.addTransaction(Transaction("Bob", "Charlie", 2.5));

    // Mine block with these transactions
    bc.minePendingTransactions();

    // Add more transactions
    bc.addTransaction(Transaction("Charlie", "Alice", 1.0));
    bc.addTransaction(Transaction("Dave", "Eve", 3.75));

    // Mine another block
    bc.minePendingTransactions();

    // Print the blockchain
    bc.printChain();

    // Validate the chain
    cout << "Blockchain is valid: " << (bc.isChainValid() ? "Yes" : "No") << endl;

    return 0;
}
