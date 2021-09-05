
&НаКлиенте
Перем МассивЛогСтрок;

&НаКлиенте
Процедура СтартоватьСервис(Команда)
#Если НЕ ВебКлиент Тогда		
	Если ВебСокетКлиент <> Неопределено ТОгда  
		Если ВебСокетКлиент.Запущен() Тогда
			ПредупреждениеАсинх("Сервис уже запущен!");
			Возврат;
		Иначе
			WebSocketКлиент.ОткрытьВебСокет();	   
		КонецЕсли;
	Иначе
		ПредупреждениеАсинх("Драйвер не загружен!");
	КонецЕсли;
#КонецЕсли
КонецПроцедуры

&НаКлиенте
Процедура ОчиститьКонсоль(Команда)
	Консоль = "";
КонецПроцедуры

&НаКлиенте
Процедура РезультатСменыПользователя(Результат, ДополнительныеПараметры) Экспорт
	Если Результат <> Неопределено И ВебСокетКлиент <> Неопределено И ВебСокетКлиент.Запущен() Тогда    
		WebSocketКлиент.СтартоватьСервис(Истина, Результат.Имя, Результат.Пароль, Результат.Идентификатор);
	КонецЕсли;
КонецПроцедуры


&НаКлиенте
Процедура СменитьПользователя(Команда)
	
	ПараметрыФормы = Новый Структура();
	ПараметрыФормы.Вставить("ПодтверждениеПароляНеТребуется", Истина);
	Если ВебСокетКлиент <> Неопределено Тогда
		ПараметрыФормы.Вставить("Имя", ВебСокетКлиент.ТекущееИмя());
	КонецЕсли;
	
	ОписаниеОповещения = Новый ОписаниеОповещения("РезультатСменыПользователя", ЭтотОбъект, ПараметрыФормы);

	ОткрытьФорму("Обработка.WebSocketService.Форма.РегистрацияПользователя", ПараметрыФормы,,,,,ОписаниеОповещения);	

КонецПроцедуры

&НаКлиенте
Процедура ОбновитьСписокАктивныхПользователей(Команда)
	WebSocketКлиент.ПолучитьСписокАктивныхПодключений(УникальныйИдентификатор);
КонецПроцедуры


&НаКлиенте
Процедура ОстановитьСервис(Команда)
	Если ВебСокетКлиент <> Неопределено ТОгда  
		Если ВебСокетКлиент.Запущен() Тогда
             ВебСокетКлиент.Закрыть();
		Иначе
			#Если НЕ ВебКлиент Тогда
			ПредупреждениеАсинх("Сервис не запущен!"); 
			#КонецЕсли  
		КонецЕсли;
	КонецЕсли;
КонецПроцедуры

&НаКлиенте
Процедура ОтправитьСообщение(Команда)
	Если Элементы.АктивныеПользователи.ТекущаяСтрока <> Неопределено Тогда
		ПараметрыФормы = Новый Структура();
		ТекущаяСтрока = АктивныеПользователи.НайтиПоИдентификатору(Элементы.АктивныеПользователи.ТекущаяСтрока);
		ПараметрыФормы.Вставить("ПолучательИмя", ТекущаяСтрока.name);
		ПараметрыФормы.Вставить("ИдентфикаторСессииПолучателя", ТекущаяСтрока.uuid);
		ПараметрыФормы.Вставить("ПолучательИдентификатор", ТекущаяСтрока.user_uuid);
		ОткрытьФорму("Обработка.WebSocketService.Форма.ФормаЧат", ПараметрыФормы);   
	Иначе
		#Если НЕ ВебКлиент Тогда
		ПредупреждениеАсинх("Выберете пользователя!");
		#КонецЕсли
	КонецЕсли;
КонецПроцедуры

