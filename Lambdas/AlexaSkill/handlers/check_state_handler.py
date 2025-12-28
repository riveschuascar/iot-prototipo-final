import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class CheckStateIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("CheckStateIntent")(handler_input)

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

            response = iot_client.get_thing_shadow(thingName=thing_name)
            shadow_data = json.loads(response['payload'].read())
            reported = shadow_data.get('state', {}).get('reported', {})

            servo_value = reported.get('servo', 0)
            if servo_value == 0:
                servo_state = "abierta"
            else:
                servo_state = "cerrada"

            fan_state = "encendido" if reported.get('fan', False) else "apagado"
            auto_state = "activado" if reported.get('autoMode', False) else "desactivado"

            interval_ms = reported.get('sensorInterval', 0)
            interval_seconds = interval_ms // 1000 if interval_ms else 0

            threshold_servo = reported.get('gasThresholdServo', 300)
            threshold_fan = reported.get('gasThresholdFan', 1000)

            speak_output = (
                f"Este es el estado actual del sistema. "
                f"La compuerta está {servo_state}. "
                f"El ventilador está {fan_state}. "
                f"El modo automático está {auto_state}. "
                f"Los datos se envían cada {interval_seconds} segundos. "
                f"El umbral de gas para la compuerta es {threshold_servo}, "
                f"y el umbral para el ventilador es {threshold_fan}."
            )

            reprompt_output = "¿Deseas realizar otra acción?"

        except Exception as e:
            logger.error(f"Error al consultar el estado: {str(e)}")
            speak_output = "Hubo un error al consultar el estado del sistema."
            reprompt_output = "¿Qué deseas hacer?"

        return (
            handler_input.response_builder
            .speak(speak_output)
            .ask(reprompt_output)
            .response
        )