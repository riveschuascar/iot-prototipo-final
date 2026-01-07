{
    "interactionModel": {
        "languageModel": {
            "invocationName": "monitor gas",
            "intents": [
                {
                    "name": "AMAZON.CancelIntent",
                    "samples": []
                },
                {
                    "name": "AMAZON.HelpIntent",
                    "samples": []
                },
                {
                    "name": "AMAZON.StopIntent",
                    "samples": []
                },
                {
                    "name": "AMAZON.NavigateHomeIntent",
                    "samples": []
                },
                {
                    "name": "ControlServoIntent",
                    "slots": [
                        {
                            "name": "action",
                            "type": "ON_OFF_DEVICE",
                            "samples": [
                                "{action}"
                            ]
                        }
                    ],
                    "samples": [
                        "{action} el servo",
                        "{action} servo",
                        "{action} compuerta",
                        "{action} la compuerta"
                    ]
                },
                {
                    "name": "ControlFanIntent",
                    "slots": [
                        {
                            "name": "action",
                            "type": "ON_OFF_DEVICE",
                            "samples": [
                                "{action}"
                            ]
                        }
                    ],
                    "samples": [
                        "{action} el ventilador",
                        "{action} ventilador",
                        "{action} extractor",
                        "{action} el extractor"
                    ]
                },
                {
                    "name": "CheckGasLevelIntent",
                    "slots": [],
                    "samples": [
                        "dime el estado del gas",
                        "dime el nivel de gas",
                        "consulta el estado del gas",
                        "consulta el nivel de gas"
                    ]
                },
                {
                    "name": "AutomodeControlIntent",
                    "slots": [
                        {
                            "name": "mode",
                            "type": "ON_OFF_DEVICE",
                            "samples": [
                                "{mode}"
                            ]
                        }
                    ],
                    "samples": [
                        "{mode} el modo automatico",
                        "{mode} modo automatico"
                    ]
                },
                {
                    "name": "CheckStateIntent",
                    "slots": [],
                    "samples": [
                        "consulta el estado del dispositivo",
                        "consulta el estado actual",
                        "dime el estado del dispositivo",
                        "dime el estado actual"
                    ]
                },
                {
                    "name": "SetThresholdIntent",
                    "slots": [
                        {
                            "name": "device",
                            "type": "DEVICE_TYPES",
                            "samples": [
                                "{device}"
                            ]
                        },
                        {
                            "name": "value",
                            "type": "AMAZON.NUMBER",
                            "samples": [
                                "{value}"
                            ]
                        }
                    ],
                    "samples": [
                        "ajusta el umbral del {device} a {value}",
                        "configurar umbral de {device} a {value}",
                        "configura el umbral del {device} a {value}"
                    ]
                },
                {
                    "name": "ChangeIntervalIntent",
                    "slots": [
                        {
                            "name": "time",
                            "type": "AMAZON.NUMBER",
                            "samples": [
                                "{time} segundos",
                                "{time}",
                                "cada {time}",
                                "cada {time} segundos"
                            ]
                        }
                    ],
                    "samples": [
                        "ajusta el intervalo de envio",
                        "configura el periodo de envio",
                        "establece el intervalo de envio en {time} segundos",
                        "manda los datos cada {time} segundos",
                        "establece el intervalo en {time} segundos",
                        "configura el intervalo a {time} segundos",
                        "ajusta el intervalo a {time} segundos",
                        "cambia el intervalo a {time} segundos"
                    ]
                }
            ],
            "types": [
                {
                    "name": "DEVICE_TYPES",
                    "values": [
                        {
                            "name": {
                                "value": "ventilador",
                                "synonyms": [
                                    "abanico",
                                    "extractor"
                                ]
                            }
                        },
                        {
                            "name": {
                                "value": "servo",
                                "synonyms": [
                                    "puerta",
                                    "compuerta"
                                ]
                            }
                        }
                    ]
                },
                {
                    "name": "ON_OFF_DEVICE",
                    "values": [
                        {
                            "name": {
                                "value": "cerrar",
                                "synonyms": [
                                    "cierra"
                                ]
                            }
                        },
                        {
                            "name": {
                                "value": "abrir",
                                "synonyms": [
                                    "abre"
                                ]
                            }
                        },
                        {
                            "name": {
                                "value": "desactivar",
                                "synonyms": [
                                    "desactiva"
                                ]
                            }
                        },
                        {
                            "name": {
                                "value": "activar",
                                "synonyms": [
                                    "activa"
                                ]
                            }
                        }
                    ]
                }
            ]
        },
        "dialog": {
            "intents": [
                {
                    "name": "AutomodeControlIntent",
                    "confirmationRequired": false,
                    "prompts": {},
                    "slots": [
                        {
                            "name": "mode",
                            "type": "ON_OFF_DEVICE",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.290971359350.960688087221"
                            }
                        }
                    ]
                },
                {
                    "name": "SetThresholdIntent",
                    "confirmationRequired": false,
                    "prompts": {},
                    "slots": [
                        {
                            "name": "device",
                            "type": "DEVICE_TYPES",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.1731495083870.75216304621"
                            }
                        },
                        {
                            "name": "value",
                            "type": "AMAZON.NUMBER",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.1731495083870.1193792948755"
                            },
                            "validations": [
                                {
                                    "type": "isLessThanOrEqualTo",
                                    "prompt": "Slot.Validation.990629300564.222226204759.1310085312398",
                                    "value": "1000"
                                }
                            ]
                        }
                    ]
                },
                {
                    "name": "ControlServoIntent",
                    "confirmationRequired": false,
                    "prompts": {},
                    "slots": [
                        {
                            "name": "action",
                            "type": "ON_OFF_DEVICE",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.1039496689926.585883537396"
                            }
                        }
                    ]
                },
                {
                    "name": "ControlFanIntent",
                    "confirmationRequired": false,
                    "prompts": {},
                    "slots": [
                        {
                            "name": "action",
                            "type": "ON_OFF_DEVICE",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.272621632030.657325926319"
                            }
                        }
                    ]
                },
                {
                    "name": "ChangeIntervalIntent",
                    "confirmationRequired": false,
                    "prompts": {},
                    "slots": [
                        {
                            "name": "time",
                            "type": "AMAZON.NUMBER",
                            "confirmationRequired": false,
                            "elicitationRequired": true,
                            "prompts": {
                                "elicitation": "Elicit.Slot.1626012759122.852634404255"
                            },
                            "validations": [
                                {
                                    "type": "isLessThanOrEqualTo",
                                    "prompt": "Slot.Validation.988111060965.692436453712.1101817379443",
                                    "value": "60"
                                },
                                {
                                    "type": "isGreaterThanOrEqualTo",
                                    "prompt": "Slot.Validation.988111060965.692436453712.1006977905156",
                                    "value": "10"
                                }
                            ]
                        }
                    ]
                }
            ],
            "delegationStrategy": "ALWAYS"
        },
        "prompts": [
            {
                "id": "Slot.Validation.24553057302.997432201333.665022494290",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "puedes intentar. activa, activar, desactiva o desactivar modo automatico"
                    }
                ]
            },
            {
                "id": "Elicit.Slot.290971359350.960688087221",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Elige una accion. Activa o activar, desactiva o desactivar."
                    }
                ]
            },
            {
                "id": "Elicit.Slot.1731495083870.1193792948755",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Debes especificar un valor entre uno y mil"
                    }
                ]
            },
            {
                "id": "Elicit.Slot.1731495083870.75216304621",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Debes nombrar uno de los siguientes dispositivos. Servo o compuerta y ventilador o extractor."
                    }
                ]
            },
            {
                "id": "Elicit.Slot.1039496689926.585883537396",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Puedes decir abrir o cerrar para completar la accion."
                    }
                ]
            },
            {
                "id": "Slot.Validation.1039496689926.585883537396.1387399025530",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Las acciones posibles son. Abrir o abre y cerrar o cierra"
                    }
                ]
            },
            {
                "id": "Elicit.Slot.272621632030.657325926319",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Puedes decir activar o desactivar para completar la accion"
                    }
                ]
            },
            {
                "id": "Slot.Validation.272621632030.657325926319.481954701360",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "Las acciones posibles son. Activar o activa y desactivar o desactiva"
                    }
                ]
            },
            {
                "id": "Slot.Validation.990629300564.222226204759.1310085312398",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "El umbral debe ser menor o igual a mil."
                    }
                ]
            },
            {
                "id": "Elicit.Slot.1626012759122.852634404255",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "¿Cada cuántos segundos deseas enviar los datos?"
                    }
                ]
            },
            {
                "id": "Slot.Validation.988111060965.692436453712.1101817379443",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "El intervalo no puede superar los sesenta segundos."
                    }
                ]
            },
            {
                "id": "Slot.Validation.988111060965.692436453712.1006977905156",
                "variations": [
                    {
                        "type": "PlainText",
                        "value": "El intervalo no puede ser menor a diez segundos."
                    }
                ]
            }
        ]
    }
}