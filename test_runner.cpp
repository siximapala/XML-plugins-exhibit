#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <array>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// ��������� ��� �������� ���������� ������ �������
struct TestResult {
    std::string lib;   // ��� ����������
    std::string file;  // ��� XML-�����
    int run;           // ����� �������
    long read_ms;      // ����� ������
    long write_ms;     // ����� ������
};

// ���������� XMl���� ������� �������, ���� �� �� ����������
void generate_test_file(const std::string& filename, int size_mb) {
    std::string cmd = "xml_generator.exe " + filename + " " + std::to_string(size_mb);
    std::cout << "��������� ��������� �����: " << filename << "\n";
    if (std::system(cmd.c_str()) != 0) {
        throw std::runtime_error("�� ������� ������������� ����: " + filename);
    }
}

// ��������� ��������� ���� � ������ ��� �����
TestResult run_test(const std::string& exe,
                    const std::string& input,
                    const std::string& output,
                    int run_num) {
    std::string cmd = exe + " " + input + " " + output;
    std::array<char, 128> buffer;
    std::string result;

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "�� ������� ��������� �������: " << cmd << "\n";
        return {exe, input, run_num, -1, -1};
    }
    // ������ ���� ����� �� �����
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    _pclose(pipe);


    long read = -1, write = -1;
    // ���� ��������� "read=" � "write="
    auto pos_r = result.find("read=");
    auto pos_w = result.find("write=");
    if (pos_r != std::string::npos) {
        read = std::stol(result.substr(pos_r + 5,
            result.find("ms", pos_r) - (pos_r + 5)));
    }
    if (pos_w != std::string::npos) {
        write = std::stol(result.substr(pos_w + 6,
            result.find("ms", pos_w) - (pos_w + 6)));
    }

    return {exe, input, run_num, read, write};
}

int main(int argc, char** argv) {
    int runs = 5;                  // �� ��������� 5 ��������
    std::string custom_file;       // ��� ������������� ���������� XML

    // ������ ��������� ��������� ������
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "-a" || a == "--amount") && i + 1 < argc) {
            runs = std::stoi(argv[++i]);  // ������ ����� ��������
        }
        else if ((a == "-c" || a == "--custom") && i + 1 < argc) {
            custom_file = argv[++i];      // ������ ���� �� ���������� �����
        }
        else {
            std::cerr << "����������� ��������: " << a << "\n";
            return 1;
        }
    }

    // ������ ����������� �������� ������ � �� �������� (��)
    std::vector<std::pair<std::string,int>> test_files = {
        {"data_10mb.xml", 10},
        {"data_50mb.xml", 50},
        {"data_100mb.xml",100},
        {"data_200mb.xml",200}
    };

    // ���� ������ ��������� ����, ��������� ��� � ������
    if (!custom_file.empty()) {
        if (!fs::exists(custom_file)) {
            std::cerr << "Custom-���� �� ������: " << custom_file << "\n";
            return 1;
        }
        // ������ �� ������������� ����� ��������� 0
        test_files.emplace_back(custom_file, 0);
    }

    // ���������� ������������� ����� (����� ����������)
    for (auto& [file, size_mb] : test_files) {
        if (size_mb > 0 && !fs::exists(file)) {
            generate_test_file(file, size_mb);
            auto sz = fs::file_size(file);
            std::cout << "������������ " << file
                      << ", ������ " << (sz/1024/1024) << " MB\n";
        }
    }

    // ������ ����������� ���������� ��� ��������� ������������� ��
    std::vector<std::string> bins = {
        "pugixml_test.exe",
        "tinyxml2_test.exe",
        "rapidxml_test.exe"
    };

    // ��������� CSV ��� ������ �����������
    std::ofstream out("results.csv");
    out << "����������,����,������,������(��),������(��)\n";

    // �������� ���� ��� ������� ����� � ������ ����������
    for (auto& [file, _] : test_files) {
        for (auto& exe : bins) {
            std::string lib = fs::path(exe).stem().string();
            std::cout << "\n=== " << lib << " �� " << file
                      << " (" << runs << " ��������) ===\n";
            // ��������� ���� runs ���
            for (int i = 1; i <= runs; ++i) {
                // ��������� ���� ��� ������
                std::string out_file = "temp_" + lib + "_" + std::to_string(i) + ".xml";
                auto res = run_test(exe, file, out_file, i);
                out << lib << "," << file << "," << res.run << ","
                    << res.read_ms << "," << res.write_ms << "\n";
                out.flush();  // ����� ����� � ����
                std::cout << "[" << lib << "] ������ " << i
                          << ": ������=" << res.read_ms
                          << "�� ������=" << res.write_ms << "��\n";
            }
        }
    }

    std::cout << "\n���������� ��������� � results.csv\n";
    return 0;
}
