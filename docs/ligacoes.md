# Ligações

Esquemático do sistema completo (Uno, OLED, sensor, ESP32-S3 e divisor do MISO): [`docs/assembly/kicad/atmega328p_sensor_display.kicad_sch`](assembly/kicad/atmega328p_sensor_display.kicad_sch). O PDF exportado está em [`docs/assembly/atmega328p_sensor_display.pdf`](assembly/atmega328p_sensor_display.pdf).

## ATmega328P (Arduino Uno)

| Pino Uno | Função            | Destino                         | Observação                                      |
|----------|-------------------|---------------------------------|-------------------------------------------------|
| A0       | Entrada analógica | Saída do sensor de temperatura  | ADC de 10 bits, referência AVCC (5 V)           |
| A4/SDA   | I2C SDA           | SDA do OLED SSD1306             | Pino fixo do TWI do ATmega328P                  |
| A5/SCL   | I2C SCL           | SCL do OLED SSD1306             | Pino fixo do TWI do ATmega328P                  |
| 5V       | Alimentação       | VCC do OLED e do sensor         |                                                 |
| GND      | Terra             | GND do OLED e do sensor         |                                                 |
| D10      | SPI SS            | GPIO10 do ESP32-S3              | Interrupção de mudança de pino (PCINT2)         |
| D11      | SPI MOSI          | GPIO11 do ESP32-S3              |                                                 |
| D12      | SPI MISO          | GPIO13 do ESP32-S3 via divisor  | Ver [Link SPI](#link-spi-esp32-s3--atmega328p)  |
| D13      | SPI SCK           | GPIO12 do ESP32-S3              |                                                 |

### Decisões

- **A0 para o sensor:** A4 e A5 são compartilhados com o I2C, então o sensor fica no primeiro canal analógico livre.
- **I2C em A4/A5:** são os únicos pinos do periférico TWI de hardware no ATmega328P.
- **D10–D13 para o SPI:** são os únicos pinos do SPI de hardware do ATmega328P, que trabalha como periférico do ESP32-S3.

### Display OLED

- Controlador SSD1306, 128x64, endereço I2C `0x3C` (alguns módulos usam `0x3D`).
- A ordem dos pinos varia entre módulos. Confira a serigrafia antes de ligar.
- Os módulos Adafruit já têm resistores de pull-up no SDA/SCL. Módulos genéricos sem pull-up precisam de 4,7 kΩ para 5 V em cada linha.
- O driver aloca 1 KB de RAM em tempo de execução para o buffer da tela, cerca de metade dos 2 KB do ATmega328P.

### Sensor de temperatura

- Conector de 3 pinos: VCC, OUT, GND.
- A conversão ADC → temperatura ainda é um placeholder linear (0 a 1023 → 0 a 100 °C) em `atmega328p/src/main.cpp`. Ela será substituída quando a curva do sensor for definida.

## ESP32-S3 N16R8

| GPIO | Função   | Destino       | Observação                                                  |
|------|----------|---------------|-------------------------------------------------------------|
| 10   | SPI CS   | D10 do Uno    | Controlado manualmente pelo firmware (ativo em nível baixo)  |
| 11   | SPI MOSI | D11 do Uno    |                                                             |
| 12   | SPI SCK  | D13 do Uno    |                                                             |
| 13   | SPI MISO | D12 do Uno    | **Somente através do divisor resistivo**                    |
| GND  | Terra    | GND do Uno    | **Obrigatório:** referência comum das duas placas           |

### Decisões

- **GPIO10–13:** são os pinos padrão do FSPI no Arduino-ESP32 para o S3. Ficam longe dos GPIO35–37, que o N16R8 usa para a PSRAM octal, e dos pinos de boot (GPIO0, 3, 45 e 46).
- **Alimentação separada:** cada placa é alimentada pelo próprio USB. Apenas o GND é compartilhado.

## Link SPI ESP32-S3 ↔ ATmega328P

Parâmetros do barramento: modo 0, 500 kHz, MSB primeiro. Detalhes em [`protocolo_spi.md`](protocolo_spi.md).

O ESP32-S3 trabalha em 3,3 V e **não tolera 5 V** nas entradas. O Uno trabalha em 5 V.

```
Uno D12 (MISO, 5 V) ──[ R1 1 kΩ ]──┬──> ESP32-S3 GPIO13 (≈3,33 V)
                                    │
                                [ R2 2 kΩ ]
                                    │
                                   GND
```

| Linha | Sentido | Adaptação | Por quê |
|---|---|---|---|
| MISO | Uno → ESP32 | Divisor 1 kΩ / 2 kΩ | 5 V × 2/3 ≈ 3,33 V, dentro do limite do ESP32-S3 |
| MOSI, SCK, SS | ESP32 → Uno | Ligação direta | O ATmega a 5 V lê nível alto a partir de 0,6 × VCC = 3,0 V; os 3,3 V do ESP32 passam com cerca de 0,3 V de margem |

Cuidados:

- **Margem pequena nas linhas diretas.** Com fios longos ou ruído, a margem de 0,3 V pode não bastar. Nesse caso, troque por um conversor de nível (por exemplo, BSS138).
- **O LED do pino D13 do Uno** fica na linha SCK. Ele adiciona carga, mas não atrapalha a 500 kHz.
- **Ordem de ligação:** ligue o GND entre as placas antes das linhas de sinal.
