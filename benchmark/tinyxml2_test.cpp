#include <tinyxml2.h>
#include <chrono>
#include <iostream>

using namespace tinyxml2;
using clk = std::chrono::high_resolution_clock;

int main(int argc, char** argv) {
    // Check that there are two arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.xml> <output.xml>";
        return 1;
    }

    const char* inPath  = argv[1];  // input XML
    const char* outPath = argv[2];  // file to save

    // Mark start of reading
    auto t1 = clk::now();

    XMLDocument doc;
    // Try to load from file
    if (doc.LoadFile(inPath) != XML_SUCCESS) {
        std::cerr << "Load error";  // if failed
        return 1;
    }

    // Time after loading
    auto t2 = clk::now();

    // Traverse all nodes
        // Traversal is needed to guarantee the parser actually built the tree and we are working with it,
        // prevent compiler optimization,
        // measure overhead of node traversal in memory.
    for (XMLElement* e = doc.FirstChildElement(); e; e = e->NextSiblingElement()) {
        // Do nothing - this loop is only for traversal testing
    }

    // Mark start of writing
    auto t3 = clk::now();

    // Save to output file
    if (doc.SaveFile(outPath) != XML_SUCCESS) {
        std::cerr << "Save error";  // failed to write
        return 1;
    }

    // Time after writing
    auto t4 = clk::now();

    // Calculate durations
    auto read_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto write_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();

    // Output results
    std::cout << "tinyxml2: read=" << read_ms << "ms write=" << write_ms << "ms";

    return 0;
}
