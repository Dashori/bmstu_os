## Мой любимый и многострадальный open()...

**ВИЗУАЛЬНО ВСЕ ЛУЧШЕ ПЕРЕДЕЛЫВАТЬ!**

Из моих приколов:
- нельзя на одной странице делать кружочки А, Б, ибо они обычно используются в случае, если все не влезает на одну страницу;
- иногда нужно явно подписывать что возвращается.

Со следующим у меня все ок, но на всякий случай:
- общую схему open() делать единую;
- O_PATH и O_TMPFILE это флаги open(), возможно это надо будет явно сказать;
- флаг O_LARGEFILE устанавливается макросомом _LARGEFILE64_SOURCE;
- в alloc_fd() обязательно добавить расширение таблицы;
- в do_filp_open() проверки ECHILD и ESTALE идут последовательно;
- в lookup_open() добавить проверку O_EXCL;
- подписать явно что такое LOOKUP_RCU и LOOKUP_REVAL;
- кажется, что у меня не хватает какой-то схемы, которая при этом есть в файле drawio...


Ваш любимый сайт на это время (помимо drawio): https://elixir.bootlin.com/linux/v5.15.32/source/fs/open.c