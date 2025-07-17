#include <fstream>      // for working with files
#include <iostream>     // for console output
#include <string>       // for std::string
#include <random>       // for generating random numbers and strings
#include <ctime>        // for getting current time
#include <sstream>      // for ostringstream — convenient string assembly

// Function generates a random string of the given length
std::string generate_random_string(size_t length) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
    static std::mt19937 rng(std::time(nullptr)); // initialize random number generator with current time
    static std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length); // reserve required size immediately
    for(size_t i = 0; i < length; ++i) {
        result += chars[dist(rng)]; // each character is randomly selected from chars
    }
    return result;
}

int main(int argc, char** argv) {
    try {
        // Check number of arguments (expecting filename and size)
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <output.xml> <size_mb>\n";
            return 1;
        }

        const std::string filename = argv[1]; // output file name
        const size_t target_size_mb = std::stoul(argv[2]); // size in MB to generate
        const size_t target_bytes = target_size_mb * 1024 * 1024; // convert to bytes

        std::ofstream out(filename, std::ios::binary); // open file for writing
        if (!out) {
            std::cerr << "Failed to open output file\n"; // error if file not opened
            return 1;
        }

        std::cout << "Generating " << filename << " (" << target_size_mb << " MB)\n";

        // Start writing XML file
        out << "<?xml version=\"1.0\"?>\n<root>\n";
        size_t current_size = out.tellp(); // get current file position (bytes written)

        std::mt19937 rng(std::time(nullptr)); // random number generator
        std::uniform_int_distribution<int> value_dist(0, 10000); // range for <value>

        size_t entry_count = 0; // number of entries made
        const size_t desc_length = 100; // description length

        // Until required size is reached (leave some space for </root>)
        while (current_size < target_bytes - 100) {
            std::ostringstream entry; // assemble entry in stream

            int value = value_dist(rng); // random number from 0 to 10000
            
            // Form one XML entry with three fields
            entry << "  <entry id=\"" << entry_count << "\">\n"
                  << "    <name>" << generate_random_string(20) << "</name>\n"
                  << "    <value>" << value << "</value>\n"
                  << "    <description>" << generate_random_string(desc_length) << "</description>\n"
                  << "  </entry>\n";

            const std::string entry_str = entry.str(); // get ready string
            out << entry_str; // write to file
            current_size += entry_str.size(); // update size counter
            
            // Every 1000 entries show progress
            if (entry_count % 1000 == 0) {
                std::cout << "\rProgress: " << (current_size / (1024.0 * 1024)) << "/" 
                          << target_size_mb << " MB";
            }
            entry_count++; // increment entry counter
        }

        // Finish XML
        out << "</root>\n";
        out.close(); // close file

        // Show result
        std::cout << "\nGenerated " << filename << " (" << entry_count << " entries, "
                  << (current_size / (1024.0 * 1024)) << " MB)\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n"; // show error if something went wrong
        return 1;
    }
}
