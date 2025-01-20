#include "TButton.h"
#include "stdlib.h"

TButton TButtonAssign(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, ButtonType ButtonType, PinType PinType, uint8_t Confirms, uint8_t LongTime){
	TButton new = {0};
	new.GPIO_Port = GPIO_Port;
	new.GPIO_Pin = GPIO_Pin;
	new.ButtonType = ButtonType;
	new.PinType = PinType;
	new.ButtonState = TBUTTON_STATE_RELEASED;
	new.PreviousState = TBUTTON_STATE_RELEASED;
	new.StartTime = HAL_GetTick();
	new.Counter = 1;
	new.Confirms = Confirms;

	return new;
}


void TButtonTick(TButton Button){ //Опрос кноки и фильтрация дребезга
	Button.PreviousState = Button.ButtonState;


	if (HAL_GPIO_ReadPin(Button.GPIO_Port, Button.GPIO_Pin) == (Button.ButtonType ^ Button.PinType)){
			Button.Counter++;
	} else {
			Button.Counter--;
	}
	if (Button.Counter <= 1){
		Button.Counter = 1;
		Button.ButtonState = TBUTTON_STATE_RELEASED;
	}


	if (Button.Counter >= Button.Confirms) { //При достижении значения подтвеждения нажатия
		Button.Counter = Button.Confirms;

		if (Button.ButtonState == TBUTTON_STATE_RELEASED) { // Если кнопка считалась отпущенной
				Button.ButtonState = TBUTTON_STATE_PRESSED; //счиаем её нажатой
				Button.StartTime = HAL_GetTick(); //регистрируем время нажатия
			}
		}

		if ((Button.ButtonState == TBUTTON_STATE_RELEASED) && ((Button.PreviousState == TBUTTON_STATE_PRESSED)||(Button.PreviousState == TBUTTON_STATE_CLICKED))) {
				Button.ButtonState = TBUTTON_STATE_CLICKED;
		}

		if ((Button.ButtonState == TBUTTON_STATE_PRESSED) && (Button.LongTime > 0) && (HAL_GetTick()-Button.StartTime>=Button.LongTime*1000)){
				Button.ButtonState = TBUTTON_STATE_LONG_PRESSED;
		}


}

void TButtonForcedRelease(TButton Button){
	Button.Counter=0;
	Button.ButtonState = TBUTTON_STATE_RELEASED;
	Button.PreviousState = TBUTTON_STATE_RELEASED;
}

void TButtonDestroy(TButton * Button){
	free(Button);
}

ButtonState TButtonGetState (TButton Button){

	if (Button.ButtonState == TBUTTON_STATE_LONG_PRESSED) {
		return TBUTTON_STATE_LONG_PRESSED;
	}

	if (Button.ButtonState == TBUTTON_STATE_CLICKED) {
			return TBUTTON_STATE_CLICKED;
		}

	if ((Button.ButtonState == Button.PreviousState) || (Button.PreviousState == TBUTTON_STATE_RELEASED)){
			return TBUTTON_STATE_RELEASED;
		}


		return TBUTTON_STATE_RELEASED;
}
