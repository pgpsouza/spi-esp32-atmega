"""Generates the KiCad 9 schematic of the system: Uno (sensor + SPI peripheral) and ESP32-S3 (SPI controller + OLED).

Usage, from the repository root:
    python tools/generate_schematic.py <kicad-symbols-dir>
    e.g. python tools/generate_schematic.py E:/kicad/share/kicad/symbols

The root sheet UUID is read from the existing schematic so the .kicad_pro keeps matching it.
Validate the result with:
    kicad-cli sch erc docs/assembly/kicad/atmega328p_sensor_display.kicad_sch
"""
import re
import sys
import uuid
from pathlib import Path

projectName = "atmega328p_sensor_display"
libraryName = "spi_esp32_atmega"
outputDir = Path(__file__).resolve().parent.parent / "docs" / "assembly" / "kicad"
kicadSymbolsDir = Path(sys.argv[1])
fontEffects = "(effects (font (size 1.27 1.27)))"
hiddenFontEffects = "(effects (font (size 1.27 1.27)) (hide yes))"


def read_root_uuid():
    """Returns the root sheet UUID of the existing schematic, or a new one if there is none."""
    schematicPath = outputDir / f"{projectName}.kicad_sch"
    if not schematicPath.exists():
        return str(uuid.uuid4())
    return re.search(r'\(uuid "([0-9a-f-]{36})"\)', schematicPath.read_text(encoding="utf-8")).group(1)


rootUuid = read_root_uuid()


def new_uuid():
    """Returns a random UUID for a schematic item."""
    return str(uuid.uuid4())


def prop(name, value, x=0, y=0, hide=False, angle=0):
    """Returns a symbol property field."""
    return f'(property "{name}" "{value}" (at {x} {y} {angle}) {hiddenFontEffects if hide else fontEffects})'


def pin(pinType, x, y, angle, name, number, length=5.08):
    """Returns a pin of a library symbol."""
    return (f'(pin {pinType} line (at {x} {y} {angle}) (length {length}) '
            f'(name "{name}" {fontEffects}) (number "{number}" {fontEffects}))')


def rect(x1, y1, x2, y2):
    """Returns the body rectangle of a library symbol."""
    return (f"(rectangle (start {x1} {y1}) (end {x2} {y2}) "
            f"(stroke (width 0.254) (type default)) (fill (type background)))")


def official_symbol(libraryFile, name, libraryId):
    """Copies a symbol from the KiCad library, renamed to its lib_id for embedding."""
    text = (kicadSymbolsDir / libraryFile).read_text(encoding="utf-8")
    start = text.index(f'\n\t(symbol "{name}"') + 2
    depth = 0
    for end in range(start, len(text)):
        depth += {"(": 1, ")": -1}.get(text[end], 0)
        if depth == 0:
            break
    return text[start:end + 1].replace(f'(symbol "{name}"', f'(symbol "{libraryId}:{name}"', 1)


