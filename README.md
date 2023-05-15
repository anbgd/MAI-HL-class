# MAI-HL-class

### Сравнение скорости работы (30 секунд): 

| Количество потоков | Количество соединений | cache | no_cache |
|--------------------|-----------------------|-------|----------|
| 1                  | 1                     | 11216 | 2158     |
| 1                  | 10                    | 12251 | 2846     |
| 1                  | 100                   | 13059 | 3461     |
| 5                  | 10                    | 14470 | 4208     |
| 5                  | 100                   | 14632 | 4287     |
| 10                 | 10                    | 15802 | 4969     |
| 10                 | 100                   | 16409 | 5155     |

## Список компонентов  

### Сервис авторизации
**API**:
-	Создание нового пользователя
      - входные параметры: login, пароль, имя, фамилия, email, дата рождения
      - выходные параметры: отсутствуют
-	Поиск пользователя по логину
     - входные параметры:  login
     - выходные параметры: имя, фамилия, email, дата рождения
-	Поиск пользователя по маске имени и фамилии
     - входные параметры: маска фамилии, маска имени
     - выходные параметры: login, имя, фамилия, email, дата рождения
- Управление профилем пользователя
  - Входные параметры: ID пользователя
  - Выходные параметры: массив [фамилия, имя, почта, дата рождения]

### Сервис стен
**API**:
- Добавление записи на стену
  - Входные параметры: заголовок записи, пользователь, содержани записи, дата создания
  - Выходные параметры: идентификатор записи
- Загрузка стены пользователя
  - Входнае параметры: идентификатор пользователя
  - Выходные парамтеры: заголовок записи, пользователь, содержани поста, дата создания, комментарии
- Изменение поста
  - Входные параметры: идентификатор поста, заголовок поста, автор, содержание поста, дата создания, комментарии
  - Выходные параметры: отсутствуют
- Просмотр комментариев поста
  - Входные параметры: идентификатор поста
  - Выходные параметры: комментарии поста
- Добавление комментариев к посту
  - Входные параметры: идентификатор поста, комментарий поста
  - Выходные параметры: отсутствуют

### Сервис пользователей
**API**:
- Отправка сообщения пользователю
  - Входные параметры: ID отправителя, ID получателя, текст сообщения
  - Выходные параметры: отсутствуют

- Получение списка сообщений для пользователя
  - Входные параметры: ID пользователя
  - Выходные параметры: массив [отправитель, получатель, дата создания сообщения, текст сообщения]