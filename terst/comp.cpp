#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <bitset>

using namespace std;

// Huffman Tree Node
struct Node {
    int value;
    int freq;
    Node* left;
    Node* right;
    Node(int v, int f) : value(v), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator for min-heap priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;  // Min heap
    }
};

// Generate Huffman Codes
void buildHuffmanCodes(Node* root, string code, unordered_map<int, string>& huffmanCodes) {
    if (!root) return;
    if (!root->left && !root->right) { // Leaf node
        huffmanCodes[root->value] = code;
    }
    buildHuffmanCodes(root->left, code + "0", huffmanCodes);
    buildHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Compress the input data
vector<bool> compressData(const vector<int>& data, unordered_map<int, string>& huffmanCodes) {
    vector<bool> compressedBits;
    for (int num : data) {
        for (char bit : huffmanCodes[num]) {
            compressedBits.push_back(bit == '1');
        }
    }
    return compressedBits;
}

// Write compressed data to file
void writeCompressedFile(const string& filename, const vector<bool>& compressedBits, const unordered_map<int, string>& huffmanCodes) {
    ofstream file(filename, ios::binary);

    // Store the Huffman Code Table
    uint16_t tableSize = huffmanCodes.size();
    file.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));

    for (const auto& [value, code] : huffmanCodes) {
        file.write(reinterpret_cast<const char*>(&value), sizeof(value)); // Store value
        uint8_t codeLen = code.size();
        file.write(reinterpret_cast<const char*>(&codeLen), sizeof(codeLen)); // Store code length
        bitset<32> bitCode(stoi(code, nullptr, 2)); // Convert code to binary
        file.write(reinterpret_cast<const char*>(&bitCode), sizeof(bitCode)); // Store bit pattern
    }

    // Write compressed data
    uint8_t byte = 0;
    int bitCount = 0;
    for (bool bit : compressedBits) {
        byte = (byte << 1) | bit;
        bitCount++;
        if (bitCount == 8) { // Write full byte
            file.write(reinterpret_cast<const char*>(&byte), 1);
            byte = 0;
            bitCount = 0;
        }
    }

    // Write any remaining bits
    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        file.write(reinterpret_cast<const char*>(&byte), 1);
    }

    file.close();
}

// Read Huffman Code Table from file
unordered_map<string, int> readHuffmanTable(ifstream& file) {
    unordered_map<string, int> huffmanTable;
    uint16_t tableSize;
    file.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));

    for (int i = 0; i < tableSize; ++i) {
        int value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        uint8_t codeLen;
        file.read(reinterpret_cast<char*>(&codeLen), sizeof(codeLen));
        bitset<32> bitCode;
        file.read(reinterpret_cast<char*>(&bitCode), sizeof(bitCode));
        string code = bitCode.to_string().substr(32 - codeLen); // Extract relevant bits
        huffmanTable[code] = value;
    }

    return huffmanTable;
}

// Decompress data
vector<int> decompressData(const string& filename) {
    ifstream file(filename, ios::binary);
    unordered_map<string, int> huffmanTable = readHuffmanTable(file);

    vector<int> decompressedData;
    string currentBits = "";
    char byte;
    while (file.read(&byte, 1)) {
        for (int i = 7; i >= 0; --i) {
            currentBits += ((byte >> i) & 1) ? '1' : '0';
            if (huffmanTable.find(currentBits) != huffmanTable.end()) {
                decompressedData.push_back(huffmanTable[currentBits]);
                currentBits = "";
            }
        }
    }

    file.close();
    return decompressedData;
}

int main() {
    // Example input data (can be changed)
    vector<int> data = {3, 3, 3, 7, 7, 2, 2, 2, 2, 9, 9, 6};

    // Step 1: Calculate frequencies
    unordered_map<int, int> freqMap;
    for (int num : data) freqMap[num]++;

    // Step 2: Build Huffman Tree
    priority_queue<Node*, vector<Node*>, Compare> minHeap;
    for (auto [value, freq] : freqMap) minHeap.push(new Node(value, freq));
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();
        Node* parent = new Node(-1, left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }

    Node* root = minHeap.top(); // Final tree root

    // Step 3: Generate Huffman Codes
    unordered_map<int, string> huffmanCodes;
    buildHuffmanCodes(root, "", huffmanCodes);

    // Step 4: Compress Data
    vector<bool> compressedBits = compressData(data, huffmanCodes);

    // Step 5: Write Compressed File
    writeCompressedFile("output.bin", compressedBits, huffmanCodes);

    // Print Encoded Binary
    cout << "Encoded Binary: ";
    for (bool bit : compressedBits) cout << bit;
    cout << endl;

    // Step 6: Decompress & Verify
    vector<int> decompressedData = decompressData("output.bin");

    cout << "Decompressed Data: ";
    for (int num : decompressedData) cout << num << " ";
    cout << endl;

    return 0;
}
