import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CODEOWNERS = ["@elvit"]
DEPENDENCIES = ["uart"]
MULTICONF = True

CONF_PANASONIC_HEATPUMP_ID = "panasonic_heatpump"
CONF_UART_CLIENT = "uart_client_id"
CONF_UART_CLIENT_TIMEOUT = "uart_client_timeout"
CONF_LOG_UART_MSG = "log_uart_msg"
CONF_MODEL_PROFILE = "model_profile"

panasonic_heatpump_ns = cg.esphome_ns.namespace("panasonic_heatpump")
PanasonicHeatpumpComponent = panasonic_heatpump_ns.class_(
    "PanasonicHeatpumpComponent", cg.PollingComponent, uart.UARTDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PanasonicHeatpumpComponent),
            cv.Optional(CONF_UART_CLIENT): cv.use_id(uart.UARTComponent),
            cv.Optional(
                CONF_UART_CLIENT_TIMEOUT, default="10000ms"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_LOG_UART_MSG, default=False): cv.boolean,
            cv.Optional(CONF_MODEL_PROFILE, default="generic"): cv.one_of(
                "generic", "k_7kw"
            ),
            cv.Optional(
                "response_timeout", default="2s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional("data_timeout"): cv.positive_time_period_milliseconds,
            cv.Optional("command_timeout"): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.polling_component_schema("3s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


# Adapted from ElVit/esphome_components PR #39.
FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "panasonic_heatpump",
    baud_rate=9600,
    data_bits=8,
    parity="EVEN",
    stop_bits=1,
    require_tx=True,
    require_rx=True,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_UART_CLIENT in config:
        uart_client = await cg.get_variable(config[CONF_UART_CLIENT])
        cg.add(var.set_uart_client(uart_client))
        cg.add(var.set_uart_client_timeout(config[CONF_UART_CLIENT_TIMEOUT]))

    cg.add(var.set_log_uart_msg(config[CONF_LOG_UART_MSG]))
    cg.add(var.set_model_profile(config[CONF_MODEL_PROFILE] == "k_7kw"))
    response_ms = config["response_timeout"].total_milliseconds
    cg.add(var.set_response_timeout(response_ms))
    default_age = max(3 * config["update_interval"].total_milliseconds, 2 * response_ms)
    for option in ("data_timeout", "command_timeout"):
        milliseconds = (
            config[option].total_milliseconds if option in config else default_age
        )
        cg.add(getattr(var, "set_" + option)(milliseconds))