&НаКлиенте
Процедура ПользователиВБазеПередНачаломДобавления(Элемент, Отказ, Копирование, Родитель, ЭтоГруппа, Параметр)
		
	Отказ = Истина;
	Результат = ПолучитьФорму("РегистрацияПользователя").ОткрытьМодально(); 
	Если Результат <> Неопределено Тогда
		//ВебСокетКлиент.ДобавитьПользователя(Результат.Имя, Результат.Пароль, Результат.Представление, Результат.Идентификатор, Результат.Роль);
		СтруктураПараметры = Новый Структура;
		СтруктураПараметры.Вставить("name", Результат.Имя);
		СтруктураПараметры.Вставить("pwd", Результат.Пароль);
		СтруктураПараметры.Вставить("role", Результат.Роль);
		СтруктураПараметры.Вставить("uuid", Результат.Идентификатор); 
		СтруктураПараметры.Вставить("pref", Результат.Представление);
	
		Параметры = РаботаСJSON.ПолучитьТекст(СтруктураПараметры);
		
		Если ВебСокетКлиент <> Неопределено Тогда
			ВебСокетКлиент.КомандаСерверу("add_user", Строка(УникальныйИдентификатор), Параметры);
		КонецЕсли;
	КонецЕсли;

КонецПроцедуры

&НаКлиенте
Процедура ВывестиЛог(Данные)

	МассивЛогСтрок.Вставить(0, Символы.ПС);	
	МассивЛогСтрок.Вставить(0, ВывестиЛогСервер(Данные));
	МассивЛогСтрок.Вставить(0, ПолучитьЗаголовок());
	
	Элементы.Консоль.Заголовок = Новый ФорматированнаяСтрока(МассивЛогСтрок);

КонецПроцедуры

&НаСервере
Функция ВывестиЛогСервер(Данные)
	Возврат Новый ФорматированнаяСтрока(Данные,, ЦветаСтиля.ЦветТекстаВыделенияПоля);
КонецФункции

&НаСервере
Функция ПолучитьЗаголовок()
	Возврат Новый ФорматированнаяСтрока("ws_client: ", Новый Шрифт(Элементы.Консоль.Шрифт,,, Истина), ЦветаСтиля.ЦветАктивности);
КонецФункции

&НаКлиенте
Асинх Процедура ОтправитьСообщениеВсем(Команда)
#Если НЕ ВебКлиент Тогда
	Если ВебСокетКлиент = Неопределено ИЛИ НЕ ВебСокетКлиент.Запущен() ТОгда
		
		ПредупреждениеАсинх("Сервис не запущен!");
		
	КонецЕсли;
	ТекстСообщения = "";
	Результат = Ждать ВвестиСтрокуАсинх(ТекстСообщения, "Введите сообщение", 0, Истина);                                                  
	//Сообщить
	// -- ТекстСообщения
	// -- ИдентификаторПолучателя (Пустой uuid -> Сообщить всем)
	// -- ИдентификаторФормы
     ВебСокетКлиент.Сообщить(ТекстСообщения, РаботаСJSON.ПустаяСсылка(), Строка(УникальныйИдентификатор));
#КонецЕсли
КонецПроцедуры

&НаКлиенте
Процедура ОбновитьСписокПользователей(Команда)
	Если ВебСокетКлиент <> Неопределено И ВебСокетКлиент.Запущен() Тогда
		ВебСокетКлиент.КомандаСерверу("get_users", Строка(УникальныйИдентификатор), "");
	КонецЕсли;	
КонецПроцедуры
&НаКлиенте
Процедура ПользователиВБазеВыбор(Элемент, ВыбраннаяСтрока, Поле, СтандартнаяОбработка)
		
	СтандартнаяОбработка = Ложь;
	ИзменитьПользователя(ВыбраннаяСтрока);

КонецПроцедуры

&НаКлиенте
Процедура ПользователиВБазеПередНачаломИзменения(Элемент, Отказ)
		Отказ = Истина;
	
	ИзменитьПользователя(Элементы.ПользователиВБазе.ТекущаяСтрока);

КонецПроцедуры

&НаКлиенте
Процедура ПриОткрытии(Отказ)

	МассивЛогСтрок = Новый Массив();
		
	Если ВебСокетКлиент = Неопределено Тогда
		Подключить();
	Иначе
		Если НЕ ВебСокетКлиент.Запущен() Тогда
			Подключить();
		КонецЕсли;
	КонецЕсли;   
	
    ПроверкаАктивностиКлиента();
	
	ПодключитьОбработчикОжидания("ПроверкаАктивностиКлиента", 10, Ложь);
	
	ЗагрузитьСписокАктивныхПользователей();
	ОбновитьСписокПользователей(0);

