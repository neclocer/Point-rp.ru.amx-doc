# Point-rp.ru.amx-doc
# Фреймворк Point RP — отличия от обычного Pawn

Документация для разработчиков, впервые работающих с гейммодом Point RP.

Мод написан на Pawn, но поверх него лежит самописный слой, который меняет почти всё
привычное: как объявляются переменные, как ловятся события, как пишутся команды.
Код, скопированный из обычного SA-MP мода, тут в большинстве случаев не заработает.

Ниже — что именно отличается и как писать правильно.

---

## Содержание

1. [Префиксы модулей — главная особенность](#1-префиксы-модулей--главная-особенность)
2. [Макросы: private, CALLBACK, UNUSED](#2-макросы-private-callback-unused)
3. [Система колбэков](#3-система-колбэков)
4. [Жизненный цикл модуля](#4-жизненный-цикл-модуля)
5. [Команды](#5-команды)
6. [Диалоги](#6-диалоги)
7. [База данных](#7-база-данных)
8. [Правила стиля](#8-правила-стиля)
9. [Частые ошибки](#9-частые-ошибки)
10. [Шаблон модуля](#10-шаблон-модуля)

---

## 1. Префиксы модулей — главная особенность

В обычном Pawn нельзя использовать точку в имени переменной. Здесь это обходится
макросом, который раскрывает точку в префикс.

### Как объявляется

```pawn
#if defined MyModule
    #undef MyModule
#endif
#define MyModule. _mmod

#if defined this
    #undef this
#endif
#define this. _mmod
```

После этого:

| Что пишем | Во что разворачивается |
|---|---|
| `MyModule.count` | `_mmodcount` |
| `this.count` | `_mmodcount` |
| `this.Init()` | `_mmodInit()` |

`MyModule.` и `this.` дают **одно и то же**. Внутри своего модуля пишут `this.`,
снаружи — `MyModule.`.

### Зачем это нужно

Все имена в Pawn глобальные. Префикс гарантирует, что переменная `count` в одном
модуле не столкнётся с `count` в другом. По сути — эмуляция пространств имён.

### Что важно помнить

`#undef this` в начале обязателен — иначе `this.` подхватит префикс предыдущего
подключённого модуля, и код молча начнёт писать в чужие переменные.

Префикс выбирают коротким и уникальным: `_fesc`, `_atools`, `_lchrdata`.

### Пример

```pawn
#define Hunting. _hunt
#define this. _hunt

private Hunting.meatCount[MAX_PLAYERS];   // -> _huntmeatCount

stock Hunting.AddMeat (playerid)
{
    this.meatCount[playerid]++;            // -> _huntmeatCount
}
```

Из другого модуля:

```pawn
Hunting.AddMeat(playerid);
```

---

## 2. Макросы: private, CALLBACK, UNUSED

```pawn
#define CALLBACK%0(%1)      forward%0(%1);public%0(%1)
#define private             stock static
#define UNUSED%0;           if (%0) {}
```

### private

Обычный Pawn:
```pawn
static stock MyFunc() { }
new gVariable;
```

Здесь:
```pawn
private MyModule.MyFunc() { }
private MyModule.variable[MAX_PLAYERS];
```

`private` = `stock static`. Функция или переменная видна только внутри файла.
Для того, что должно быть доступно снаружи — обычный `stock`.

### CALLBACK

Обычный Pawn требует два объявления:
```pawn
forward OnSomething(playerid);
public OnSomething(playerid) { }
```

Здесь одно:
```pawn
CALLBACK MyModule.OnSomething (playerid)
{
}
```

Макрос сам разворачивает это в `forward` + `public`.

### UNUSED

Гасит предупреждение о неиспользуемом параметре:

```pawn
stock MyModule.DoSomething (playerid, time)
{
    UNUSED time;
    ...
}
```

Для массива:
```pawn
stock MyModule.DoSomething (playerid, const text[])
{
    UNUSED text[0];
    ...
}
```

**В `CALLBACK` использовать `UNUSED` не нужно** — только в `stock`.

---

## 3. Система колбэков

Стандартные колбэки SA-MP напрямую **не используются**. Вместо них — своя система
с регистрацией.

### Обычный SA-MP

```pawn
public OnPlayerConnect(playerid)
{
    // код
    return 1;
}
```

Проблема: колбэк один на весь мод, модули дерутся за него через ALS-хуки.

### Здесь

Регистрация в `Init`:
```pawn
Callback.Add(@OnPlayerConnect, #this.OnPlayerConnect);
```

Объявление:
```pawn
CALLBACK MyModule.OnPlayerConnect (playerid)
{
    // код
}
```

Каждый модуль подписывается независимо, конфликтов нет.

### Возвращаемые значения

В большинстве колбэков — просто `return;` без значения.

В части колбэков нужно вернуть `BREAK` или `CONTINUE`:

- `CONTINUE` — событие идёт дальше по цепочке к другим модулям
- `BREAK` — обработано, дальше не передавать

Возврат требуется ровно в этих тринадцати:
```
OnPlayerCmd
OnDialogResponse
OnEnterCPoint, OnLeaveCPoint
OnEnterRaceCPoint, OnLeaveRaceCPoint
OnEnterDynamicArea, OnLeaveDynamicArea
OnPickupUp, OnPickupUpDynamic
OnClickTD, OnClickPlayerTD
OnEditDynamicObject
```

В остальных — голый `return;`, в конце функции ничего.

Не путать с колбэками, имя которых передаётся строкой в чужой API
(например `#this.OnKeyArea` в `KeyArea.Create` или `#this.OnKeyMGameFinish`
в `KeyMGame.Start`) — это обычные функции модуля, там `return;` без значения.

### Передача адреса колбэка в чужой API

Некоторые системы принимают адрес функции:

```pawn
private MyModule.eventEnd;

private MyModule.Init ()
{
    this.eventEnd = Callback.GetAddress(#this.OnSomethingEnd);
}
```

Или строкой:

```pawn
KeyMGame.Start(playerid, "FISHING", #this.OnKeyMGameFinish, 10);
```

### Подписка на события другого модуля

```pawn
Callback.AddListener (const event[], const callback[])
```

```pawn
Callback.AddListener("MMenu.OnBindHeader", #this.OnMMenuBindHeader);
```

Событие ищется по хешу имени, обработчик сохраняется указателем на публичную функцию.
Ограничения — `CBACK_MAX_EVENTS` и `CBACK_MAX_LISTENERS`, при превышении фреймворк
роняет мод через `assert` с сообщением в лог.

### Доступные колбэки

Отличаются от стандартных SA-MP. Полный список:

```
OnPlayerConnect (playerid)
OnPlayerLogin (playerid, bool:isRegistration)
OnPlayerDisconnect (playerid, reason)
OnPlayerSpawn (playerid, skip)
OnPlayerKilled (playerid, killerid, reason)
OnPlayerDeath (playerid)
OnVehicleSpawn (vehicleid)
OnVehicleDeath (vehicleid, killerid)
OnVehicleDestroy (vehicleid)
OnPlayerEnterVehicle (playerid, vehicleid, ispassenger)
OnPlayerExitVehicle (playerid, vehicleid)
OnStateEnterVehicle (playerid, vehicleid, bool:isPassenger)
OnStateExitVehicle (playerid, vehicleid, bool:isPassenger, bool:vehDestroyed)
OnPlayerStateChange (playerid, newstate, oldstate)
OnEnterCPoint (playerid, cpointid)
OnLeaveCPoint (playerid, cpointid)
OnEnterRaceCPoint (playerid, cpointid)
OnLeaveRaceCPoint (playerid, cpointid)
OnPickupUp (playerid, pickupid)
OnInteriorChange (playerid, newinteriorid, oldinteriorid)
OnWorldChange (playerid, worldid, oldWorldid)
OnKeyStateChange (playerid, newkeys, oldkeys)
OnClickTD (playerid, Text:clickedid)
OnClickPlayerTD (playerid, PlayerText:playertextid)
OnVehicleSirenChange (playerid, vehicleid, newstate)
OnPlayerClickPlayer (playerid, clickedplayerid, source)
OnDynamicObjectMoved (STREAMER_TAG_OBJECT:objectid)
OnEditDynamicObject (playerid, STREAMER_TAG_OBJECT:objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
OnPickupUpDynamic (playerid, STREAMER_TAG_PICKUP:pickupid)
OnEnterDynamicArea (playerid, STREAMER_TAG_AREA:areaid)
OnLeaveDynamicArea (playerid, STREAMER_TAG_AREA:areaid)
OnDialogResponse (playerid, dialogid, response, listitem, const inputtext[])
OnDialogShow (playerid, dialogid)
OnPlayerCmd (playerid, const cmd[], const cmdtext[], argidx)
OnPayDay ()
OnAdminLogin (playerid, bool:isRegistration)
OnLevelUp (playerid, oldlvl, newlvl)
OnEnterWater (playerid)
OnLeaveWater (playerid)
OnPlayerClickMap (playerid, Float:x, Float:y, Float:z)
OnCefInitialize (playerid, success)
OnMutePlayer (playerid, bool:muted)
```

Обратите внимание: смерть разделена на два колбэка. `OnPlayerKilled` даёт убийцу,
`OnPlayerDeath` — только жертву.

---

## 4. Жизненный цикл модуля

`OnGameModeInit` напрямую не используется. Вместо него ALS-цепочка `OnModeInit`,
которая дописывается в конец файла:

```pawn
CALLBACK OnModeInit ()
{
    this.Init();

#if defined MyModule@OnModeInit
    MyModule@OnModeInit();
#endif
}

#if defined MyModule@OnModeInit
    forward MyModule@OnModeInit();
#endif
#if defined _ALS_OnModeInit
    #undef OnModeInit
#else
    #define _ALS_OnModeInit
#endif
#define OnModeInit MyModule@OnModeInit
```

Это копируется как есть, меняется только имя модуля. Благодаря цепочке каждый
подключённый модуль получает свою инициализацию, не перебивая остальные.

Сам `Init` — обычная приватная функция:

```pawn
private MyModule.Init ()
{
    this.handle = GeneralDB.InitTable("my_table", GDB_TYPE_ACCOUNT);

    SetTimer(#this.OnTimer1sec, 1000, true);

    Callback.Add(@OnPlayerConnect, #this.OnPlayerConnect);
    Callback.Add(@OnPlayerCmd,     #this.OnPlayerCmd);
}
```

Таймеры регистрируются через `#this.ИмяКолбэка` — макрос `#` превращает это в строку
с уже развёрнутым префиксом.

---

## 5. Команды

`OnPlayerCommandText` не используется. Команды приходят в `OnPlayerCmd` уже разобранными
на имя и аргументы.

```pawn
CALLBACK MyModule.OnPlayerCmd (playerid, const cmd[], const cmdtext[], argidx)
{
    if (CMDCompare(cmd, "/mycmd"))
    {
        new targetid;

        if (!iparam(targetid, cmdtext, ++argidx))
            return SendClientMessage(playerid, -1, " Введите: /mycmd [playerid]");

        if (!IsPlayerLogin(targetid))
            return SendClientMessage(playerid, COLOR_GREY, " Игрок оффлайн / не залогинен");

        // код команды

        return BREAK;
    }
    return CONTINUE;
}
```

### Разбор аргументов

| Функция | Что достаёт |
|---|---|
| `iparam(&value, source, idx)` | число, сам проверяет что это число |
| `wparam(out[], source, idx)` | одно слово — ник, короткий параметр |
| `sparam(out[], source, idx)` | текст до конца строки — причина, сообщение |

`argidx` инкрементируется **перед** каждым параметром: `++argidx`.

Все три возвращают `false`, если аргумента нет — на этом строится вывод подсказки.

### Важно

`CMDCompare` возвращает **true при совпадении** (внутри `strcmp(...) == 0`).
Это противоположно привычному `strcmp`.

После обработанной команды — `return BREAK`. В конце колбэка — `return CONTINUE`.

---

## 6. Диалоги

ID диалогов не хардкодятся. Используется сквозной счётчик `LAST_DIALOGID`:

```pawn
const DIALOG_MY_MENU    = LAST_DIALOGID + 1;
const DIALOG_MY_CONFIRM = LAST_DIALOGID + 2;
#undef LAST_DIALOGID
const LAST_DIALOGID = DIALOG_MY_CONFIRM;
```

Каждый модуль сдвигает счётчик, поэтому ID никогда не пересекаются между модулями.

Обработка:

```pawn
CALLBACK MyModule.OnDialogResponse (playerid, dialogid, response, listitem, const inputtext[])
{
    if (dialogid != DIALOG_MY_MENU)
        return CONTINUE;

    if (!response)
        return BREAK;

    // код

    return BREAK;
}
```

Не забыть зарегистрировать в `Init`:
```pawn
Callback.Add(@OnDialogResponse, #this.OnDialogResponse);
```

### Подводный камень TABLIST

В `DIALOG_STYLE_TABLIST` **первая строка считается заголовком** и не кликается,
а `listitem` отсчитывается от строк под ней. Если нужны все строки кликабельными —
использовать `DIALOG_STYLE_TABLIST_HEADERS` и явно добавить строку-шапку.

---

## 7. База данных

Прямой `db_open` не используется. Физических баз около пяти, они разделены по типам,
а таблицы привязываются к нужной через `GeneralDB`:

```pawn
private DB:MyModule.handle;

private MyModule.Init ()
{
    this.handle = GeneralDB.InitTable("my_table", GDB_TYPE_ACCOUNT);

#if defined GENERAL_DB_CREATE
    db_free_result(db_query(this.handle, "CREATE TABLE IF NOT EXISTS `my_table` (`uuid` TEXT, `value` INTEGER DEFAULT 0)"));
#endif
}
```

Типы: `GDB_TYPE_GENERAL`, `GDB_TYPE_ACCOUNT`, `GDB_TYPE_ADMIN`, `GDB_TYPE_FACTIONS`,
`GDB_TYPE_FAMILIES`, `GDB_TYPE_EVENTS`, `GDB_TYPE_SETTINGS`.

Дальше — **нативные функции SQLite** из `a_samp_db`, ничего самописного:

```pawn
db_query, db_free_result, db_num_rows, db_next_row
db_get_field_assoc_int (result, "колонка")
db_get_field_assoc (result, "колонка", out[], size)
db_get_field_assoc_float (result, "колонка")
```

### Правила

Читать **по имени колонки** (`assoc`), а не по индексу. При добавлении новых колонок
индексы съедут, имена — нет.

Запрос без `SELECT` оборачивать сразу:
```pawn
db_free_result(db_query(this.handle, query));
```

Запрос с `SELECT` — освобождать после чтения:
```pawn
new DBResult:result = db_query(this.handle, query);

while (db_num_rows(result))
{
    // чтение
    db_next_row(result);
}
db_free_result(result);
```

**Забытый `db_free_result` — утечка памяти.** Частая ошибка: освобождают только
в ветке «строк нет», а в основной забывают.

Привязка к игроку — по `PlayerInfo[playerid][pUUID]`.

Загрузка данных вешается на `OnPlayerLogin`, сохранение — на `OnPlayerDisconnect`.

---

## 8. Правила стиля

### Проверки-возвраты блоками

Не так:
```pawn
if (!condition)
    return SendClientMessage(playerid, COLOR_GREY, " Ошибка");
```

А так:
```pawn
if (!condition)
{
    SendClientMessage(playerid, COLOR_GREY, " Ошибка");
    return;
}
```

Исключение — команды в `OnPlayerCmd`, там первый вариант допустим.

### Комментарии

В боевом коде минимальны. Допустимы короткие пометки строчными буквами:

```pawn
// дёргается из производства когда игрок что то крафтит
// заглушка, временно нет данных
```

Секции-разделители вида `/** Init */` не используются.

### Именование

Функции с заглавной: `GetPlayerData`, `SpawnActor`.
Приватные переменные — как удобно, часто camelCase: `alexSpot`, `blockTime`.
Константы модуля с префиксом: `FESC_ORDER_TIME`, `AT_LVL_WHO`.

### Тестовый режим

Код только для тестового сервера:

```pawn
if (!ServerInfo.IsTest())
    return;
```

Для тестовых команд — проверка в начале, затем `return BREAK`.

---

## 9. Частые ошибки

### `[Callback.Add] idx == -1`

Объявили `Callback.Add(@OnX, #this.OnX)`, но не создали `CALLBACK Module.OnX`.
Либо переименовали колбэк и забыли поправить регистрацию.

### Колбэк объявлен, но не вызывается

Забыли `Callback.Add` в `Init`. Само по себе по имени ничего не подхватывается.

### Кириллица превратилась в мусор

Файл сохранён в UTF-8. Пересохранить в CP1251.

### `this.` пишет в чужие переменные

Забыт `#undef this` в шапке модуля.

### `sizeof` на двумерном массиве

`sizeof(array)` для `array[N][M]` вернёт `N` — количество строк, не общий размер.
Для второго измерения: `sizeof(array[])`.

### Приоритет операторов с битовыми масками

```pawn
if (flags & MASK != MASK)      // неверно: != приоритетнее &
if ((flags & MASK) != MASK)    // верно
```

### Self-referencing format

```pawn
format(buf, size, "%s|", buf);   // буфер одновременно приёмник и источник
```
Поведение не гарантировано, на длинных данных режется. Собирать через `strcat`
во временный буфер.

### Индекс, сбрасывающийся во вложенном цикле

Классика при загрузке данных из БД: внешний цикл по группам, внутренний пишет в общий
массив начиная с нуля — группы затирают друг друга. Индекс приёмника объявлять
**снаружи** обоих циклов.

---

## 10. Шаблон модуля

Готовый каркас, от которого можно отталкиваться:

```pawn
/*
 * Module: MyModule
 * Author: YourNick
 *
 * Point Role Play (c) Point-rp.ru 2026
 */

#if defined MyModule
    #undef MyModule
#endif
#define MyModule. _mmod

#if defined this
    #undef this
#endif
#define this. _mmod


#define MYMOD_SOME_TIME                 300


const DIALOG_MYMOD_MENU = LAST_DIALOGID + 1;
#undef LAST_DIALOGID
const LAST_DIALOGID = DIALOG_MYMOD_MENU;


enum MyModule.eData
{
    MyModule.value,
    bool:MyModule.active
}
private MyModule.data[MAX_PLAYERS][MyModule.eData];

private DB:MyModule.handle;


private MyModule.Init ()
{
    this.handle = GeneralDB.InitTable("mymod", GDB_TYPE_ACCOUNT);

#if defined GENERAL_DB_CREATE
    db_free_result(db_query(this.handle, "CREATE TABLE IF NOT EXISTS `mymod` (`uuid` TEXT, `value` INTEGER DEFAULT 0)"));
#endif

    Callback.Add(@OnPlayerCmd,        #this.OnPlayerCmd);
    Callback.Add(@OnPlayerLogin,      #this.OnPlayerLogin);
    Callback.Add(@OnPlayerDisconnect, #this.OnPlayerDisconnect);
}

private MyModule.Clear (playerid)
{
    this.data[playerid][this.value]  = 0;
    this.data[playerid][this.active] = false;
}


stock MyModule.LoadData (playerid)
{
    new query[160];
    format(query, sizeof(query), "SELECT * FROM `mymod` WHERE `uuid` = '%s' LIMIT 1", PlayerInfo[playerid][pUUID]);

    new DBResult:result = db_query(this.handle, query);

    if (!db_num_rows(result))
    {
        db_free_result(result);

        format(query, sizeof(query), "INSERT INTO `mymod` (`uuid`) VALUES ('%s')", PlayerInfo[playerid][pUUID]);
        db_free_result(db_query(this.handle, query));
        return;
    }

    this.data[playerid][this.value] = db_get_field_assoc_int(result, "value");

    db_free_result(result);
}

stock MyModule.SaveData (playerid)
{
    if (!IsPlayerLogin(playerid))
        return;

    new query[160];
    format(query, sizeof(query), "UPDATE `mymod` SET `value` = %d WHERE `uuid` = '%s'",
        this.data[playerid][this.value], PlayerInfo[playerid][pUUID]);

    db_free_result(db_query(this.handle, query));
}


CALLBACK MyModule.OnPlayerCmd (playerid, const cmd[], const cmdtext[], argidx)
{
    if (CMDCompare(cmd, "/mycmd"))
    {
        new targetid;

        if (!iparam(targetid, cmdtext, ++argidx))
            return SendClientMessage(playerid, -1, " Введите: /mycmd [playerid]");

        if (!IsPlayerLogin(targetid))
            return SendClientMessage(playerid, COLOR_GREY, " Игрок оффлайн / не залогинен");

        SendFormatMsg(playerid, COLOR_LIGHTBLUE, " Цель: %s[%d]", PlayerNikName[targetid], targetid);
        return BREAK;
    }
    return CONTINUE;
}

CALLBACK MyModule.OnPlayerLogin (playerid, bool:isRegistration)
{
    this.LoadData(playerid);
}

CALLBACK MyModule.OnPlayerDisconnect (playerid, reason)
{
    this.SaveData(playerid);
    this.Clear(playerid);
}


CALLBACK OnModeInit ()
{
    this.Init();

#if defined MyModule@OnModeInit
    MyModule@OnModeInit();
#endif
}

#if defined MyModule@OnModeInit
    forward MyModule@OnModeInit();
#endif
#if defined _ALS_OnModeInit
    #undef OnModeInit
#else
    #define _ALS_OnModeInit
#endif
#define OnModeInit MyModule@OnModeInit
```

---

## Перед началом работы

Прежде чем писать свою реализацию чего-либо — **спросите, есть ли готовое**.
В моде уже есть подсистемы почти под всё: мини-игры, зоны на клавишу, диалоги NPC,
предложения с подтверждением, чекпоинты, GPS-метки, работа с деньгами, оружием,
фракциями, наказаниями.

Самописная реализация в обход существующего модуля обычно означает, что данные не
попадут в интерфейс, не сохранятся и не будут видны другим системам.


ps. гит будет обновляться новыми файлами 
