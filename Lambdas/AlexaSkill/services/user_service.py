from utils.logger import logger
from services.aws_clients import USER_THING_TABLE

def get_thing_name_from_user_id(user_id):
    try:
        response = USER_THING_TABLE.get_item(Key={"user_id": user_id})
        return response.get("Item", {}).get("thing_name")
    except Exception as e:
        logger.error(f"Error consultando DynamoDB: {e}")
        return None
