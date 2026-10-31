# Ligações

## ATmega328P (Arduino Uno)

Esquemático KiCad: [`docs/assembly/kicad/atmega328p_sensor_display.kicad_sch`](assembly/kicad/atmega328p_sensor_display.kicad_sch) — PDF exportado em [`docs/assembly/atmega328p_sensor_display.pdf`](assembly/atmega328p_sensor_display.pdf).

| Pino Uno | Função            | Destino                         | Observação                                      |
|----------|-------------------|---------------------------------|-------------------------------------------------|
| A0       | Entrada analógica | Saída do sensor de temperatura  | ADC de 10 bits, referência AVCC (5 V)           |
| A4/SDA   | I2C SDA           | SDA do OLED SSD1306             | Pino fixo do TWI do ATmega328P                  |
| A5/SCL   | I2C SCL           | SCL do OLED SSD1306             | Pino fixo do TWI do ATmega328P                  |
| 5V       | Alimentação       | VCC do OLED e do sensor         |                                                 |
| GND      | Terra             | GND do OLED e do sensor         |                                                 |
| D10      | SPI SS            | Reservado (ESP32-S3)            | Não usar para outras funções                    |
| D11      | SPI MOSI          | Reservado (ESP32-S3)            | Não usar para outras funções                    |
| D12      | SPI MISO          | Reservado (ESP32-S3)            | Não usar para outras funções                    |
| D13      | SPI SCK           | Reservado (ESP32-S3)            | Não usar para outras funções                    |

### Decisões

- **A0 para o sensor:** A4 e A5 são compartilhados com o I2C, então o sensor fica no primeiro canal analógico livre.
- **I2C em A4/A5:** são os únicos pinos do periférico TWI de hardware no ATmega328P.
- **D10–D13 reservados:** são os pinos do SPI de hardware e serão usados no link com o ESP32-S3. O ESP32-S3 trabalha em 3,3 V e o Uno em 5 V, então as linhas SPI precisarão de conversor de nível.

### Display OLED

- Controlador SSD1306, 128x64, endereço I2C `0x3C` (alguns módulos usam `0x3D`).
- A ordem dos pinos varia entre módulos. Confira a serigrafia antes de ligar.
- Os módulos Adafruit já têm resistores de pull-up no SDA/SCL. Módulos genéricos sem pull-up precisam de 4,7 kΩ para 5 V em cada linha.
- O driver aloca 1 KB de RAM em tempo de execução para o buffer da tela, cerca de metade dos 2 KB do ATmega328P.

### Sensor de temperatura

- Conector de 3 pinos: VCC, OUT, GND.
- A conversão ADC → temperatura ainda é um placeholder linear (0 a 1023 → 0 a 100 °C) em `atmega328p/src/main.cpp`. Ela será substituída quando a curva do sensor for definida.
