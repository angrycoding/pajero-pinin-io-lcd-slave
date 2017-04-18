# Доработка штатного БК Pajero Pinin / IO
Хотелось бы сказать спасибо человеку который сделал это: http://carisma-club.su/index.php?showtopic=2770 за то, что он это сделал, а также за то, что все таки **не дал** мне прошивку, благодаря ему, мне пришлось разбираться с этим самому, что несомненно пошло мне только на пользу. Здесь в одном месте собрана вся информация, о БК которую мне удалось накопать из различных источников.

Штатный бортовой комп Pinin/IO состоит из двух плат, процессорной платы и платы дисплея, данный проект описывает, что нужно сделать, чтобы отрезать плату дисплея и бахнуть вместо нее ардуину, которая будет выполнять роль дисплея. То есть считывать то, что должно быть отображено на дисплее (температуру, время, расход, запас топлива, среднюю скорость) и выводить это в последовательный порт, либо куда либо еще, тут уж зависит от вашей фантазии. **Внимание!** Эта хрень работает с дисплейной платой под управлением чипа [LC75874](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/LC75874-Sanyo%20Semicon%20Device.pdf). С другими чипами может работать а может и не работать, в любом случае это не займет много времени, чтобы переделать все на другой чип, так как я на 100% уверен что другие чипы используют схожий протокол.

Дисплей со всеми включенными сегментами:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/all_segments.jpg)

Схема подключения ардуины и процессорной платы:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/circuit.png)

Вот так это выглядит в реальности:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/connections.png)

## Протокол взаимодействия процессорной платы и платы дисплея
Все взаимодействие построено на очень похожем на SPI протоколе. То есть у нас есть по сути две линии - CLOCK и DATA, изменение значения CLOCK с 0 на 1, говорит на о том, что на линии DATA можно считать бит данных. Более подробно, протокол описан в документации на чип [LC75874](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/LC75874-Sanyo%20Semicon%20Device.pdf), там есть еще одна линия, изменение значения на которой дает нам понять о том что конкретно мы  данный момент принимаем (адрес чипа или сами данные), но я на нее забил, поскольку тратить еще один вход ради 8 бит - это непозволительная роскошь, поэтому я просто читаю все от начала и до конца.

Один пакет данных от процессора к дисплею состоит из четырех блоков по 11 байт в каждом (то есть весь пакет - это 44 байта), каждый из блоков начинается с байта адреса чипа = 0x45 (хрен знает зачем он там нужен, наверное для того чтобы можно было на одну линию повесить несколько устройств, я не вникал), затем следует 10 байт описывающих состояние (включено / выключено) каждого сегмента LCD - дисплея. Не все из этих 10 байт - относятся к битам сегментов (см. документацию на [LC75874](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/LC75874-Sanyo%20Semicon%20Device.pdf)), несколько последних бит зарезервировано под какие то спец. нужды (не вникал), главное это последние два бита, для первого блока они будут равны 00, для второго 01, для третьего 10 и для последнего 11. Таким образом имея адрес (0x45), количество байт, и этот двухбитный счетчик блоков, можно читать и проверять инфу идующую на дисплей, больше ничего интересного там нет.

## Автономное питание бортового компа

На задней стенке БК есть два разъема:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/sockets.png)

Для того, чтобы запитать бортовой комп вне автомобиля, нужно подать +12 вольт на пины 25 и 26 разъема C-52, масса есть на корпусе.

## Подключение ардуины, чтение и вывод данных
На рисунке изображено как подключал ее я, в других модификациях БК, смотрите на то чтобы линии DATA и CLK чипа были подключены к соответствующим SPI линиям Arduino. Для того, чтобы понять какие биты в блоках описанных в предыдущем параграфе, мне пришлось разрезать шлейф идущий на дисплей и подключить ардуину в разрыв шлейфа. Затем я сформировал пакет состоящий из одних единиц, и выдал его дисплею, получив то, что изображено на первой картинке (все сегменты горят). Потом тыркая различные биты я нашел все то, что меня интересовало, а именно: сегменты часов (нужны лишь для тестирования), температуры, пробега расхода и т. д. все это описано в [bits_to_segments.txt](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/bits_to_segments.txt).

