# Инициализация conan и cmake
```sh
mkdir build
cd build
conan install ..
cmake ..
```
# Сборка
В папке `build` выполнить команду
```sh
cmake --build .
```
# Запуск
В папке `build` выполнить команду
```sh
bin/game_server ../data/config.json ../static/
```
После этого можно открыть в браузере:
* http://127.0.0.1:8080/api/v1/maps для получения списка карт и
* http://127.0.0.1:8080/api/v1/map/map1 для получения подробной информации о карте `map1`
* http://127.0.0.1:8080/ для чтения статического контента (в каталоге static)
