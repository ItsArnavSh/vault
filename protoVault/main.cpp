
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

enum colFlags { PRIMARY, COMPRESSED, INDEXED };
enum dataType { NUMBER, DECIMAL, TEXT, BLOB };

struct colDef {
    std::string name;
    dataType type;
    std::vector<colFlags> flags;
};

struct tableDef {
    std::string name;
    std::vector<colDef> columns;
};

std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\n");
    size_t last = str.find_last_not_of(" \t\n");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

dataType getType(const std::string &typeStr) {
    if (typeStr == "number") return NUMBER;
    if (typeStr == "decimal") return DECIMAL;
    if (typeStr == "text") return TEXT;
    if (typeStr == "blob") return BLOB;
    throw std::invalid_argument("Unknown type: " + typeStr);
}

colFlags getFlag(const std::string &flagStr) {
    if (flagStr == "primary") return PRIMARY;
    if (flagStr == "compressed") return COMPRESSED;
    if (flagStr == "indexed") return INDEXED;
    throw std::invalid_argument("Unknown flag: " + flagStr);
}

tableDef parseTable(std::ifstream &file) {
    tableDef table;
    std::string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '}') break;

        std::istringstream iss(line);
        std::string type, name, flag;
        colDef column;

        iss >> type >> name;
        column.name = name;
        column.type = getType(type);

        while (iss >> flag) {
            column.flags.push_back(getFlag(flag));
        }

        table.columns.push_back(column);
    }
    return table;
}

void generateClass(const tableDef &table, const std::string &outputFile) {
    std::ofstream out(outputFile);
    out << "#pragma once\n\n";
    out << "#include <string>\n\n";
    out << "class " << table.name << " {\npublic:\n";

    for (const auto &col : table.columns) {
        std::string typeStr;
        switch (col.type) {
            case NUMBER: typeStr = "int"; break;
            case DECIMAL: typeStr = "double"; break;
            case TEXT: typeStr = "std::string"; break;
            case BLOB: typeStr = "std::vector<unsigned char>"; break;
        }
        out << "    " << typeStr << " " << col.name << ";";
        if (!col.flags.empty()) {
            out << "  // ";
            for (const auto &flag : col.flags) {
                out << (flag == PRIMARY ? "PRIMARY " : (flag == COMPRESSED ? "COMPRESSED " : "INDEXED "));
            }
        }
        out << "\n";
    }

    out << "\n    " << table.name << "(";
    for (size_t i = 0; i < table.columns.size(); i++) {
        if (i > 0) out << ", ";
        out << "const " << (table.columns[i].type == TEXT ? "std::string&" : (table.columns[i].type == BLOB ? "std::vector<unsigned char>&" : "")) << table.columns[i].name;
    }
    out << ") : ";
    for (size_t i = 0; i < table.columns.size(); i++) {
        if (i > 0) out << ", ";
        out << table.columns[i].name << "(" << table.columns[i].name << ")";
    }
    out << " {}\n";

    out << "\n    void save() {\n        // Save logic\n    }\n";
    out << "};\n";
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file!\n";
        return 1;
    }

    std::string tableName;
    file >> tableName; // Read 'table'
    file >> tableName; // Read actual table name

    tableDef table = parseTable(file);
    table.name = tableName;

    generateClass(table, "output.cpp");
    std::cout << "Class generated successfully in output.h\n";
    return 0;
}
