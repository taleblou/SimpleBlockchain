# **SimpleBlockchainCPP**

A basic blockchain implementation in C++ that includes block mining, transaction management, and chain validation.

## **Features**

* Implements a simple blockchain with proof-of-work mining  
* Uses SHA-256 hashing from OpenSSL  
* Allows transaction creation and block validation  
* Demonstrates how blocks are linked using hashes

## **Requirements**

* C++ compiler (G++ or MSVC)  
* OpenSSL library

## **Installation**

Clone the repository:  
git clone https://github.com/yourusername/SimpleBlockchainCPP.git

1. cd SimpleBlockchainCPP  
2. Install OpenSSL (if not already installed):  
   * Linux:  
     sudo apt-get install libssl-dev  
   * Windows: Download and install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html  
3. Compile the code:  
   g++ \-o blockchain main.cpp \-lssl \-lcrypto  
4. Run the program:  
   ./blockchain

## **Usage**

* The program simulates a blockchain with mining and transaction processing.  
* It adds transactions, mines blocks, and prints the blockchain.  
* The validity of the chain is checked at the end.

## **Example Output**

Block mined: 0000a1b2c3d4...  
Block mined: 0000f5e6d7c8...  
Blockchain is valid: Yes

## **License**

This project is licensed under the MIT License.

## **Contributions**

Contributions are welcome\! Feel free to fork and submit pull requests.

## **Contact**

For any questions or suggestions, feel free to reach out\!
