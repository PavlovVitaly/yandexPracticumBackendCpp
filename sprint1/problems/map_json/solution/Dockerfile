# Создать образ на основе базового слоя gcc (там будет ОС и сам компилятор).
# 11.3 — используемая версия gcc.
FROM gcc:11.3

# Выполнить установку зависимостей внутри контейнера.
RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
    && \
    pip3 install conan

# Скопировать файлы проекта внутрь контейнера
COPY ./src /app/src
COPY ./data /app/data
COPY CMakeLists.txt conanfile.txt /app/

# новая команда для сборки сервера:
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . 
