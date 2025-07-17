
# Работа с RapidXML на C++

**RapidXML** - это очень быстрая и компактная XML-библиотека на C++, предназначенная для парсинга XML в DOM-структуру. Она не поддерживает потоковую загрузку и работу с большими файлами без предварительной буферизации, но обеспечивает максимальную производительность при работе в памяти.

## Подключение

```cpp
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>
```
RapidXML - это заголовочная библиотека. Т.е. в проект нужно  будет добавлять заголовочные файлы

## Основные особенности
- Парсинг из строки или из std::vector<char>

- Высокая скорость

- Отсутствие зависимостей

- Поддержка DOM API (узлы, атрибуты и текст)

Очень важно: библиотека RapidXML требует, чтобы буфер с XML-данными **оставался валидным во время использования**, т.к. RapidXML не копирует данные.

## Чтение и парсинг XML
```cpp
rapidxml::file<char> xmlFile("input.xml"); // Чтение файла и создание буфера
rapidxml::xml_document<> doc;
doc.parse<0>(xmlFile.data());
```
- file\<char> - вспомогательный класс для загрузки файла в буфер.
- parse<0>(...) - парсинг XML в DOM. Могут использоваться флаги (например, parse_no_data_nodes).

### Навигация по узлам
```cpp
auto* root = doc.first_node("root");
for (auto* entry = root->first_node("entry"); entry; entry = entry->next_sibling("entry")) {
    const char* name = entry->first_node("name")->value();
    int value = std::atoi(entry->first_node("value")->value());
}
```
- first_node("tag") - получить первый дочерний элемент по имени.
- next_sibling("tag") - переход к следующему элементу на том же уровне.
- value() - получить текстовое содержимое узла.

### Создание и изменение XML
```cpp
using namespace rapidxml;

xml_document<> doc;

xml_node<>* root = doc.allocate_node(node_element, "root");
doc.append_node(root);

xml_node<>* entry = doc.allocate_node(node_element, "entry");
entry->append_attribute(doc.allocate_attribute("id", "123"));

entry->append_node(doc.allocate_node(node_element, "name", "example"));
entry->append_node(doc.allocate_node(node_element, "value", "42"));

root->append_node(entry);
```
- Все строки (имена, значения) должны быть через allocate_string(...) или строковые литералы.

- Используется allocate_node(...) и allocate_attribute(...).

## Сохранение XML
```cpp
std::ofstream out("output.xml");
out << doc;
out.close();
```
Также можно сохранить в строку:

```cpp
std::string result;
rapidxml::print(std::back_inserter(result), doc, 0);
```
## Полезные методы
1. node->name()	Получить имя узла
2. node->value()	Получить текстовое содержимое
3. node->first_node("tag")	Получить первый дочерний элемент
4. node->next_sibling("tag")	Перейти к следующему одноуровневому элементу
5. node->append_node(...)	Добавить дочерний узел
6. node->append_attribute(...)	Добавить атрибут
7. node->first_attribute("name")->value()	Получить значение атрибута
8. doc.clear()	Очистить документ