customSymbols = {
    "Arduino_UNO_R3_Subset": dict(
        ref="U", desc="Arduino Uno R3 (ATmega328P) - only the pins used by this assembly",
        refPos=(0, 21.59), valPos=(0, -21.59), body=[rect(-10.16, 15.24, 10.16, -15.24)],
        pins=[
            ("power_out", 0, 20.32, 270, "5V", "1"),
            ("power_out", 0, -20.32, 90, "GND", "2"),
            ("input", -15.24, 7.62, 0, "A0", "3"),
            ("input", 15.24, 7.62, 180, "D10/SS", "6"),
            ("input", 15.24, 5.08, 180, "D11/MOSI", "7"),
            ("input", 15.24, 2.54, 180, "D13/SCK", "9"),
            ("output", 15.24, 0, 180, "D12/MISO", "8"),
        ]),
    "ESP32_S3_DevKitC_Subset": dict(
        ref="U", desc="ESP32-S3-DevKitC-1 N16R8 - only the pins used by this assembly (3.3 V logic)",
        refPos=(0, 13.97), valPos=(0, -21.59), body=[rect(-10.16, 10.16, 10.16, -12.7)],
        pins=[
            ("power_out", 0, 15.24, 270, "3V3", "6"),
            ("power_in", 0, -17.78, 90, "GND", "5"),
            ("output", -15.24, 7.62, 0, "GPIO10/CS", "1"),
            ("output", -15.24, 5.08, 0, "GPIO11/MOSI", "2"),
            ("output", -15.24, 2.54, 0, "GPIO12/SCK", "3"),
            ("input", -15.24, 0, 0, "GPIO13/MISO", "4"),
            ("bidirectional", 15.24, 5.08, 180, "GPIO8/SDA", "7"),
            ("output", 15.24, 2.54, 180, "GPIO9/SCL", "8"),
        ]),
    "OLED_SSD1306_I2C": dict(
        ref="J", desc="SSD1306 128x64 OLED module, I2C (pin 1..4 = GND VCC SCL SDA)",
        refPos=(-1.27, 6.35), valPos=(-1.27, -8.89), body=[rect(-7.62, 5.08, 5.08, -7.62)],
        pins=[
            ("power_in", 10.16, 2.54, 180, "VCC", "2"),
            ("bidirectional", 10.16, 0, 180, "SDA", "4"),
            ("input", 10.16, -2.54, 180, "SCL", "3"),
            ("power_in", 10.16, -5.08, 180, "GND", "1"),
        ]),
    "Analog_Sensor_3pin": dict(
        ref="J", desc="Analog temperature sensor connector (pin 1..3 = VCC OUT GND)",
        refPos=(-1.27, 6.35), valPos=(-1.27, -6.35), body=[rect(-7.62, 5.08, 5.08, -5.08)],
        pins=[
            ("power_in", 10.16, 2.54, 180, "VCC", "1"),
            ("output", 10.16, 0, 180, "OUT", "2"),
            ("power_in", 10.16, -2.54, 180, "GND", "3"),
        ]),
}


def custom_symbol(name, definition, prefix):
    """Returns a project symbol, prefixed with its library name when embedded in the schematic."""
    unit = (f'(symbol "{name}_1_1" ' + " ".join(definition["body"]) + " "
            + " ".join(pin(*p) for p in definition["pins"]) + ")")
    return (f'(symbol "{prefix}{name}" (pin_names (offset 1.016)) (exclude_from_sim no) (in_bom yes) (on_board yes) '
            + prop("Reference", definition["ref"], *definition["refPos"]) + " "
            + prop("Value", name, *definition["valPos"]) + " "
            + prop("Footprint", "", hide=True) + " " + prop("Datasheet", "", hide=True) + " "
            + prop("Description", definition["desc"], hide=True) + " " + unit + ")")


items = []
powerCount = 0


def place(libId, ref, value, x, y, pinNumbers, footprint="", angle=0, refOffset=(0, 0), valOffset=(0, 0),
          isPower=False, fieldAngle=0):
    """Places a symbol instance on the sheet."""
    pins = " ".join(f'(pin "{n}" (uuid "{new_uuid()}"))' for n in pinNumbers)
    inBom = "no" if isPower else "yes"
    items.append(
        f'(symbol (lib_id "{libId}") (at {x} {y} {angle}) (unit 1) (exclude_from_sim no) '
        f'(in_bom {inBom}) (on_board {inBom}) (dnp no) (uuid "{new_uuid()}") '
        + prop("Reference", ref, round(x + refOffset[0], 2), round(y + refOffset[1], 2), hide=isPower, angle=fieldAngle) + " "
        + prop("Value", value, round(x + valOffset[0], 2), round(y + valOffset[1], 2), angle=fieldAngle) + " "
        + prop("Footprint", footprint, x, y, hide=True) + " " + prop("Datasheet", "", x, y, hide=True) + " "
        + prop("Description", "", x, y, hide=True) + " " + pins + " "
        + f'(instances (project "{projectName}" (path "/{rootUuid}" (reference "{ref}") (unit 1)))))')


def power(name, x, y):
    """Places a power symbol (+5V, +3V3 or GND) whose pin is at (x, y)."""
    global powerCount
    powerCount += 1
    valOffset = (0, -3.81) if name in ("+5V", "+3V3") else (0, 5.08)
    place(f"power:{name}", f"#PWR{powerCount:02d}", name, x, y, ["1"], valOffset=valOffset, isPower=True)


