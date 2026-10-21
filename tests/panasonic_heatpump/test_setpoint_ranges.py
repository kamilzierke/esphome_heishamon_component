"""Validate the real schema range checks, without mocking code generation."""

from pathlib import Path
import sys
import math
import pytest
import esphome.config_validation as cv

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "components"))
from panasonic_heatpump.number import _validate_ranges


@pytest.mark.parametrize(
    "low,high,step",
    [
        (-12, -2, 1),
        (-10, -2, 1),
        (-8, 0, 1),
        (64, 254, 1),
        (10, 60, 1),
    ],
)
def test_valid_protocol_ranges(low, high, step):
    key = "set15" if high == 254 else "set20"
    config = {key: {"min_value": low, "max_value": high, "step": step}}
    assert _validate_ranges(config) is config


@pytest.mark.parametrize(
    "low,high,step",
    [
        (1, 0, 1),
        (0, 2, 0),
        (0, 2, 0.5),
        (math.nan, 1, 1),
        (0, math.inf, 1),
        (0, 2, math.inf),
    ],
)
def test_invalid_protocol_ranges(low, high, step):
    with pytest.raises(cv.Invalid):
        _validate_ranges({"set20": {"min_value": low, "max_value": high, "step": step}})


@pytest.mark.parametrize(
    "low,high,step",
    [(1, 0, 1), (0, 2, 0.5), (0, math.inf, 1), (0, 2, math.nan), (-128, 20, 1)],
)
def test_invalid_climate_ranges(low, high, step):
    from panasonic_heatpump.climate import _validate_climate_ranges

    with pytest.raises(cv.Invalid):
        _validate_climate_ranges(
            {
                "tank": {
                    "min_temperature": low,
                    "max_temperature": high,
                    "temperature_step": step,
                }
            }
        )


def test_number_encoding_overflow():
    with pytest.raises(cv.Invalid):
        _validate_ranges({"set11": {"min_value": 40, "max_value": 1000, "step": 1}})
