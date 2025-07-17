# Project for comparing the performance of XML libraries: PugiXML, TinyXML2, and RapidXML.

## Requirements
 - CMake 3.10+
 - C++20 compiler
 - Python 3.8+

## Project structure
 - **benchmark/** - library tests (pugixml, tinyxml2, rapidxml).
 - **ext/** - sources of used libraries
 - **xml_generator.cpp** - XML file generator
 - **test_runner.cpp** - main executable: generates tests using xml_generator.cpp (if not yet generated) and runs files from /benchmark folder.
 - **visualizer.py** - program based on matplotlib and pandas that shows speed diagrams of libraries from the csv table.
 - **CMakeLists.txt** - cmake build instruction.
## Building the project

### Windows (Visual Studio)
```powershell
# 1. Clone the repository with submodules
git clone --recurse-submodules https://github.com/siximapala/XML-plugins-exhibit.git

# 2. Create a build folder and go into it
mkdir build
cd build

# 3. Generate the build
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Build the project
cmake --build . --config Release
```

After this, in `build/Release` you will find:

-   `xml_generator.exe`
-   `test_runner.exe`
-   `pugixml_test.exe`
-   `tinyxml2_test.exe`
-   `rapidxml_test.exe`

## Running benchmarks

Go to the `build/Release` folder and run:

```powershell
.\test_runner.exe -a 5
```

-   `-a N` or `--amount N` - number of runs for each test (default is 5 if not specified).

You can add your own file:

```powershell
.\test_runner.exe -a 5 -c custom_test.xml
```
-   `-c file` or `--custom file` - your XML file. It will run after tests for 10, 50, 100, and 200 MB. If the file is missing, it will show an error and exit.

After execution, `results.csv` will appear. Example structure:
```csv
Library,File,Run,Read(ms),Write(ms)
pugixml_test,data_10mb.xml,1,18,43
pugixml_test,data_10mb.xml,2,20,34
pugixml_test,data_10mb.xml,3,20,38
tinyxml2_test,data_10mb.xml,1,72,123
tinyxml2_test,data_10mb.xml,2,102,128
tinyxml2_test,data_10mb.xml,3,65,119
rapidxml_test,data_10mb.xml,1,55,369
rapidxml_test,data_10mb.xml,2,42,369
rapidxml_test,data_10mb.xml,3,46,347
pugixml_test,data_50mb.xml,1,140,157
pugixml_test,data_50mb.xml,2,81,140
pugixml_test,data_50mb.xml,3,88,155
tinyxml2_test,data_50mb.xml,1,334,591
tinyxml2_test,data_50mb.xml,2,301,561
tinyxml2_test,data_50mb.xml,3,302,6031
rapidxml_test,data_50mb.xml,1,244,1718
rapidxml_test,data_50mb.xml,2,221,1740
rapidxml_test,data_50mb.xml,3,222,1717
pugixml_test,data_100mb.xml,1,-1,-1
pugixml_test,data_100mb.xml,2,-1,-1
pugixml_test,data_100mb.xml,3,-1,-1
tinyxml2_test,data_100mb.xml,1,-1,-1
tinyxml2_test,data_100mb.xml,2,-1,-1
tinyxml2_test,data_100mb.xml,3,-1,-1
rapidxml_test,data_100mb.xml,1,-1,-1
rapidxml_test,data_100mb.xml,2,-1,-1
rapidxml_test,data_100mb.xml,3,-1,-1
pugixml_test,data_200mb.xml,1,562,739
pugixml_test,data_200mb.xml,2,328,543
pugixml_test,data_200mb.xml,3,342,28085
tinyxml2_test,data_200mb.xml,1,1272,2310
tinyxml2_test,data_200mb.xml,2,1226,2534
tinyxml2_test,data_200mb.xml,3,1237,2398
rapidxml_test,data_200mb.xml,1,954,6954
rapidxml_test,data_200mb.xml,2,909,7107
rapidxml_test,data_200mb.xml,3,926,7198
```
`-1` means the file could not be read.

ADDITIONAL: to see CLI messages from the program in Russian, you may need to switch encoding:
```powershell
chcp 65001
```

## Visualization of results
After running the tests, results.csv will appear in the build/Release folder. You can run the visualizer, which will show the difference in speed between reading and writing for the three libraries.
Run the visualization script:
```powershell
python visualizer.py
```
Important: the script must be run from the root folder, it will go into `build/Release`. If the .py file or .csv table is in another location, it will not work.

# Additional (notes):

## Description of generated and executed tests

### 1. Generating XML files
To prepare the test environment, use the executable **xml_generator.exe** built from `xml_generator.cpp`. Generator principle:
1. When running, two arguments are passed:
   - `<output.xml>` - output XML file name.
   - `<size_mb>` - desired file size in megabytes.
2. The file creates a root element `<root>`.
3. Then, until the target size is reached, `<entry>` records are generated, each containing:
   - Attribute `id` - entry number.
   - Tag `<name>` - random string of length 20.
   - Tag `<value>` - random integer from 0 to 10000.
   - Tag `<description>` - random string of length 100.
4. After adding the required number of entries, the file is closed with `</root>`.
5. The utility outputs progress info to the console (current size in megabytes).

Thus, files of the following sizes are generated (depending on `size_mb`):
- `data_10mb.xml`  - 10 MB.
- `data_50mb.xml`  - 50 MB.
- `data_100mb.xml` - 100 MB.
- `data_200mb.xml` - 200 MB.
- The test file is created only if it does not already exist.

### 2. Running tests for each XML file
The main executable **test_runner.exe** (`test_runner.cpp`) performs the following steps:
1. Processes command line arguments:
   - `-a N` or `--amount N` - number of runs for each test (default is 5 if not specified).
   - `-c <custom_file>` or `--custom <custom_file>` - optional path to your own XML file. If such a file is specified and exists, it is added to the list of test files with size `0` (the test will run but the file will not be generated).
2. Internal list of test files:
   ```cpp
   std::vector<std::pair<std::string,int>> test_files = {
       {"data_10mb.xml", 10},
       {"data_50mb.xml", 50},
       {"data_100mb.xml",100},
       {"data_200mb.xml",200}
   };
   ```
   - If the `-c <custom_file>` parameter is present, this file is added as `(name, 0)`, and generation will not be performed.
3. For each file in the list, the `generate_test_file` function is called, which runs the external command:
   ```
   xml_generator.exe <file> <size_mb>
   ```
   and brings the file size to the required amount. If the file size is 0 (i.e., it is a custom file), generation will not occur, since it was already generated.
4. After preparing all XML files, a list of executables to test is formed:
   ```cpp
   std::vector<std::string> bins = {
       "pugixml_test.exe",
       "tinyxml2_test.exe",
       "rapidxml_test.exe"
   };
   ```
   Each of these programs should contain its own read and write test (they are compiled from libraries in the `ext/` directory and implement time measurement for reading/writing the given XML file).
5. A CSV file of results `results.csv` is formed with the header:
   ```
   Library,File,Run,Read(ms),Write(ms)
   ```
6. For each combination `<file> AND <library>`, `N` runs are performed:
   - The binary is run with the command:
     ```
     <library_exe> <input.xml> <temp.xml>
     ```
   - Inside each test binary (`pugixml_test.exe`, `tinyxml2_test.exe`, `rapidxml_test.exe`), time is measured:
     - Reading, i.e., time to parse the XML file.
     - Time to write results back to a new file `<temporary_output.xml>`.
       - All in milliseconds
   - After each run, `run_test` returns a structure:
     ```cpp
     struct TestResult {
         std::string lib;     // binary name (e.g., "pugixml_test")
         std::string file;    // XML file name (e.g., "data_10mb.xml")
         int run;             // run number (1…N)
         long read_ms;        // read time in ms (or -1 on error)
         long write_ms;       // write time in ms (or -1 on error)
     };
     ```
   - are written to the CSV file `results.csv`.

As a result, we get a table with rows like:
```
pugixml_test,data_10mb.xml,1,18,43
pugixml_test,data_10mb.xml,2,20,34
...
```
where:
- `Library` - test executable name (`pugixml_test`, `tinyxml2_test`, `rapidxml_test`).
- `File`      - processed XML file name (`data_10mb.xml`, etc.).
- `Run`    - run number (from 1 to N).
- `Read(ms)`- time to parse in milliseconds (`-1` on error).
- `Write(ms)` - time to write (`-1` on error).

For each XML file, N measurements are recorded for each library, allowing you to assess stability and average runtime.

---

## Static version of test results

To be able to study the results without running the program, I prepared results_static.csv, which is in the repository. 
Run like this
```python visualizer.py -c results_static.csv```
(important that the file is in the folder with visualizer.py, but if you download it, it will be so), to show visualization for the static file.

This way you get the same graphs based on the pre-generated csv without needing to run the programs.