def resistor(ref, value, x, y, horizontal):
    """Places a resistor; horizontal ones have their pins at x - 3.81 and x + 3.81, vertical ones at y - 3.81 and y + 3.81."""
    offsets = ((0, -2.54), (0, 2.54)) if horizontal else ((2.54, 0), (-2.54, 0))
    place("Device:R", ref, value, x, y, ["1", "2"],
          footprint="Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",
          angle=90 if horizontal else 0, refOffset=offsets[0], valOffset=offsets[1],
          fieldAngle=0 if horizontal else 90)


def wire(*points):
    """Draws a wire through the given points."""
    for (x1, y1), (x2, y2) in zip(points, points[1:]):
        items.append(f'(wire (pts (xy {x1} {y1}) (xy {x2} {y2})) (stroke (width 0) (type default)) (uuid "{new_uuid()}"))')


def junction(x, y):
    """Draws a junction dot where three or more wires meet."""
    items.append(f'(junction (at {x} {y}) (diameter 0) (color 0 0 0 0) (uuid "{new_uuid()}"))')


def label(name, x, y, pointsLeft=False):
    """Places a net label; labels with the same name are connected."""
    angle, justify = (180, "right bottom") if pointsLeft else (0, "left bottom")
    items.append(f'(label "{name}" (at {x} {y} {angle}) (effects (font (size 1.27 1.27)) (justify {justify})) (uuid "{new_uuid()}"))')


def text(body, x, y):
    """Places a note anchored by its top-left corner, so extra lines grow downward."""
    items.append(f'(text "{body}" (exclude_from_sim no) (at {x} {y} 0) '
                 f'(effects (font (size 1.27 1.27)) (justify left top)) (uuid "{new_uuid()}"))')


def draw_uno():
    """Draws the Uno (U1) with its power symbols and the sensor net on A0."""
    place(f"{libraryName}:Arduino_UNO_R3_Subset", "U1", "Arduino_UNO_R3", 127, 101.6,
          [p[5] for p in customSymbols["Arduino_UNO_R3_Subset"]["pins"]], footprint="Module:Arduino_UNO_R3",
          refOffset=(2.54, -17.78), valOffset=(2.54, 17.78))
    power("+5V", 127, 81.28)
    power("GND", 127, 121.92)
    wire((111.76, 93.98), (104.14, 93.98))
    label("SENSOR_OUT", 104.14, 93.98, pointsLeft=True)


def draw_sensor():
    """Draws the analog sensor connector (J2), powered from the Uno 5V rail."""
    place(f"{libraryName}:Analog_Sensor_3pin", "J2", "Analog_Temp_Sensor", 63.5, 106.68, ["1", "2", "3"],
          footprint="Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical",
          refOffset=(-1.27, -6.35), valOffset=(-1.27, 6.35))
    wire((73.66, 104.14), (78.74, 104.14), (78.74, 99.06))
    power("+5V", 78.74, 99.06)
    wire((73.66, 106.68), (86.36, 106.68))
    label("SENSOR_OUT", 86.36, 106.68)
    wire((73.66, 109.22), (78.74, 109.22), (78.74, 114.3))
    power("GND", 78.74, 114.3)


def draw_esp32():
    """Draws the ESP32-S3 (U2): SPI on the left, wired straight to the Uno, I2C labels on the right."""
    place(f"{libraryName}:ESP32_S3_DevKitC_Subset", "U2", "ESP32-S3-DevKitC-1_N16R8", 215.9, 101.6,
          [p[5] for p in customSymbols["ESP32_S3_DevKitC_Subset"]["pins"]],
          refOffset=(5.08, -13.97), valOffset=(2.54, 15.24))
    power("+3V3", 215.9, 86.36)
    power("GND", 215.9, 119.38)
    for y, name in [(93.98, "SPI_CS"), (96.52, "SPI_MOSI"), (99.06, "SPI_SCK")]:
        wire((142.24, y), (200.66, y))
        label(name, 172.72, y)
    wire((231.14, 96.52), (238.76, 96.52))
    label("SDA", 238.76, 96.52)
    wire((231.14, 99.06), (238.76, 99.06))
    label("SCL", 238.76, 99.06)


