import math
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, time
from esphome.const import (
    CONF_ID,
    CONF_TIME_ID,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_KILOWATT_HOURS,
    CONF_RESTORE,
)
from esphome.components.panasonic_heatpump import PanasonicHeatpumpComponent

from esphome.components.panasonic_heatpump.sensor import PanasonicHeatpumpSensor


def _finite_energy(value):
    value = cv.float_range(min=0)(value)
    if not math.isfinite(value):
        raise cv.Invalid("initial_energy must be finite")
    return value


CONF_POWER_ID = "power_id"

DEPENDENCIES = ["panasonic_heatpump", "time"]
energy_ns = cg.esphome_ns.namespace("panasonic_heatpump_energy")
DailyEnergy = energy_ns.class_("DailyEnergy", sensor.Sensor, cg.Component)
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        DailyEnergy,
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    )
    .extend(
        {
            cv.Required("heatpump_id"): cv.use_id(PanasonicHeatpumpComponent),
            cv.Required(CONF_POWER_ID): cv.use_id(PanasonicHeatpumpSensor),
            cv.GenerateID(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
            cv.Optional(
                "max_sample_gap", default="15s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional("initial_energy", default=0): _finite_energy,
            cv.Optional(CONF_RESTORE, default=True): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    cg.add(var.set_power(await cg.get_variable(config[CONF_POWER_ID])))
    cg.add(var.set_heatpump(await cg.get_variable(config["heatpump_id"])))
    cg.add(var.set_time(await cg.get_variable(config[CONF_TIME_ID])))
    cg.add(var.set_max_gap(config["max_sample_gap"]))
    cg.add(var.set_initial_energy(config["initial_energy"]))
    cg.add(var.set_restore(config[CONF_RESTORE]))
