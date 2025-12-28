from ask_sdk_core.skill_builder import SkillBuilder
from ask_sdk_core.dispatch_components import AbstractExceptionHandler
from utils.logger import logger

# Handlers
from handlers.automode_control_handler import AutomodeControlIntentHandler
from handlers.built_in_handlers import HelpIntentHandler, CancelOrStopIntentHandler, SessionEndedRequestHandler
from handlers.change_interval_handler import ChangeIntervalIntentHandler
from handlers.check_gas_level_handler import CheckGasLevelIntentHandler
from handlers.check_state_handler import CheckStateIntentHandler
from handlers.control_fan_handler import ControlFanIntentHandler
from handlers.control_servo_handler import ControlServoIntentHandler
from handlers.launch_handler import LaunchRequestHandler
from handlers.set_threshold_handler import SetThresholdIntentHandler


# Exception handler
class CatchAllExceptionHandler(AbstractExceptionHandler):
    def can_handle(self, handler_input, exception):
        return True

    def handle(self, handler_input, exception):
        logger.error(f"Unhandled exception: {exception}")

        return (
            handler_input.response_builder
            .speak("Ocurrió un error inesperado. Por favor intenta de nuevo.")
            .ask("¿Qué deseas hacer?")
            .response
        )

sb = SkillBuilder()

# Add handlers
sb.add_request_handler(AutomodeControlIntentHandler())
sb.add_request_handler(HelpIntentHandler())
sb.add_request_handler(CancelOrStopIntentHandler())
sb.add_request_handler(SessionEndedRequestHandler())
sb.add_request_handler(ChangeIntervalIntentHandler())
sb.add_request_handler(CheckGasLevelIntentHandler())
sb.add_request_handler(CheckStateIntentHandler())
sb.add_request_handler(ControlFanIntentHandler())
sb.add_request_handler(ControlServoIntentHandler())
sb.add_request_handler(LaunchRequestHandler())
sb.add_request_handler(SetThresholdIntentHandler())

# Add exception handler
sb.add_exception_handler(CatchAllExceptionHandler())

# Lambda entrypoint
lambda_handler = sb.lambda_handler()
