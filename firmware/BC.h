#ifndef BC_h
#define BC_h

#include <SPI.h>

// константы используемые для контроля целостности принимаемого пакета,
// позволяют понять что конкретно мы принимаем в данный момент и не потерялось ли чего

// признак начала пакета
#define LC75874_X_START 0x45
// признак окончания первого пакета
#define LC75874_1_END 0b00
// признак окончания второго пакета
#define LC75874_2_END 0b10
// признак окончания третьего пакета
#define LC75874_3_END 0b01
// признак окончания четвертого пакета
#define LC75874_4_END 0b11

// не принимаем данные от мастера
#define BC_STATE_IDLE 100
// инициирован сброс показателя
#define BC_STATE_RESET_PRESS 101
// сброс показателя в процессе
#define BC_STATE_RESET_RELEASE 102
// инициировано переключение режима
#define BC_STATE_MODE_PRESS 103
// переключение режима в процессе
#define BC_STATE_MODE_RELEASE 104
// ожидаем первого байта от мастера
#define BC_STATE_START 0
// успешно приняли данные от мастера
#define BC_STATE_DONE 44

// задержка удержания кнопки переключения режимов
#define MODE_ACTION_DELAY_MS 500
// задержка удержания кнопки сброса показателя
#define RESET_ACTION_DELAY_MS 3000

// константы для перевода миль и галлонов в километры и литры
#define MILE_TO_KM 1.60934
#define MPG_TO_L100KM 282.48163

// 24й байт посылки содержит информацию о индикаторах единиц измерения, по ним
// можно определить какой из трех возможных параметров и в какой единице
// измерения в данный момент отображен на экране

// запас топлива, км
#define METERAGE_FUEL_KM 0b00000010
// запас топлива, миль
#define METERAGE_FUEL_MILES 0b10000000
// скорость, км/ч
#define METERAGE_SPEED_KMH 0b00000011
// скорость, миль/ч
#define METERAGE_SPEED_MPH 0b00010000
// расход, км/л
#define METERAGE_CONSUMPTION_KML 0b01100000
// расход, миль/галлон
#define METERAGE_CONSUMPTION_MPG 0b00001000
// расход, л/100км
#define METERAGE_CONSUMPTION_L100KM 0b01000100

// макросы для управления виртуальным LCD дисплеем,
// каждый из макросов проставляет соответствующий бит в 32х - битное число
// (по сути включает / выключает определенный сегмент определенного знакоместа)
// которое затем используется для получения символа (цифры) отображенного
// на соответствующем знакоместе, формат (0 - младший бит):
// - 0..6 - разряд единиц
// - 7..13 - разряд десятков
// - 14..20 - разряд сотен
// - 21 - разряд тысяч
// - 22 - точка
// - 23 - минус
// по сути нужны лишь для того, чтобы потом, при разборе значения
// знать что откуда доставать, поскольку в пакете от БК до дисплея
// биты сегментов - идут вразнобой

// единицы - верхний сегмент
#define LCD_1T(value, bit) ((value >> bit & 1UL) << 0)
// единицы - средний сегмент
#define LCD_1C(value, bit) ((value >> bit & 1UL) << 1)
// единицы - нижний сегмент
#define LCD_1B(value, bit) ((value >> bit & 1UL) << 2)
// единицы - верхний левый сегмент
#define LCD_1TL(value, bit) ((value >> bit & 1UL) << 3)
// единицы - верхний правый сегмент
#define LCD_1TR(value, bit) ((value >> bit & 1UL) << 4)
// единицы - нижний левый сегмент
#define LCD_1BL(value, bit) ((value >> bit & 1UL) << 5)
// единицы - нижний правый сегмент
#define LCD_1BR(value, bit) ((value >> bit & 1UL) << 6)
// десятки - верхний сегмент
#define LCD_10T(value, bit) ((value >> bit & 1UL) << 7)
// десятки - средний сегмент
#define LCD_10C(value, bit) ((value >> bit & 1UL) << 8)
// десятки - нижний сегмент
#define LCD_10B(value, bit) ((value >> bit & 1UL) << 9)
// десятки - верхний левый сегмент
#define LCD_10TL(value, bit) ((value >> bit & 1UL) << 10)
// десятки - верхний правый сегмент
#define LCD_10TR(value, bit) ((value >> bit & 1UL) << 11)
// десятки - нижний левый сегмент
#define LCD_10BL(value, bit) ((value >> bit & 1UL) << 12)
// десятки - нижний правый сегмент
#define LCD_10BR(value, bit) ((value >> bit & 1UL) << 13)
// сотни - верхний сегмент
#define LCD_100T(value, bit) ((value >> bit & 1UL) << 14)
// сотни - средний сегмент
#define LCD_100C(value, bit) ((value >> bit & 1UL) << 15)
// сотни - нижний сегмент
#define LCD_100B(value, bit) ((value >> bit & 1UL) << 16)
// сотни - верхний левый сегмент
#define LCD_100TL(value, bit) ((value >> bit & 1UL) << 17)
// сотни - верхний правый сегмент
#define LCD_100TR(value, bit) ((value >> bit & 1UL) << 18)
// сотни - нижний левый сегмент
#define LCD_100BL(value, bit) ((value >> bit & 1UL) << 19)
// сотни - нижний правый сегмент
#define LCD_100BR(value, bit) ((value >> bit & 1UL) << 20)
// тысячи - контроллируется одним сегментом
#define LCD_1000(value, bit) ((value >> bit & 1UL) << 21)
// десятичный разделитель между десятками и единицами
#define LCD_DOT(value, bit) ((value >> bit & 1UL) << 22)
// знак минуса
#define LCD_MINUS(value, bit) ((value >> bit & 1UL) << 23)