КонецПроцедуры

&НаКлиенте
Процедура ЗагрузитьСписокАктивныхПользователей()
	WebSocketКлиент.ПолучитьСписокАктивныхПодключений(УникальныйИдентификатор);
КонецПроцедуры

&НаКлиенте
Асинх Процедура Подключить()
	
	Результат = Ждать WebSocketКлиент.СтартоватьСервис();
	
	Если НЕ Результат Тогда
		#Если НЕ ВебКлиент Тогда
		ПредупреждениеАсинх("Ошибка подключения к серверу!");
		#КонецЕсли
	КонецЕсли;
	
КонецПроцедуры


&НаКлиенте
Процедура ВнешнееСобытие(Источник, Событие, Данные)
		
	Текст = WebSocketКлиент.Base64ВСтроку(Данные); 
//	
//	Консоль = "Log >> " + Текст + Символы.ПС + Консоль;
	
	ВывестиЛог(Текст);
	
	СтруктураСообщения = WebSocketСервер.ПолучитьСтруктуруСообщения(Текст);
	
	Если СтруктураСообщения.Свойство("uuid_form") И 
		СокрЛП(СтруктураСообщения.uuid_form) <> "" И СокрЛП(СтруктураСообщения.uuid_form) <> РаботаСJSON.ПустаяСсылка() И
		Новый УникальныйИдентификатор(СтруктураСообщения.uuid_form) <> УникальныйИдентификатор Тогда
		Возврат;
	КонецЕсли;
	
	ОбработатьСообщениеСервера(СтруктураСообщения);

КонецПроцедуры

&НаСервере
Процедура УдалитьИзСписка(ИмяСписка, Идентификатор)
	мТаблица = РеквизитФормыВЗначение(ИмяСписка, Тип("ТаблицаЗначений"));
	ТекСтрока = мТаблица.Найти(СокрЛП(Идентификатор), "uuid");
	Если ТекСтрока <> Неопределено Тогда
		мТаблица.Удалить(ТекСтрока);
	КонецЕсли; 
	ЗначениеВРеквизитФормы(мТаблица, ИмяСписка);
КонецПроцедуры

&НаСервере
Процедура ДобавитьВСписок(ИмяСписка, Идентификатор, ДанныеСтроки)
	мТаблица = РеквизитФормыВЗначение(ИмяСписка, Тип("ТаблицаЗначений"));
	ТекСтрока = мТаблица.Найти(СокрЛП(Идентификатор), "uuid");
	Если ТекСтрока = Неопределено Тогда
		НоваяСтрока = мТаблица.Добавить();
		ЗаполнитьЗначенияСвойств(НоваяСтрока, ДанныеСтроки);
	КонецЕсли; 
	ЗначениеВРеквизитФормы(мТаблица, ИмяСписка);
КонецПроцедуры

&НаКлиенте
Процедура ОбработатьСообщениеСервера(СтруктураСообщения)
	
	Если СтруктураСообщения.result = "error" Тогда
		Сообщить("Ошибка: " + СтруктураСообщения.message, СтатусСообщения.Важное);
		Возврат;
	КонецЕсли;
	
	Команда = СтруктураСообщения.command;
	
	Если Команда = "get_active_users" Тогда 
		Результат = РаботаСJSON.ПрочитатьОтветСервера(СтруктураСообщения.message); 
		ОбновитьСписокАктивныхПодключений(Результат); 
	ИначеЕсли Команда = "set_client_param" Тогда
		мСообщение = СтрРазделить(СтруктураСообщения.message, ":");
		Если мСообщение.Количество() = 3 Тогда
			НовыйКлиент = Новый Структура("uuid,name", СокрЛП(мСообщение[2]), СокрЛП(мСообщение[1]));
			ДобавитьВСписок("АктивныеПользователи", НовыйКлиент.uuid, НовыйКлиент);
		КонецЕсли;
	ИначеЕсли Команда = "client_leave" Тогда
		мСообщение = СтрРазделить(СтруктураСообщения.message, ":");
		Если мСообщение.Количество() = 3 Тогда
			УдалитьИзСписка("АктивныеПользователи", СокрЛП(мСообщение[2]));			 
		КонецЕсли;	
	ИначеЕсли Команда = "get_users" Тогда 
		Результат = РаботаСJSON.ПрочитатьОтветСервера(СтруктураСообщения.message);  
		ОбновитьСписокЗарегистрированныхПользователей(Результат);
	ИначеЕсли Команда = "add_user" Тогда 	
		ОбновитьСписокПользователей(0);
	ИначеЕсли Команда = "update_user" Тогда 	
		ОбновитьСписокПользователей(0);
	Иначе
		Сообщить( СтруктураСообщения.message);
	КонецЕсли;                          	
	
	
