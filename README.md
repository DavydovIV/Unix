# Unix


Laboratory work for UNIX

Group: 63607/1

Student: Davydov Ivan

Teacher: Glazunov Vadim

Lab 1. MyDisk

Нужно написать демон под линукс который бы читал конфигурационный файл, брал оттуда параметры: каталог, время проверки. После чего проверял заданий каталог(рекурсивно), через заданные промежутки времени, на предмет наличия/отсутствия модификации файлов и записывал результаты в файл журнала, предусмотреть обработку сигналов: SIGHUP - для перечитывания конфигурационного файла, и SIGTERM - для контролируемого завершения демона (запись о выходе в файл журнала).
Lab 2. MyWebServer

Написать веб-сервер работающий в виде демона, веб-сервер должен обеспечивать базовую поддержку протокола HTTP и отдавать статический контент, обязательна реализация метода GET, по желанию методы HEAD и POST. Предусмотреть контроль и журналирование ошибок (либо в файл либо через syslog). Обеспечить одновременную работу сервера с множественным числом клиентов.
Lab 3. MyChat

Написать сетевой чат, сервер должен быть реализован ввиде демона, предусмотреть контроль и журналирование ошибок, либо через syslog, либо в файл журнала. Сервер должен обеспечивать прием тестовых сообщений и дальнейшую пересылку их всем участникам. Реализовать сетевой клиент для проверки работоспособности сервера.
Lab 4. MyFsReader

Каждому студенту выдается файл, содержащий образ файловой системы. Задачей студента является написать программу на языке C, читающую файловую систему из образа и предоставляющую пользователю интерфейс для выполнения следующих действий над файловой системой: 1. Вывести информацию о файловой системе из суперблока. 2. Вывести содержимое корневого каталога. 3. По требованию пользователя вывести содержимое таблицы FAT. 4. Поддержка добавление файлов и каталогов с жесткого диска в образ файловой системы. При добавлении требуется проверять наличие свободных блоков. 5. По требованию пользователя вывести дерево всей файловой системы включая атрибуты файлов и каталогов. 6. По требованию пользователя сохранить содержимое выбранного файла из образа файловой системы на диск. Путь к файлу задается как абсолютный путь относительно корневой директории. 7. По требованию пользователя вывести содержимое текстовых файлов(расширение .txt) из образа файловой системы на экран.
