import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class ControlServoIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("ControlServoIntent")(handler_input)

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
                speak_output = "No entendí si deseas abrir o cerrar la compuerta."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            action = action.lower()

            if action in ["abrir", "abre"]:
                servo_value = 90
                speak_output = "Abriendo la compuerta. ¿Deseas hacer otra acción?"

            elif action in ["cerrar", "cierra"]:
                servo_value = 0
                speak_output = "Cerrando la compuerta. ¿Deseas hacer otra acción?"

            else:
                speak_output = "Solo puedes decir abrir o cerrar la compuerta."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            payload = {"state": {"desired": {"servo": servo_value}}}

            iot_client.update_thing_shadow(
                thingName=thing_name,
                payload=json.dumps(payload)
            )

            reprompt_output = "Puedes decir consultar nivel de gas o configurar otro valor."

        except Exception as e:
            logger.error(f"Error al controlar la compuerta: {str(e)}")
            speak_output = "Hubo un error al intentar controlar la compuerta."
            reprompt_output = "Por favor dime qué deseas hacer."

        return (
            handler_input.response_builder
            .speak(speak_output)
            .ask(reprompt_output)
            .response
        )