def draw_miso_divider():
    """Draws the MISO divider: Uno D12 (5 V) -> R1 1k -> ESP32 GPIO13, with R2 2k to GND."""
    resistor("R1", "1k", 157.48, 101.6, horizontal=True)
    wire((142.24, 101.6), (153.67, 101.6))
    wire((161.29, 101.6), (167.64, 101.6))
    wire((167.64, 101.6), (200.66, 101.6))
    label("SPI_MISO_3V3", 172.72, 101.6)
    junction(167.64, 101.6)
    resistor("R2", "2k", 167.64, 109.22, horizontal=False)
    wire((167.64, 101.6), (167.64, 105.41))
    wire((167.64, 113.03), (167.64, 116.84))
    power("GND", 167.64, 116.84)


def draw_oled():
    """Draws the OLED module (J1), powered from the ESP32-S3 3V3 rail."""
    place(f"{libraryName}:OLED_SSD1306_I2C", "J1", "OLED_SSD1306_128x64_I2C", 243.84, 71.12, ["1", "2", "3", "4"],
          footprint="Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical",
          refOffset=(-1.27, -6.35), valOffset=(-1.27, 8.89))
    wire((254.0, 68.58), (259.08, 68.58), (259.08, 63.5))
    power("+3V3", 259.08, 63.5)
    wire((254.0, 71.12), (266.7, 71.12))
    label("SDA", 266.7, 71.12)
    wire((254.0, 73.66), (266.7, 73.66))
    label("SCL", 266.7, 73.66)
    wire((254.0, 76.2), (259.08, 76.2), (259.08, 81.28))
    power("GND", 259.08, 81.28)


def draw_notes():
    """Writes the design notes shown on the sheet."""
    text("SPI: modo 0, 500 kHz. ESP32-S3 = controlador, ATmega328P = periferico.\\n"
         "Divisor R1/R2 no MISO: 5 V -> 3,33 V (ESP32-S3 nao tolera 5 V).\\n"
         "CS, MOSI e SCK diretos: 3,3 V >= 0,6 x VCC = 3,0 V do ATmega (margem de 0,3 V).\\n"
         "GND comum entre as placas e obrigatorio; cada placa alimentada pelo proprio USB.", 142.24, 129.54)
    text("OLED no ESP32-S3: I2C em GPIO8 (SDA) e GPIO9 (SCL), alimentado em 3,3 V.\\n"
         "Conferir a ordem dos pinos na serigrafia do modulo e o endereco I2C (0x3C).", 50.8, 129.54)
    text("Sensor: curva de conversao ADC -> temperatura a definir no firmware.", 50.8, 137.16)


def build_schematic():
    """Returns the full schematic text, with every symbol it uses embedded."""
    librarySymbols = ("(lib_symbols "
                      + " ".join(custom_symbol(n, d, f"{libraryName}:") for n, d in customSymbols.items()) + " "
                      + official_symbol("Device.kicad_sym", "R", "Device") + " "
                      + official_symbol("power.kicad_sym", "+5V", "power") + " "
                      + official_symbol("power.kicad_sym", "+3V3", "power") + " "
                      + official_symbol("power.kicad_sym", "GND", "power") + ")")
    return (f'(kicad_sch (version 20250114) (generator "eeschema") (generator_version "9.0") '
            f'(uuid "{rootUuid}") (paper "A4") '
            f'(title_block (title "Link SPI ESP32-S3 <-> ATmega328P com sensor e OLED") (date "2026-11-14") (rev "3") '
            f'(company "IFES Serra - Comunicacao de Dados") '
            f'(comment 1 "ESP32-S3: controlador SPI, gateway MQTT e display; ATmega328P: aquisicao")) '
            + librarySymbols + " " + " ".join(items) + " "
            + '(sheet_instances (path "/" (page "1"))) (embedded_fonts no))\n')


def build_symbol_library():
    """Returns the project symbol library referenced by sym-lib-table."""
    return ('(kicad_symbol_lib (version 20241209) (generator "kicad_symbol_editor") (generator_version "9.0") '
            + " ".join(custom_symbol(n, d, "") for n, d in customSymbols.items()) + ")\n")


def main():
    """Draws the sheet and writes the schematic and the project symbol library."""
    draw_uno()
    draw_sensor()
    draw_esp32()
    draw_miso_divider()
    draw_oled()
    draw_notes()
    (outputDir / f"{projectName}.kicad_sch").write_text(build_schematic(), encoding="utf-8")
    (outputDir / f"{libraryName}.kicad_sym").write_text(build_symbol_library(), encoding="utf-8")
    print(f"written to {outputDir}")


if __name__ == "__main__":
    main()
