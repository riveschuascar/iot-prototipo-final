import boto3

IOT_REGION = "us-west-1"

iot_client = boto3.client("iot-data", region_name=IOT_REGION)
dynamodb = boto3.resource("dynamodb", region_name=IOT_REGION)

USER_THING_TABLE = dynamodb.Table("user_thing")
