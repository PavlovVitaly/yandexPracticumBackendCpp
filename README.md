# cpp-backend-template

Шаблон репозитория для практического трека «C++ backend».

# Начало работы

Склонируйте репозиторий в любую подходящую директорию на вашем компьютере.

# Обновление шаблона

Чтобы иметь возможность получать обновления автотестов и других частей шаблона выполните следующую команду:

```
git remote add -m main template https://github.com/cpppracticum/cpp-backend-template-practicum.git
```

Для обновления шаблона выполните команду:

```
git fetch template && git checkout template/main .github
```

Затем добавьте полученные изменения в свой репозиторий.

# Тестирование в Github Actions

При отправке (push) изменений в ветку main репозитория будет запущен пайплайн Github Actions, 

(https://github.com/cpppracticum/cpp-backend-template-practicum/actions),

который позволит увидеть результат прохождения тестов.
