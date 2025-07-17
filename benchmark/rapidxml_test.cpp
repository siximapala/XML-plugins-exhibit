#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <iterator>

using namespace rapidxml;
using clk = std::chrono::high_resolution_clock;

// Function to load file into buffer
static std::vector<char> load_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open input file: " + filename);
    
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0);
    
    std::vector<char> buf(size + 1);
    in.read(buf.data(), size);
    buf[size] = '\0';
    
    return buf;
}

// Function to print XML to stream
void print_xml(std::ostream& out, xml_document<>& doc) {
    rapidxml::print(std::ostream_iterator<char>(out), doc, 0);
}

int main(int argc, char** argv) {
    try {
        if (argc != 3) {
            throw std::runtime_error("Usage: " + std::string(argv[0]) + 
                " <input.xml> <output.xml>");
        }

        const std::string input_file  = argv[1];  // path to input XML
        const std::string output_file = argv[2];  // path to output XML

        // 1. Reading and parsing XML
        auto t1 = clk::now();                      // start reading timer
        auto buf = load_file(input_file);          // read file into memory
        xml_document<> doc;
        doc.parse<parse_default>(buf.data());      // parse from buffer
        auto t2 = clk::now();                      // end of reading/parsing

        // Traverse all nodes
        // Traversal is needed to guarantee the parser actually built the tree and we are working with it,
        // prevent compiler optimization,
        // measure overhead of node traversal in memory.
        for (auto node = doc.first_node(); node; node = node->next_sibling()) {
            // Do nothing - this loop is only for traversal testing
        }

        // Write XML
        auto t3 = clk::now();                      // start writing timer
        {
            std::ofstream out(output_file);
            if (!out) throw std::runtime_error("Cannot open output file: " + output_file);
            print_xml(out, doc);                   // выводим документ в файл
        } // file closes when leaving block
        auto t4 = clk::now();                      // end of writing

        // Calculate times in milliseconds
        auto read_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        auto write_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3);
        
        // Output results
        std::cout << "read="  << read_ms.count() 
                  << "ms write=" << write_ms.count() << "ms\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";  // output error
        return 1;
    }
    
    return 0;
}