// константы используемые для конвертации 7 - битной маски сегмента в отображаемое значение

// цифра 0
#define LCD_CHAR_0 125
// цифра 1
#define LCD_CHAR_1 80
// цифра 2
#define LCD_CHAR_2 55
// цифра 3
#define LCD_CHAR_3 87
// цифра 4
#define LCD_CHAR_4 90
// цифра 5
#define LCD_CHAR_5 79
// цифра 6
#define LCD_CHAR_6 111
// цифра 7
#define LCD_CHAR_7 81
// цифра 8
#define LCD_CHAR_8 127
// цифра 9
#define LCD_CHAR_9 95
// пустое знакоместо
#define LCD_CHAR_SPACE 10
// неизвестный символ
#define LCD_CHAR_UNKNOWN 11

// ввиду того, что обработчик прерывания не может иметь доступа к приватным
// членам класса, это единственная возможность разграничить права доступа
namespace BC_private {

	// пин контроллирующий кнопку режима
	uint8_t PIN_MODE;
	// пин контроллирующий кнопку сброса
	uint8_t PIN_RESET;

	// внешняя температура
	float temperature = INFINITY;
	// запас топлива (км)
	float fuel = INFINITY;
	// средняя скорость (км/ч)
	float speed = INFINITY;
	// расход топлива (л/100км)
	float consumption = INFINITY;

	// время начала сброса / переключения режима
	uint32_t actionTime;
	// сбросить среднюю скорость при следующем переключении режима
	bool doResetSpeed = false;
	// сбросить расход топлива при следующем переключении режима
	bool doResetConsumption = false;

	volatile uint8_t state;
	volatile uint8_t b04;
	volatile uint8_t b05;
	volatile uint8_t b18;
	volatile uint8_t b19;
	volatile uint8_t b20;
	volatile uint8_t b23;
	volatile uint8_t lcdMeterageUnit;
	volatile uint8_t b28;

	// конвертирует 7 - битную маску сегмента полученную из 32х - битного числа
	// описывающего состояние виртуального LCD - дисплея в отображаемую цифру
	uint8_t LCD_getDigit(uint32_t value) {
		switch (value) {
			case LCD_CHAR_0: return 0;
			case LCD_CHAR_1: return 1;
			case LCD_CHAR_2: return 2;
			case LCD_CHAR_3: return 3;
			case LCD_CHAR_4: return 4;
			case LCD_CHAR_5: return 5;
			case LCD_CHAR_6: return 6;
			case LCD_CHAR_7: return 7;
			case LCD_CHAR_8: return 8;
			case LCD_CHAR_9: return 9;
			case 0: return LCD_CHAR_SPACE;
			default: return LCD_CHAR_UNKNOWN;
		}
	}

