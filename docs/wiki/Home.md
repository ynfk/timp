# Проект по дисциплине "Технологии и методы программирования"

## Участники
**Андреев Артем Андреевич** (учебная группа 251-351)

## Документация к проекту

### Тема №17: Triple DES, SHA-256, интерполяция сплайнами, градиентный спуск

### Алгоритмы и методы:
- **Triple DES** (шифрование паролей)
- **SHA-256** (хэширование)
- **Метод сплайнов** (интерполяция)
- **Градиентный спуск** (оптимизация)
- **TCP сервер** (многопользовательский режим)

---

## Структура GIT

![Структура веток](https://raw.githubusercontent.com/AndreevArtem251/andreev-artem-251-351/main/docs/diagrams/git_structure.png)

### Основные ветки:

| Ветка | Назначение |
|-------|------------|
| `main` | Стабильный релиз |
| `develop` | Основная разработка |
| `feature/server-core` | Сервер: TCP, парсинг, БД |
| `feature/crypto` | Triple DES, SHA-256 |
| `feature/math` | Сплайны, градиентный спуск |
| `feature/client-gui` | Клиент: GUI, NetworkClient |
| `feature/docker` | Docker контейнеризация |
| `feature/docs` | Документация |
| `feature/testing` | Тестирование |
| `release/v1.0` | Релизная ветка |

---

## UML диаграмма классов для сервера

![Диаграмма классов сервера](https://raw.githubusercontent.com/AndreevArtem251/andreev-artem-251-351/main/docs/diagrams/class_diagram_server.png)

## UML диаграмма классов для клиента

![Диаграмма классов клиента](https://raw.githubusercontent.com/AndreevArtem251/andreev-artem-251-351/main/docs/diagrams/class_diagram_client.png)

## Use-case диаграмма

![UseCase](https://raw.githubusercontent.com/AndreevArtem251/andreev-artem-251-351/main/docs/diagrams/usecase.png)

---

## Прогресс в работе

| Задача | Статус |
|--------|--------|
| Сервер: парсинг запросов + заглушки | ✅ Готово |
| Сервер: БД в синглтоне | ✅ Готово |
| Сервер: несколько клиентов | ✅ Готово |
| Сервер: auth/reg работают | ✅ Готово |
| Клиент: синглтон | ✅ Готово |
| Клиент: GUI интерфейс | ✅ Готово |
| Triple DES | ✅ Готово |
| SHA-256 | ✅ Готово |
| Интерполяция сплайнами | ✅ Готово |
| Градиентный спуск | ✅ Готово |
| Docker контейнеризация | ✅ Готово |
| Документация Doxygen | ✅ Готово |
| Unit тесты | ✅ Готово |