КонецПроцедуры

&НаКлиенте
Процедура ОбновитьСписокЗарегистрированныхПользователей(ОтветСервера)
	Если ТипЗнч(ОтветСервера) = Тип("Массив") ТОгда
		ПользователиВБазе.Очистить();
		Для Каждого ТекСтрук Из ОтветСервера Цикл
			НоваяСтрока = ПользователиВБазе.Добавить();
			ЗаполнитьЗначенияСвойств(НоваяСтрока, ТекСтрук);
			НоваяСтрока.Активность = ПолучитьОписаниеАктивности(НоваяСтрока.Ref);
		КонецЦикла;
	КонецЕсли;
КонецПроцедуры

&НаСервере
Функция ПолучитьОписаниеАктивности(Идентификатор)
	мАктивныеПользователи = РеквизитФормыВЗначение("АктивныеПользователи", Тип("ТаблицаЗначений"));
	Если мАктивныеПользователи.Найти(Идентификатор, "user_uuid") <> Неопределено Тогда
		Возврат "В сети";
	Иначе
		Возврат "Не в сети";
	КонецЕсли;
КонецФункции

&НаКлиенте
Процедура ОбновитьСписокАктивныхПодключений(ОтветСервера)
	
	АктивныеПользователи.Очистить();
	
	Для Каждого СтруктураПодключения Из ОтветСервера Цикл
		НоваяСтрока = АктивныеПользователи.Добавить();
		ЗаполнитьЗначенияСвойств(НоваяСтрока, СтруктураПодключения);
	КонецЦикла;
	
КонецПроцедуры

&НаКлиенте
Процедура ПередЗакрытием(Отказ, ЗавершениеРаботы, ТекстПредупреждения, СтандартнаяОбработка)
	ОтключитьОбработчикОжидания("ПроверкаАктивностиКлиента");
КонецПроцедуры

&НаКлиенте
Процедура ПроверкаАктивностиКлиента() Экспорт

	Если ВебСокетКлиент <> Неопределено Тогда
		СостояниеПодключения = "" + ВебСокетКлиент.ТекущееИмя() + " : " + ?(ВебСокетКлиент.Запущен(), "Подключен", "Не подключен"); 
		Если НЕ ВебСокетКлиент.Запущен() Тогда
			АктивныеПользователи.Очистить();
			ПользователиВБазе.Очистить();
		КонецЕсли;
	Иначе
		СостояниеПодключения = "" + Пользователи.ТекущийПользователь() + " : Не подключен";   
	КонецЕсли;
	
КонецПроцедуры

&НаКлиенте
Процедура ИзменитьПользователя(ВыбранноеЗначение)
	
	Если ВебСокетКлиент = Неопределено ИЛИ НЕ ВебСокетКлиент.Запущен() Тогда
		Возврат;
	КонецЕсли;
	
	ТекущаяСтрока = ПользователиВБазе.НайтиПоИдентификатору(ВыбранноеЗначение);
	СсылкаНаПользователя = ПолучитьПользователяБазы(ТекущаяСтрока.ref);
	
	ПараметрыФормы = Новый Структура;
	ПараметрыФормы.Вставить("ПользовательИБ", СсылкаНаПользователя);
	ПараметрыФормы.Вставить("Представление", ТекущаяСтрока.SecondField);
	ПараметрыФормы.Вставить("Имя", ТекущаяСтрока.FirstField);
	ПараметрыФормы.Вставить("Идентификатор", ТекущаяСтрока.ref);
	ПараметрыФормы.Вставить("Роль", ТекущаяСтрока.role);
	
	ОписаниеОповещения = Новый ОписаниеОповещения("РезультатИзмененияПользователя", ЭтотОбъект, ПараметрыФормы);

	ОткрытьФорму("Обработка.WebSocketService.Форма.РегистрацияПользователя", ПараметрыФормы,,,,,ОписаниеОповещения);
	