	// конвертирует 32х - битное число описывающее состояние
	// виртуального LCD - дисплея в отображаемое число
	float LCD_getValue(uint32_t value) {
		// получаем разряд единиц
		uint8_t D0 = LCD_getDigit(value & 0x7F);
		// в случае если он пустой или неопознан, возвращаем ошибку
		if (D0 >= LCD_CHAR_SPACE) return INFINITY;
		// получаем разряд десятков
		uint8_t D1 = LCD_getDigit(value >> 7 & 0x7F);
		// получаем признак дробного
		bool isFloat = (value >> 22 & 1);
		// в случае если цифра неопознана, либо отсутствует но есть точка, возвращаем ошибку
		if (D1 == LCD_CHAR_UNKNOWN || (D1 == LCD_CHAR_SPACE && isFloat)) return INFINITY;
		// получаем разряд сотен
		uint8_t D2 = LCD_getDigit(value >> 14 & 0x7F);
		// в случае если цифра неопознана, либо предыдущая цифра отсутствует, возвращаем ошибку
		if (D2 == LCD_CHAR_UNKNOWN || (D2 != LCD_CHAR_SPACE && D1 == LCD_CHAR_SPACE)) return INFINITY;
		// получаем разряд тысяч
		uint8_t D3 = (value >> 21 & 1);
		// в случае если есть тысячи, а предыдущая цифра отсутствует, возвращаем ошибку
		if (D3 && D2 == LCD_CHAR_SPACE) return INFINITY;
		// вычисляем результат
		float result = D3 * 1000 + D0;
		if (D2 != LCD_CHAR_SPACE) result += D2 * 100;
		if (D1 != LCD_CHAR_SPACE) result += D1 * 10;
		if (isFloat) result /= 10;
		// признак отрицательного значения
		if (value >> 23 & 1) result = -result;
		return result;
	}


	// вызывается после того как мы приняли последний байт от мастера
	// конвертирует LCD - значения в float (конвертирует в общепринятые еденицы
	// измерения, при необходимости), контроллирует флаги принудительного сброса показаний
	// средней скорости и расхода, обновляя стэйт соответствующим образом
	void doUpdate() {

		// один из показателей, преобразуем LCD - значение в число
		float meterage = LCD_getValue(
			LCD_100T(b18, 4) | LCD_100TL(b18, 5) | LCD_100BL(b18, 6) | LCD_1000(b18, 7) |
			LCD_100TR(b19, 0) | LCD_100C(b19, 1) | LCD_100BR(b19, 2) | LCD_100B(b19, 3) | LCD_10T(b19, 4) | LCD_10TL(b19, 5) | LCD_10BL(b19, 6) |
			LCD_10TR(b20, 0) | LCD_10C(b20, 1) | LCD_10BR(b20, 2) | LCD_10B(b20, 3) |
			LCD_1T(b23, 0) | LCD_1TL(b23, 1) | LCD_1BL(b23, 2) | LCD_1TR(b23, 4) | LCD_1C(b23, 5) | LCD_1BR(b23, 6) | LCD_1B(b23, 7) |
			LCD_DOT(b28, 7)
		);

		switch (lcdMeterageUnit) {

			// читаем показатель остатка топлива
			case METERAGE_FUEL_KM:
			case METERAGE_FUEL_MILES: {

				// конвертируем в км, в случае если установлены мили и мы прочитали что - то осмысленное
				if (lcdMeterageUnit == METERAGE_FUEL_MILES && meterage != INFINITY && meterage != 0)
					meterage = ceil(meterage * MILE_TO_KM);

				// обновляем значение
				fuel = meterage;

				break;
			}

			// читаем показатель средней скорости
			case METERAGE_SPEED_KMH:
			case METERAGE_SPEED_MPH: {

				// проверяем нужно ли сбросить показатель
				if (doResetSpeed) {
					doResetSpeed = false;
					meterage = INFINITY;
					state = BC_STATE_RESET_PRESS;
				}

				// конвертируем в км/ч, в случае если установлены мили и мы прочитали что - то осмысленное
				if (lcdMeterageUnit == METERAGE_SPEED_MPH && meterage != INFINITY && meterage != 0)
					meterage = ceil(meterage * MILE_TO_KM);

				// обновляем значение
				speed = meterage;

				break;
			}

			// читаем показатель расхода
			case METERAGE_CONSUMPTION_L100KM:
			case METERAGE_CONSUMPTION_KML:
			case METERAGE_CONSUMPTION_MPG: {

				// проверяем нужно ли сбросить показатель
				if (doResetConsumption) {
					doResetConsumption = false;
					meterage = INFINITY;
					state = BC_STATE_RESET_PRESS;
				}

				// конвертируем в л/100км, в случае если мы прочитали что - то осмысленное и установлены км/л или галлоны
				if (meterage != INFINITY && meterage != 0) {
					if (lcdMeterageUnit == METERAGE_CONSUMPTION_MPG) meterage = round(10 * (MPG_TO_L100KM / meterage)) / 10;
					else if (lcdMeterageUnit == METERAGE_CONSUMPTION_KML) meterage = round(10 * (100 / meterage)) / 10;
				}

				// обновляем значение
				consumption = meterage;

				break;
			}
		}

		// температура, преобразуем LCD - значение в число и обновляем
		temperature = LCD_getValue(
			LCD_10T(b04, 0) | LCD_10TL(b04, 1) | LCD_10BL(b04, 2) | LCD_MINUS(b04, 3) | LCD_10TR(b04, 4) | LCD_10C(b04, 5) | LCD_10BR(b04, 6) | LCD_10B(b04, 7) |
			LCD_1T(b05, 0) | LCD_1TL(b05, 1) | LCD_1BL(b05, 2) | LCD_1TR(b05, 4) | LCD_1C(b05, 5) | LCD_1BR(b05, 6) | LCD_1B(b05, 7)
		);

		// в случае если не нужно сбрасывать показатель, переключаем режим
		if (state == BC_STATE_DONE) state = BC_STATE_MODE_PRESS;
	}


