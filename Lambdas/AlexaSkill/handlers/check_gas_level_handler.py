import json
import ask_sdk_core.utils as ask_utils

from ask_sdk_core.dispatch_components import AbstractRequestHandler
from services.aws_clients import iot_client
from utils.logger import logger

class CheckGasLevelIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("CheckGasLevelIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = handler_input.attributes_manager.session_attributes.get('thing_name')
        try:
            if not thing_name:
                speak_output = "Sesión no inicializada. Por favor vuelve a empezar."
                return handler_input.response_builder.speak(speak_output).ask(speak_output).response

            response = iot_client.get_thing_shadow(thingName=thing_name)
            shadow_data = json.loads(response['payload'].read())

            reported = shadow_data.get('state', {}).get('reported', {})

            gas_level = reported.get('gasLevel', 0)
            alert = reported.get('alert', 'DESCONOCIDO')

            if alert == "SEGURO":
                speak_output = f"El ambiente es seguro. El nivel de gas es {gas_level} PPM."
            elif alert == "GAS_DETECTADO":
                speak_output = f"Se ha detectado gas. El nivel actual es {gas_level} PPM."
            elif alert == "ADVERTENCIA":
                speak_output = f"Advertencia. El nivel de gas es {gas_level} PPM. Se recomienda ventilar el área."
            elif alert == "PELIGRO":
                speak_output = f"Peligro. El nivel de gas es alto, {gas_level} PPM. Actúa de inmediato."
            elif alert == "RIESGO_EXPLOSION":
                speak_output = f"Existe riesgo de explosión. Nivel de gas {gas_level} PPM."
            elif alert == "CRITICO":
                speak_output = f"Nivel de gas crítico. Evacúa inmediatamente el area."

            reprompt_output = "¿Deseas realizar otra acción?"

        except Exception as e:
            logger.error(f"Error al consultar el nivel de gas: {str(e)}")
            speak_output = "Hubo un error al consultar el nivel de gas."
            reprompt_output = "¿Qué deseas hacer?"

        return (
            handler_input.response_builder
                .speak(speak_output)
                .ask(reprompt_output)
                .response
        )
