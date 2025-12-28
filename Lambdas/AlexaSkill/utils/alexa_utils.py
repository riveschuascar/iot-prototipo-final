from utils.logger import logger

def get_user_id_from_handler_input(handler_input):
    try:
        return handler_input.request_envelope.context.system.user.user_id
    except Exception as e:
        logger.error(f"Error obteniendo user_id: {e}")
        return None