	// обработчик SPI - прерывания
	ISR(SPI_STC_vect) {
		uint8_t value = SPDR;
		switch (state++) {
			case 0: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 4: b04 = value; break;
			case 5: b05 = value; break;
			case 10: if (value >> 6 != LC75874_1_END) state = BC_STATE_START; break;
			case 11: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 18: b18 = value; break;
			case 19: b19 = value; break;
			case 20: b20 = value; break;
			case 21: if (value >> 6 != LC75874_2_END) state = BC_STATE_START; break;
			case 22: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 23: b23 = value; break;
			case 24: lcdMeterageUnit = value; break;
			case 28: b28 = value; break;
			case 32: if (value >> 6 != LC75874_3_END) state = BC_STATE_START; break;
			case 33: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 43: if (value >> 6 == LC75874_4_END) SPI.detachInterrupt(); else state = BC_STATE_START; break;
		}
	}

}

namespace BC {

	// процедура инициализации, вызывается в setup()
	// принимает номера пинов на которые повешаны кнопки сброса и режима
	void init(uint8_t PIN_MODE, uint8_t PIN_RESET) {
		using namespace BC_private;
		// настройка SPI в режиме slave
		SPCR |= (1 << SPE);
		SPI.setBitOrder(LSBFIRST);
		SPI.setDataMode(SPI_MODE3);
		// инициализация переменных и настройка пинов
		state = BC_STATE_IDLE;
		pinMode(BC_private::PIN_MODE = PIN_MODE, OUTPUT);
		pinMode(BC_private::PIN_RESET = PIN_RESET, OUTPUT);
		// просто для того, чтобы было понятно в каком состоянии должны
		// находиться соответствующие пины по - умолчанию
		digitalWrite(PIN_MODE, LOW);
		digitalWrite(PIN_RESET, LOW);
	}

	// процедура обновления, вызывается в loop(), возвращает true, в случае,
	// если доступны новые данные
	void update() {

		using namespace BC_private;

		switch (state) {

			// начинаем получать данные от мастера
			case BC_STATE_IDLE:
				state = BC_STATE_START;
				SPI.attachInterrupt();
				break;

			// приняли данные от мастера, обновляем показатели
			case BC_STATE_DONE:
				doUpdate();
				break;

			// нажимаем кнопку сброса
			case BC_STATE_RESET_PRESS:
				actionTime = millis();
				digitalWrite(PIN_RESET, HIGH);
				state = BC_STATE_RESET_RELEASE;
				break;

			// отпускаем кнопку сброса
			case BC_STATE_RESET_RELEASE:
				if (millis() - actionTime >= RESET_ACTION_DELAY_MS) {
					digitalWrite(PIN_RESET, LOW);
					state = BC_STATE_MODE_PRESS;
				}
				break;

			// нажимаем кнопку переключения режима
			case BC_STATE_MODE_PRESS:
				actionTime = millis();
				digitalWrite(PIN_MODE, HIGH);
				state = BC_STATE_MODE_RELEASE;
				break;

			// отпускаем кнопку переключения режима
			case BC_STATE_MODE_RELEASE:
				if (millis() - actionTime >= MODE_ACTION_DELAY_MS) {
					digitalWrite(PIN_MODE, LOW);
					state = BC_STATE_IDLE;
				}
				break;

		}
	}

	// выполняет сброс показателя средней скорости, сброс будет произведен
	// при следующем обновлении
	void resetSpeed() {
		using namespace BC_private;
		doResetSpeed = true;
	}

	// выполняет сброс показателя среднего расхода, сброс будет произведен
	// при следующем обновлении
	void resetConsumption() {
		using namespace BC_private;
		doResetConsumption = true;
	}

	// возвращает внешнюю температуру
	float getTemperature() {
		using namespace BC_private;
		return temperature;
	}

	// возвращает остаток топлива в километрах
	float getFuel() {
		using namespace BC_private;
		return fuel;
	}

	// возвращает среднюю скорость в км/ч
	float getSpeed() {
		using namespace BC_private;
		return speed;
	}

	// возвращает средний расход, в л/100км
	float getConsumption() {
		using namespace BC_private;
		return consumption;
	}

}

#endif