
  

# Работа с pugixml на C++

  

**Pugixml** - это лёгкая, быстрая и удобная C++ библиотека для работы с XML-документами. Она поддерживает парсинг, модификацию и сохранение XML в виде DOM-дерева и не требует сторонних зависимостей.

  

## Подключение

  

```cpp

#include  <pugixml.hpp>

```

  

Также нужно, чтобы файл `pugixml.cpp` добавлен в проект (можно через gitmodules, например).

  

## Основные возможности

  

- Загрузка и парсинг XML-файлов

- Навигация по узлам (элементы, атрибуты и текст)

- Изменение и создание новых XML-структур

- Сохранение документа в файл или строку

  

----------

  

## Чтение и парсинг XML

  

```cpp

pugi::xml_document  doc;
pugi::xml_parse_result result = doc.load_file("input.xml");

```

  

-  `xml_document` - корневой объект.

-  `load_file(...)` - загружает и парсит XML-файл.

- Полученный xml можно проверить и отобразить в данных (`result.description()`).

  

Также можно загружать из строки или буфера:

  

```

doc.load_string("<root><child /></root>");

```

  

----------

  

## Навигация по узлам

  

```cpp

pugi::xml_node  root  =  doc.child("root");
for (pugi::xml_node  child : root.children("entry"))
{
std::string  name  =  child.child("name").text().get();
int  value  =  child.child("value").text().as_int();
}

```

  

-  `child("name")` - доступ к вложенному узлу.

-  `children("entry")` - итерация по всем тегам `<entry>`.

-  `.text().get()` - получить текст из тега.

-  `.as_int()`, `.as_double()` и т.п. - преобразование типов.

  

----------

  

## Создание и изменение XML

  

```cpp

pugi::xml_node  root  =  doc.append_child("root");
pugi::xml_node  entry  =  root.append_child("entry");
entry.append_attribute("id") =  123;
entry.append_child("name").text() =  "example";
entry.append_child("value").text() =  42;

```

-  `append_child(...)` - добавляет элемент.

-  `append_attribute(...)` - добавляет атрибут.

-  `.text() = ...` - задаёт текстовое содержимое узла.

  

----------

  

## Сохранение XML

  

```cpp

doc.save_file("output.xml");

```

  

Также можно сохранить в строку или стрим:

  

```cpp

std::ostringstream  oss;
doc.save(oss);
std::string  xml_str  =  oss.str();

```

  

----------

  

## Полезные методы

1. `node.name()` Получить имя узла

2. `node.attributes()` Итерация по всем атрибутам

3.  `node.remove_child("tag")` Удалить дочерний узел по имени

4. `node.remove_attribute("attr")` Удалить атрибут по имени

5. `doc.reset()` Очистить весь документ

6. `node.set_name("newname")` Изменить имя узла

7. `node.prepend_child("tag")` Добавить узел в начало списка дочерних элементов

8. `node.insert_child_before(...)` Вставить узел перед другим
