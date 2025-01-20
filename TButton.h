/*
 * Библиотека для работы с механическими контактами (кнопками, переключателями и пр.) во FreeRTOS
 * Функционал:
 * - Фильтрация дребезга
 * - Отработка длинного нажатия
 *
 * Используется HAL и FreeRTOS
 */

#ifndef INC_TBUTTON_H_
#define INC_TBUTTON_H_

#include "stm32f1xx_hal.h" //поменять при смене ядра

typedef enum {
	TBUTTON_BTYPE_NO,
	TBUTTON_BTYPE_NC
} ButtonType; // Тип контакта - нормально открытый/нормально закрытый

typedef enum {
	TBUTTON_PT_PULL_UP,
	TBUTTON_PT_PULL_DOWN
} PinType; //Подтяжка пина вверх, либо вниз. При чтении ксорим с типом кнопки

typedef enum {
	TBUTTON_STATE_RELEASED,
	TBUTTON_STATE_PRESSED, // Этот тип состояния для используется внутренних обработчиков и никогда не передаётся во внешние
	TBUTTON_STATE_CLICKED,
	TBUTTON_STATE_LONG_PRESSED
} ButtonState; //Возможные состояния кнопки

typedef struct{
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin; // Пин подключения кнопки
	ButtonType ButtonType:1;
	PinType PinType:1;
	ButtonState ButtonState:2; //текущее состояние кнопки
	ButtonState PreviousState:2; //состояние кнопки в момент предыдущего опроса
	uint8_t Counter; // счётчик опросов
	uint8_t Confirms; // значение подтвеждения
	uint8_t LongTime; // Время долгого нажатия в секундах
	uint32_t StartTime; // Время начала отсчёта для долгого нажатия
} TButton;

TButton TButtonAssign(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, ButtonType ButtonType, PinType PinType, uint8_t Confirms, uint8_t LongTime); //Инициализация кнопки
void TButtonTick(TButton Button); // Опрос кнопки. Запускать периодически в задаче опроса кнопок
ButtonState TButtonGetState (TButton Button); // Чтение состояния кнопки. Вызывать в обработчике
void TButtonForcedRelease(TButton Button); // Cброс кнопки в состояние RELEASED. Использовать в обработчике нажатия или для фильтрации одновременного нажатия конфликтующих кнопок.
void TButtonDestroy(TButton * Button);

#endif /* INC_TBUTTON_H_ */
