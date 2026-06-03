# API Протокол

## Формат
command&payload\n

## Команды
| Команда | Формат | Ответ |
|---------|--------|-------|
| reg | reg&login,password | REG_OK: логин |
| auth | auth&login,password | AUTH_OK: логин (роль) |
| sha256 | sha256&text | SHA256_OK: хэш |
| spline | spline&input.csv,output.csv | SPLINE_OK |
| gd | gd&func&start&lr | GD_OK: результат |
| users | users&sort | USERS_OK: список |
