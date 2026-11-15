# Ligações

Esquemático do sistema completo (Uno com o sensor, ESP32-S3 com o OLED e divisor do MISO): [`docs/assembly/kicad/atmega328p_sensor_display.kicad_sch`](assembly/kicad/atmega328p_sensor_display.kicad_sch). O PDF exportado está em [`docs/assembly/atmega328p_sensor_display.pdf`](assembly/atmega328p_sensor_display.pdf).

## ATmega328P (Arduino Uno)

| Pino Uno | Função            | Destino                         | Observação                                      |
|----------|-------------------|---------------------------------|-------------------------------------------------|
| A0       | Entrada analógica | Saída do sensor de temperatura  | ADC de 10 bits, referência AVCC (5 V)           |
| 5V       | Alimentação       | VCC do sensor                   |                                                 |
| GND      | Terra             | GND do sensor e do ESP32-S3     |                                                 |
| D10      | SPI SS            | GPIO10 do ESP32-S3              | Interrupção de mudança de pino (PCINT2)         |
| D11      | SPI MOSI          | GPIO11 do ESP32-S3              |                                                 |
| D12      | SPI MISO          | GPIO13 do ESP32-S3 via divisor  | Ver [Link SPI](#link-spi-esp32-s3--atmega328p)  |
| D13      | SPI SCK           | GPIO12 do ESP32-S3              |                                                 |

### Decisões

- **A0 para o sensor:** primeiro canal analógico do ATmega328P.
- **Sem display:** o ATmega328P cuida só da aquisição. O display passou para o ESP32-S3, o que libera o 1 KB de RAM que o buffer da tela ocupava.
- **D10–D13 para o SPI:** são os únicos pinos do SPI de hardware do ATmega328P, que trabalha como periférico do ESP32-S3.

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
| 8    | I2C SDA  | SDA do OLED   |                                                             |
| 9    | I2C SCL  | SCL do OLED   |                                                             |
| 3V3  | Alimentação | VCC do OLED |                                                           |
| GND  | Terra    | GND do Uno e do OLED | **Obrigatório:** referência comum das duas placas    |

### Decisões

- **GPIO10–13:** são os pinos padrão do FSPI no Arduino-ESP32 para o S3. Ficam longe dos GPIO35–37, que o N16R8 usa para a PSRAM octal, e dos pinos de boot (GPIO0, 3, 45 e 46).
- **GPIO8/GPIO9:** são os pinos padrão do `Wire` (I2C) no ESP32-S3 e não conflitam com o SPI, a PSRAM ou os pinos de boot.
- **Alimentação separada:** cada placa é alimentada pelo próprio USB. Apenas o GND é compartilhado.

### Display OLED

- Controlador SSD1306, 128x64, endereço I2C `0x3C` (alguns módulos usam `0x3D`).
- Alimentado pelo pino 3V3 do ESP32-S3. O SSD1306 funciona em 3,3 V, e assim os pull-ups do módulo puxam SDA/SCL para 3,3 V, nível seguro para o ESP32-S3.
- A ordem dos pinos varia entre módulos. Confira a serigrafia antes de ligar.
- Os módulos Adafruit já têm resistores de pull-up no SDA/SCL. Módulos genéricos sem pull-up precisam de 4,7 kΩ para 3,3 V em cada linha.
- Se o display não responder, o firmware registra `SSD1306 not found` no serial e continua funcionando sem tela: SPI e MQTT não dependem dele.

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
