# Андреев Артем 251-351 - Тема №17

**Triple DES | SHA-256 | Сплайны | Градиентный спуск**

---

## 🚀 Запуск

### Локально

```bash
git clone https://github.com/AndreevArtem251/andreev-artem-251-351.git
cd andreev-artem-251-351
mkdir build && cd build
cmake .. && make -j4

# Терминал 1
./server

# Терминал 2
./client
```

## 📡 Команды

| Команда | Пример |
|---------|--------|
| `reg` | `reg&alice,12345` |
| `auth` | `auth&alice,12345` |
| `sha256` | `sha256&hello` |
| `spline` | `spline&in.csv,out.csv` |
| `gd` | `gd&0&5&0.01` |
| `users` | `users&login` |

---

## 📁 Структура

```
server/   - TCP сервер
client/   - сетевой клиент
crypto/   - Triple DES, SHA-256
math/     - сплайны, градиентный спуск
```

---


