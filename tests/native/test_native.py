"""Compile/run the actual C++ protocol and component with deterministic UART/time."""

from pathlib import Path
import shutil
import subprocess
import pytest

ROOT = Path(__file__).resolve().parents[2]
COMPONENT = ROOT / "components" / "panasonic_heatpump"


@pytest.mark.parametrize("suite", ["protocol", "component", "energy"])
def test_native_cpp(suite, tmp_path):
    compiler = shutil.which("g++")
    if not compiler:
        pytest.fail("g++ is required for the native regression tests")
    output = tmp_path / (
        "regression.exe" if __import__("os").name == "nt" else "regression"
    )
    sources = [
        ROOT
        / "tests/native"
        / (
            "test_reliable_protocol.cpp"
            if suite == "protocol"
            else ("test_energy.cpp" if suite == "energy" else "test_component.cpp")
        ),
        COMPONENT / "commands.cpp",
        COMPONENT / "decode.cpp",
    ]
    command = [compiler, "-std=c++17", "-O0", "-g", "-Wall", "-Wextra", "-Werror"]
    if suite in ("component", "energy"):
        command += ["-Wno-unused-variable", "-I", str(ROOT / "tests/native/stubs")]
        sources += [COMPONENT / "panasonic_heatpump.cpp", COMPONENT / "helpers.cpp"]
    if suite == "energy":
        shutil.copytree(COMPONENT, tmp_path / "esphome/components/panasonic_heatpump")
        command += ["-I", str(tmp_path)]
        sources += [ROOT / "components/panasonic_heatpump_energy/energy_sensor.cpp"]
    command += [str(source) for source in sources] + ["-o", str(output)]
    built = subprocess.run(command, capture_output=True, text=True, timeout=120)
    assert built.returncode == 0, built.stdout + built.stderr
    run = [str(output)]
    if suite == "protocol":
        run += [
            str(ROOT / "tests/native/fixtures/k_standard.hex"),
            str(ROOT / "tests/native/fixtures/k_extra.hex"),
        ]
    result = subprocess.run(run, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stdout + result.stderr
