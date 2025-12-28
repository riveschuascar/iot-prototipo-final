import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class SetThresholdIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("SetThresholdIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = handler_input.attributes_manager.session_attributes.get("thing_name")
        try:
            if not thing_name:
                speak_output = "Sesión no inicializada. Por favor vuelve a empezar."
                return handler_input.response_builder.speak(speak_output).ask(speak_output).response

            device = ask_utils.get_slot_value(handler_input, "device")

            if not device:
                speak_output = "No entendí que dispositivo quieres configurar. Por favor dime qué deseas hacer."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            value = ask_utils.get_slot_value(handler_input, "value")

            if not value:
                speak_output = "No entendí a que valor deseas configurar el dispositivo."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            device = device.lower()
            value = int(value)

            if device in ["servo", "compuerta"]:
                payload = {"state": {"desired": {"gasThresholdServo": value}}}
            else:
                payload = {"state": {"desired": {"gasThresholdFan": value}}}

            speak_output = f"Configurando umbral de: {device} a {value} PPM."

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