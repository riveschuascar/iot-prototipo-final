import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class ChangeIntervalIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("ChangeIntervalIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = handler_input.attributes_manager.session_attributes.get("thing_name")
        try:
            if not thing_name:
                speak_output = "Sesión no inicializada. Por favor vuelve a empezar."
                return handler_input.response_builder.speak(speak_output).ask(speak_output).response

            seconds = ask_utils.get_slot_value(handler_input, "time")

            if not seconds:
                speak_output = "No entendí cada cuanto tiempo debe ser el intervalo."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            # Convert to milliseconds
            seconds = int(seconds) * 1000

            payload = {"state": {"desired": {"sensorInterval": seconds}}}

            speak_output = f"Configurando intervalo de {seconds/1000} segundos."

            iot_client.update_thing_shadow(
                thingName=thing_name,
                payload=json.dumps(payload)
            )

            reprompt_output = "Puedes decir consultar nivel de gas o configurar otro valor."

        except Exception as e:
            logger.error(f"Error al controlar el modo automático: {str(e)}")
            speak_output = "Hubo un error al intentar controlar el modo automático."
            reprompt_output = "Por favor dime qué deseas hacer."

        return (
            handler_input.response_builder
            .speak(speak_output)
            .ask(reprompt_output)
            .response
        )