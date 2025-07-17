#include <pugixml.hpp>
#include <chrono>
#include <iostream>

// Timer
using clk = std::chrono::high_resolution_clock;

int main(int argc, char** argv) {
    // Check arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.xml> <output.xml>";
        return 1;
    }

    const char* inPath  = argv[1];  // input file
    const char* outPath = argv[2];  // output file

    // Mark start time of reading
    auto t1 = clk::now();

    // Load XML document
    pugi::xml_document doc;
    if (!doc.load_file(inPath)) {
        std::cerr << "Load error";  // if failed to read
        return 1;
    }

    // Mark end time of reading
    auto t2 = clk::now();

    // Traverse all nodes
    // Traversal is needed to guarantee the parser actually built the tree and we are working with it,
    // prevent compiler optimization,
    // measure overhead of node traversal in memory.
    for (auto node = doc.first_child(); node; node = node.next_sibling()) {
        // Do nothing - this loop is only for traversal testing
    }

    // Mark time before writing
    auto t3 = clk::now();

    // Save back to file
    if (!doc.save_file(outPath)) {
        std::cerr << "Save error";  // if failed to write
        return 1;
    }

    // Mark end time of writing
    auto t4 = clk::now();

    // Calculate durations in milliseconds
    auto read_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    auto write_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();

    // Output results
    std::cout << "pugixml: read=" << read_ms << "ms write=" << write_ms << "ms";

    return 0;
}
