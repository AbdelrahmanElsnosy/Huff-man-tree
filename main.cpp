#define _CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 265
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <string>
#include <sstream>
#include <bitset>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <filesystem>
#include <unordered_map>
using namespace std;
typedef struct node {
    char data;
    int frequency;
    node* left;
    node* right;
}node;
typedef struct compare {
    bool operator()(node const* node1, node const* node2) {
        return node1->frequency > node2->frequency;
    }
};
bool isLeaf(node* root) {
    return !(root->left) && !(root->right);
}
char str_to_char(string binaryString) {
    stringstream sstream(binaryString);
    bitset<8> bits;
    sstream >> bits;
    return char(bits.to_ulong());
}
string char_to_str(char ch)
{
    bitset<8> temp(ch);
    return temp.to_string();
}
void fillMIN_HEAP(node* newNode, priority_queue<node*, vector<node*>, compare>* min_heap) {
    min_heap->push(newNode);
}
void huffman(priority_queue<node*, vector<node*>, compare>* min_heap) {
    node* left, * right;
    left = right = NULL;
    int size = min_heap->size();
    for (int i = 1; i < size; i++) {
        node* parent = new(node);
        left = min_heap->top();
        min_heap->pop();
        right = min_heap->top();
        min_heap->pop();
        if (left->data != '\0') {
            left->left = NULL;
            left->right = NULL;
        }
        if (right->data != '\0') {
            right->left = NULL;
            right->right = NULL;
        }
        parent->left = left;
        parent->right = right;
        parent->frequency = left->frequency + right->frequency;
        parent->data = '\0';
        min_heap->push(parent);
    }
}
void tree_traversal(node* root, unordered_map<char, string>& codes, string binary) {
    if (root == NULL)
        return;
    if (isLeaf(root))
        codes[root->data] = binary;
    else
    {
        tree_traversal(root->left, codes, binary + "0");
        tree_traversal(root->right, codes, binary + "1");
    }
}
void compress(const string filename) {
    clock_t t1, t2;
    t1 = clock();
    unordered_map <char, int> fileChars;
    char tempChar;
    ifstream read(filename);
    string output = "outputCOMPRESSED.txt";
    ofstream write(output.c_str(), std::ofstream::binary);
    while (read.get(tempChar))
    {
        if (fileChars.find(tempChar) == fileChars.end()) {
            fileChars[tempChar] = 1;
        }
        else
            fileChars[tempChar]++;
    }
    priority_queue<node*, vector<node*>, compare> huffmanCode;
    for (auto it = fileChars.begin(); it != fileChars.end(); it++) {
        node* newNode = new(node);
        newNode->data = it->first;
        newNode->frequency = it->second;
        fillMIN_HEAP(newNode, &huffmanCode);
    }
    huffman(&huffmanCode);
    node* tree = huffmanCode.top();
    unordered_map<char, string>codes;
    string binary;
    string temp;
    tree_traversal(tree, codes, binary);
    for (auto it = codes.begin(); it != codes.end(); it++) {
        if (it->first == '\n') {
            write << "\\n" << ":" << it->second << endl;
        }
        else {
            write << it->first << ":" << it->second << endl;
        }
    }
    write << "STOP U VIOLATING THE LAW" << endl;
    read.close();
    read.open(filename);
    while (read.get(tempChar)) {
        string code = codes[tempChar];
        temp.append(code);
        if (temp.size() > 8) {
            write << str_to_char(temp.substr(0, 8));
            temp = temp.substr(8);
        }
    }
    int  padding = 8 - temp.size() % 8;
    while (true) {
        if (temp.size() > 8) {
            write << str_to_char(temp.substr(0, 8));
            temp = temp.substr(8);
        }
        else {
            int size = temp.size();
            for (int i = 0; i < 8 - size; i++) {
                temp.append("0");
            }
            write << str_to_char(temp);
            break;
        }
    }
    cout << "Codes of each character" << endl;
    cout << "-------------------------" << endl;
    for (auto it = codes.begin(); it != codes.end(); it++) {
        if (it->first == '\n') {
            cout << "\\n" << " " << it->second << endl;
        }
        else {
            cout << it->first << ":" << it->second << endl;
        }
    }
    t2 = clock();
    cout << "compression successful!\n" << endl;
    cout << "time: " << ((double)t2 - (double)t1) / (double)CLOCKS_PER_SEC << endl << endl;
    std::streampos fsize = 0;
    std::ifstream myfile(filename, ios::in);  // File is of type const char*
    fsize = myfile.tellg();         // The file pointer is currently at the beginning
    myfile.seekg(0, ios::end);      // Place the file pointer at the end of file
    fsize = myfile.tellg() - fsize;
    myfile.close();

    static_assert(sizeof(fsize) >= sizeof(long long), "Oops.");

    cout << "size uncompressed is: " << fsize / 1000 << " Kbytes.\n";


    std::streampos fsize2 = 0;
    std::ifstream myfile2(output, ios::in);  // File is of type const char*
    fsize2 = myfile2.tellg();         // The file pointer is currently at the beginning
    myfile2.seekg(0, ios::end);      // Place the file pointer at the end of file
    fsize2 = myfile2.tellg() - fsize2;
    myfile2.close();

    static_assert(sizeof(fsize2) >= sizeof(long long), "Oops.");
    cout << "size compressed is: " << fsize2 / 1000 << " kbytes.\n";
    cout << "Compression ratio is: " << ((double)fsize2 / fsize * 1.0)*100 <<"%"<< endl;
}
void decompress(const string filename) {
    clock_t t1, t2;
    t1 = clock();
    std::ifstream read(filename.c_str(), std::ifstream::binary);;
    ofstream write("outputDECOMPRESSED.txt");
    unordered_map<string, char>chars;
    string line;
    while (getline(read, line)) {
        if (line.compare("STOP U VIOLATING THE LAW") == 0)
            break;
        else if (line.substr(0, 2).compare("\\n") == 0) {
            char tempChar = '\n';
            chars[line.substr(3)] = tempChar;
        }
        else {
            chars[line.substr(2)] = line.at(0);
        }
    }
    char tempChar;
    string tempString;
    while (read.get(tempChar)) {
        tempString += char_to_str(tempChar);
        int i = 1;
        while (!tempString.empty()) {
            if (chars.find(tempString.substr(0, i)) != chars.end()) {
                write << chars[tempString.substr(0, i)];
                tempString = tempString.substr(i);
                i = 1;
                continue;
            }
            i++;
            if (i > tempString.size()) 
                break;
        }
    }
    read.get(tempChar);
    int padding = tempChar - 48;
    tempString = tempString.substr(0, tempString.size() - padding);
    int i = 1;
    t2 = clock();
    cout << "decompression successful!\n" << endl;
    cout << "time: " << ((double)t2 - (double)t1) / (double)CLOCKS_PER_SEC<< endl << endl;
}
int main() {
    char filename[MAX_SIZE];
    string mod;
    cout << "Please Enter file Name: ";
    cin >> filename;
    cout << "Please Enter Policy " << endl;
    cout << "--------------------" << endl;
    cout << "1-compress\n2-decompress\n3-exit" << endl;
    cin >> mod;
    while (mod.compare("3") != 0)
    {
        try
        {
            int modInt = stoi(mod);
            if (modInt == 1) {
                compress(filename);
            }
            else
                decompress(filename);
        }
        catch (const std::exception&)
        {
            if (mod.compare("COMPRESS") == 0 || mod.compare("compress") == 0) {
                compress(filename);
            }
            else if (mod.compare("DECOMPRESS") == 0 || mod.compare("decompress") == 0) {
                decompress(filename);
            }
            else
                cout << "You are officially GAY" << endl;
        }

        cout << "Please Enter file Name: ";
        cin >> filename;
        cout << "Please Enter Policy " << endl;
        cout << "--------------------" << endl;
        cout << "1-compress\n2-decompress\n3-exit" << endl;
        cin >> mod;

    }

    return 0;
}