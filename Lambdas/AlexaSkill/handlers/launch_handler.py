import ask_sdk_core.utils as ask_utils
from ask_sdk_core.dispatch_components import AbstractRequestHandler

from utils.logger import logger
from utils.alexa_utils import get_user_id_from_handler_input
from services.user_service import get_thing_name_from_user_id

class LaunchRequestHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_request_type("LaunchRequest")(handler_input)

    def handle(self, handler_input):
        try:
            session_attr = handler_input.attributes_manager.session_attributes

            user_id = get_user_id_from_handler_input(handler_input)
            if not user_id:
                speak_output = "No se pudo identificar tu usuario. Por favor intenta de nuevo."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            thing_name = get_thing_name_from_user_id(user_id)
            if not thing_name:
                speak_output = "No se encontró un dispositivo asociado a tu cuenta."
                return (
                    handler_input.response_builder
                    .speak(speak_output)
                    .ask(speak_output)
                    .response
                )

            session_attr["user_id"] = user_id
            session_attr["thing_name"] = thing_name

            logger.info(f"Sesión iniciada para {user_id} con thing {thing_name}")

            speak_output = (
                "Bienvenido al control de puerta y monitoreo de gas. "
                "Puedes decir abrir compuerta, cerrar compuerta, "
                "o consultar nivel de gas. ¿Qué deseas hacer?"
            )

            reprompt_output = "¿Qué deseas hacer?"

        except Exception as e:
            logger.error(f"Error en LaunchRequestHandler: {e}")
            speak_output = "Hubo un error al inicializar la sesión."
            reprompt_output = "¿Qué deseas hacer?"

        return (
            handler_input.response_builder
            .speak(speak_output)
            .ask(reprompt_output)
            .response
        )
