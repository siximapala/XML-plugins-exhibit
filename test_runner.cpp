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

// структура для хранения результата одного прогона
struct TestResult {
    std::string lib;   // имя библиотеки
    std::string file;  // имя XML-файла
    int run;           // номер прогона
    long read_ms;      // время чтения
    long write_ms;     // время записи
};

// генерируем XMlфайл нужного размера, если он не существует
void generate_test_file(const std::string& filename, int size_mb) {
    std::string cmd = "xml_generator.exe " + filename + " " + std::to_string(size_mb);
    std::cout << "Генерация тестового файла: " << filename << "\n";
    if (std::system(cmd.c_str()) != 0) {
        throw std::runtime_error("Не удалось сгенерировать файл: " + filename);
    }
}

// запускаем отдельный тест и парсим его вывод
TestResult run_test(const std::string& exe,
                    const std::string& input,
                    const std::string& output,
                    int run_num) {
    std::string cmd = exe + " " + input + " " + output;
    std::array<char, 128> buffer;
    std::string result;

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Не удалось выполнить команду: " << cmd << "\n";
        return {exe, input, run_num, -1, -1};
    }
    // читаем весь вывод из теста
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    _pclose(pipe);


    long read = -1, write = -1;
    // ищем подстроки "read=" и "write="
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
    int runs = 5;                  // по умолчанию 5 прогонов
    std::string custom_file;       // для опционального кастомного XML

    // Парсим аргументы командной строки
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "-a" || a == "--amount") && i + 1 < argc) {
            runs = std::stoi(argv[++i]);  // читаем число прогонов
        }
        else if ((a == "-c" || a == "--custom") && i + 1 < argc) {
            custom_file = argv[++i];      // читаем путь до кастомного файла
        }
        else {
            std::cerr << "Неизвестный аргумент: " << a << "\n";
            return 1;
        }
    }

    // список стандартных тестовых файлов и их размеров (МБ)
    std::vector<std::pair<std::string,int>> test_files = {
        {"data_10mb.xml", 10},
        {"data_50mb.xml", 50},
        {"data_100mb.xml",100},
        {"data_200mb.xml",200}
    };

    // если указан кастомный файл, добавляем его в список
    if (!custom_file.empty()) {
        if (!fs::exists(custom_file)) {
            std::cerr << "Custom-файл не найден: " << custom_file << "\n";
            return 1;
        }
        // размер не генерируемого файла оставляем 0
        test_files.emplace_back(custom_file, 0);
    }

    // генерируем отсутствующие файлы (кроме кастомного)
    for (auto& [file, size_mb] : test_files) {
        if (size_mb > 0 && !fs::exists(file)) {
            generate_test_file(file, size_mb);
            auto sz = fs::file_size(file);
            std::cout << "Сгенерирован " << file
                      << ", размер " << (sz/1024/1024) << " MB\n";
        }
    }

    // список исполняемых бинарников для тогочтобы оттестировать их
    std::vector<std::string> bins = {
        "pugixml_test.exe",
        "tinyxml2_test.exe",
        "rapidxml_test.exe"
    };

    // открываем CSV для записи результатов
    std::ofstream out("results.csv");
    out << "Библиотека,Файл,Запуск,Чтение(мс),Запись(мс)\n";

    // основной цикл для каждого файла и каждой библиотеки
    for (auto& [file, _] : test_files) {
        for (auto& exe : bins) {
            std::string lib = fs::path(exe).stem().string();
            std::cout << "\n=== " << lib << " на " << file
                      << " (" << runs << " прогонов) ===\n";
            // Запускаем тест runs раз
            for (int i = 1; i <= runs; ++i) {
                // временный файл для вывода
                std::string out_file = "temp_" + lib + "_" + std::to_string(i) + ".xml";
                auto res = run_test(exe, file, out_file, i);
                out << lib << "," << file << "," << res.run << ","
                    << res.read_ms << "," << res.write_ms << "\n";
                out.flush();  // сразу пишет в файл
                std::cout << "[" << lib << "] запуск " << i
                          << ": чтение=" << res.read_ms
                          << "мс запись=" << res.write_ms << "мс\n";
            }
        }
    }

    std::cout << "\nРезультаты сохранены в results.csv\n";
    return 0;
}
