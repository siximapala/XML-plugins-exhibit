# Работа с tinyxml2

**tinyxml2** - минималистичная библиотека на C++ для чтения, записи и обработки XML-документов.

## Подключение

```cpp
#include <tinyxml2.h>
```
(Также необходимо подключить реализацию библиотеки: tinyxml2.cpp)


## Основные возможности
- Загрузка XML из файла, строки или потока

- Доступ и навигация по узлам и атрибутам

- Создание и изменение структуры XML

- Сохранение в файл или строку

- Простота и скорость

### Чтение и парсинг XML
```cpp
tinyxml2::XMLDocument doc;
tinyxml2::XMLError result = doc.LoadFile("input.xml");
```
1. XMLDocument - объект документа.

2. LoadFile(...) -это загрузка и парсинг XML-файла.

3. result == tinyxml2::XML_SUCCESS - успешная загрузка.

В случае ошибки можно вызвать doc.ErrorStr() и узнать в чем причина.

Также. можно загружать из строки:

```cpp
doc.Parse("<root><child /></root>");
```
### Навигация по узлам
```cpp
tinyxml2::XMLElement* root = doc.FirstChildElement("root");
for (tinyxml2::XMLElement* entry = root->FirstChildElement("entry"); entry;entry = entry->NextSiblingElement("entry")) {
    const char* name = entry->FirstChildElement("name")->GetText();
    int value = entry->FirstChildElement("value")->IntText();
}
```
FirstChildElement("tag") - первый дочерний элемент по имени.

NextSiblingElement("tag") - переход к следующему одноуровневому элементу.

GetText() - получить текст тега.

IntText(), FloatText() - получение значений с преобразованием типов.

### Создание и изменение XML
```cpp
tinyxml2::XMLDocument doc;

tinyxml2::XMLElement* root = doc.NewElement("root");
doc.InsertFirstChild(root);

tinyxml2::XMLElement* entry = doc.NewElement("entry");
entry->SetAttribute("id", 123);

tinyxml2::XMLElement* name = doc.NewElement("name");
name->SetText("example");

tinyxml2::XMLElement* value = doc.NewElement("value");
value->SetText(42);

entry->InsertEndChild(name);
entry->InsertEndChild(value);
root->InsertEndChild(entry);
```
NewElement(...) - создать элемент.

SetAttribute(...) - установить атрибут.

SetText(...) - задать текст.

InsertFirstChild(...), InsertEndChild(...) - вставка в дерево.

### Сохранение XML
```cpp
doc.SaveFile("output.xml");
```

Также можно сохранить в строку:

```cpp
tinyxml2::XMLPrinter printer;
doc.Print(&printer);
std::string xmlStr = printer.CStr();
```

## Полезные методы
1. `element->Name()` Получить имя элемента

2. `element->Attribute("attr")` Получить значение атрибута как строку

3. `element->SetAttribute("attr", val)` Установить или изменить атрибут

4. `element->DeleteChild(child)` Удалить дочерний элемент

5. `element->DeleteAttribute("attr")` Удалить атрибут

6. `element->FirstChildElement()` Получить первый дочерний элемент

7. `element->NextSiblingElement()` Перейти к следующему одноуровневому элементу

8. `element->GetText()` Получить текстовое содержимое

9. `element->SetText("value")` Установить текстовое содержимое

10. `doc.Clear()` Очистить документ