## Прошивка
Поскольку данные в блоках от процессора до дисплея представлены в формате "каши", то биты отвечающие за определенные сегменты определенных знакомест дисплея - могут находится в произвольных местах. Чтобы решить проблему канонизации маппинга бит - сегмент, пришлось сделать что то типа виртуального LCD индикатора, который работает на битовой арифметике. Также прикольнуло то, что дотошные японцы в случае когда температура окружающего воздуха больше или меньше предела измерений (-99 .. 99) то вместо неведомой хрени - отображается "HI" и "LO", и затем дисплей переходит в какой то странный режим который сбросить можно только зажиманием кнопки сброса одновременно с включением питания. 

Время 1:52:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/bc.jpg)

То же, но уже в терминале:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/terminal.jpg)

## Использование watchdog - таймера

Для предотвращения полного зависания чего - либо в ардуине, в прошивке использован т. н. "watchdog - таймер". Смысл его в том, чтобы в случае, если главный цикл loop(), был вызван в последний раз более 1 секунды назад (к примеру), то ардуина - автоматически перезагружается. Но смысл данного абзаца не в описании того как это работает, а того, что в большинстве Arduino - совместимых плат watchdog - таймер работает некорректно, поэтому если вы решили использовать данную функциональность, у вас есть следующие варианты:

- подобрать плату где watchdog таймер - работает корректно
- использовать альтернативный загрузчик (к примеру optiboot)
- снести загрузчик, выставить фьюзы и загружать прошивку вместо загрузчика
- закомментировать использование watchdog - таймера в исходном коде

В случае, если вы решите полностью забить на это и оставить все как есть, то при зависании ардуины она - перейдет в цикл бесконечной перезагрузки, выйти из которого можно будет лишь отключив ее от питания.

## Обработка различных единиц измерения

Штатный бортовой комп позволяет выводить показания в различных единицах измерения (запас топлива: км / миль, средняя скорость: км/ч / миль/ч, расход топлива: км/л, миль/галлон, л/100км), кстати мили - британские. Переключение между режимами отображения производится путем одновременного нажатия кнопки сброса (самая левая) и кнопки установки часов (H). Дабы не перекладывать задачу переключения режимов на ардуину, в прошивке я определяю какой конкретно параметр (запас топлива, средняя скорость, расход топлива) и в каких единицах измерения отображается в данный момент и просто перевожу их в км, км/ч и л/100км соответственно. Запас топлива и средняя скорость - всегда округляются до ближайшего целого, расход - округляется до одной цифры после запятой (штатно также, я проверял). Все это сделано для того, чтобы если по какой - либо неведомой причине режим отображения собъется, на хз какой, но точно не тот, что нужен, не пришлось бы припаивать обратно дисплейную плату с кнопками, чтобы переключить его обратно или танцевать с резистором и проводом вокруг того пина что это контроллирует.

## Эмуляция нажатия кнопок

- Кнопка сброса - 5К
- Кнопка "H" - 3К
- Кнопка "M" - 2К
- Кнопка "Set" - 1К
- Вход в режим установки яркости дисплея: кратковременно сброс, а потом не отпуская сброса кнопка "H"
- Вход в режим установки единиц измерения: кратковременно сброс, а потом не отпуская сброса кнопка "M"
- Кнопка переключения режимов (в случае если оригинальная магнитола с кнокпой "Disp" - на помойке) - 6.8К

Любители пасхальных яиц и багов в аркадных автоматах могут почесать репу над этим режимом:

![segments](https://github.com/angrycoding/pajero-pinin-io-lcd-slave/blob/master/docs/kr-mode.jpg)

В него можно войти, если зажать кнопку сброса и затем кнопку "Set" и подержать так секунды две. Хз что это за режим и для чего он был сделан, но там ничего нельзя изменить и кроме "KR", ничего больше получить не удалось, а жаль я ведь думал что нажав еще пару кнопок можно сыграть в пакмана или увидеть отрывок из мультика "Ну погоди!", в псевдографике.

## TODO

Посмотреть как приделать эту штуку к Android - приложению: https://github.com/Prosjekt2-09arduino/STK500-Android для того, чтобы можно было бы обновлять прошивку через приложение.
