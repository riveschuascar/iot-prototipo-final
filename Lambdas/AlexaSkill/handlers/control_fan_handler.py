import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class ControlFanIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("ControlFanIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = handler_input.attributes_manager.session_attributes.get("thing_name")
        try:
            if not thing_name:
                speak_output = "Sesión no inicializada. Por favor vuelve a empezar."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            action = ask_utils.get_slot_value(handler_input, "action")

            if not action:
                speak_output = "No entendí si deseas activar o desactivar el ventilador."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            action = action.lower()

            if action in ["activar", "activa"]:
                isOn = True
                speak_output = "Activando el ventilador. ¿Deseas hacer otra acción?"

            elif action in ["desactivar", "desactiva"]:
                isOn = False
                speak_output = "Desactivando el ventilador. ¿Deseas hacer otra acción?"

            else:
                speak_output = "Solo puedes decir activar o desactivar el ventilador."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            payload = {"state": {"desired": {"fan": isOn}}}

            iot_client.update_thing_shadow(
                thingName=thing_name,
                payload=json.dumps(payload)
            )

            reprompt_output = "Puedes decir consultar nivel de gas o configurar otro valor."

        except Exception as e:
            logger.error(f"Error al controlar el ventilador: {str(e)}")
            speak_output = "Hubo un error al intentar controlar el ventilador."
            reprompt_output = "Por favor dime qué deseas hacer."

        return (
            handler_input.response_builder
            .speak(speak_output)
            .ask(reprompt_output)
            .response
        )