КонецПроцедуры

&НаКлиенте
Процедура РезультатИзмененияПользователя(Результат, ДополнительныеПараметры) Экспорт

	Если Результат <> Неопределено И Результат <> КодВозвратаДиалога.Отмена ТОгда
		_Хеш = ВебСокетКлиент.ПолучитьХеш(Результат.Имя, Результат.Пароль);
		 
		ПараметрыЗапроса = ИзменитьПользователяПолучитьПараметрыЗапроса(Результат, _Хеш);
		
		ВебСокетКлиент.КомандаСерверу("update_user", Строка(УникальныйИдентификатор), ПараметрыЗапроса);
	КонецЕсли;

КонецПроцедуры

&НаСервереБезКонтекста
Функция ПолучитьПользователяБазы(Идентификатор)
		
	ИдентификаторПользователя = Новый УникальныйИдентификатор(Идентификатор);
	
	Пользователь = Справочники.Пользователи.ПолучитьСсылку(ИдентификаторПользователя);

	Возврат Пользователь;
			  
			      
КонецФункции

&НаСервере
Функция ИзменитьПользователяПолучитьПараметрыЗапроса(Результат, _Хеш)	
	
	ТекущаяСтрока = ПользователиВБазе.НайтиПоИдентификатору(Элементы.ПользователиВБазе.ТекущаяСтрока);
	
	Если ТекущаяСтрока <> Неопределено И Результат <> Неопределено Тогда
		  СтруктураПараметров = Новый Структура;
		  Если Результат.Имя <> ТекущаяСтрока.FirstField ТОгда
			  СтруктураПараметров.Вставить("FirstField", Результат.Имя);
		  КонецЕсли;
		  Если Результат.Представление <> ТекущаяСтрока.SecondField ТОгда
			  СтруктураПараметров.Вставить("SecondField", Результат.Представление);
		  КонецЕсли;
		  Если Результат.Роль <> ТекущаяСтрока.role ТОгда
			  СтруктураПараметров.Вставить("role", Результат.Роль);
		  КонецЕсли;
		  
		  СтруктураПараметров.Вставить("hash", _Хеш);
		  
		  Если СтруктураПараметров.Количество() > 0 Тогда
			  Параметры = Новый Структура();
			  Параметры.Вставить("set", СтруктураПараметров);
			  Параметры.Вставить("where", Новый Структура("Ref", ТекущаяСтрока.ref ));
			  
			  Запись = Новый ЗаписьJSON; 
			  Запись.УстановитьСтроку();
			  ЗаписатьJSON(Запись, Параметры);
			  
			  Возврат Запись.Закрыть();
			
			  
		  КонецЕсли;
		  
	КонецЕсли;
	
	Возврат "";
	
КонецФункции


&НаКлиенте
Процедура ПолучитьТекущуюДату(Команда)
		
	Если ВебСокетКлиент <> Неопределено Тогда
		Сообщить(ВебСокетКлиент.ТекущаяДата());
	КонецЕсли;

КонецПроцедуры

&НаКлиенте
Процедура ТекущаяДатаВСекундах(Команда)
	
	Если ВебСокетКлиент <> Неопределено Тогда
		Сообщить(ВебСокетКлиент.ТекущаяДатаВСекундах());
	КонецЕсли;

КонецПроцедуры

&НаКлиенте
Процедура ТекущаяДатаВСекундахВДату(Команда)
		
	Если ВебСокетКлиент <> Неопределено Тогда
		unixtime = ВебСокетКлиент.ТекущаяДатаВСекундах() + ВебСокетКлиент.ПолучитьСмещениеВременнойЗоны();
		Сообщить(Дата(1970,1,1,1,0,0) + unixtime);
	КонецЕсли;

	//unixtime = 1404158409000/1000;
	//Дата1С = Дата(1970,1,1,1,0,0) + unixtime;// 30.06.2014 21:00:09

КонецПроцедуры




