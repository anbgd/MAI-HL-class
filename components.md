# Компонентная архитектура
<!-- Состав и взаимосвязи компонентов системы между собой и внешними системами с указанием протоколов, ключевые технологии, используемые для реализации компонентов.
Диаграмма контейнеров C4 и текстовое описание. 
-->
## Компонентная диаграмма

```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

AddElementTag("microService", $shape=EightSidedShape(), $bgColor="CornflowerBlue", $fontColor="white", $legendText="microservice")
AddElementTag("storage", $shape=RoundedBoxShape(), $bgColor="lightSkyBlue", $fontColor="white")

Person(user, "Пользователь")

System_Ext(web_site, "Клиентский веб-сайт", "HTML, CSS, JavaScript, React", "Веб-интерфейс")

System_Boundary(conference_site, "Социальная сеть") {
   Container(authorization_service, "Сервис авторизации", "Python", "Сервис аутентификации пользователей и управления доступом к системе", $tags = "microService")    
   Container(wall_service, "Сервис стен", "Python", "Сервис загрузки записей на стену пользователей", $tags = "microService") 
   Container(message_service, "Сервис сообщений", "Python", "Сервис управления сообщениями", $tags = "microService")  
   ContainerDb(db, "База данных", "MySQL", "Хранение данных о сообщениях, постах и пользователях", $tags = "storage")
   
}


Rel(user, web_site, "Просмотр, добавление и редактирование информации о пользователях, постах и комментариях")

Rel(web_site, authorization_service, "Работа с пользователями", "localhost/auth")
Rel(authorization_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, wall_service, "Работа со стенами", "localhost/wall")
Rel(wall_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, message_service, "Работа с сообщениями", "localhost/massages")
Rel(message_service, db, "INSERT/SELECT/UPDATE", "SQL")


@enduml
```
## Список компонентов  

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

### Сервис постов
**API**:
- Добавление записи на стену
  - Входные параметры: заголовок записи, пользователь, содержани записи, дата создания
  - Выходные параметры: идентификатор записи
- Загрузка стены пользователя
  - Входнае параметры: идентификатор записи
  - Выходные парамтеры: заголовок записи, пользователь, содержани поста, дата создания, комментарии
- Изменение поста
  - Входные параметры: идентификатор поста, заголовок поста, автор, содержание поста, дата создания, комментарии
  - Выходные параметры: отсутствуют
- Просмотр комментариев поста
  - Входные параметры: идентификатор поста
  - Выходные параметры: комментарии поста
- Добавление комментариев к посту
  - Входные параметры: идентификатор поста
  - Выходные параметры: новый комментарий поста

### Сервис пользователей
**API**:
- Отправка сообщения пользователю
  - Входные параметры: ID отправителя, ID получателя
  - Выходные параметры: отсутствуют

- Получение списка сообщений для пользователя
  - Входные параметры: ID пользователя
  - Выходные параметры: массив [отправитель, получатель, дата создания сообщения]
  
- Управление профилем пользователя
  - Входные параметры: ID пользователя
  - Выходные параметры: массив [фамилия, имя, почта, дата рождения]