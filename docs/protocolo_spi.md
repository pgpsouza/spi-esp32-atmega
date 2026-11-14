# Protocolo SPI

Contrato entre o **ESP32-S3** (controlador, embarcado A) e o **ATmega328P** (periférico, embarcado B). A fonte da verdade é [`common/protocol.h`](../common/protocol.h). Qualquer mudança nele deve atualizar este documento no mesmo commit.

## Parâmetros do barramento

| Parâmetro | Valor | Motivo |
|---|---|---|
| Versão do protocolo | `1` | Enviada em todo quadro; o ESP32 descarta versões diferentes |
| Modo SPI | 0 (CPOL=0, CPHA=0), MSB primeiro | Padrão do hardware SPI do ATmega328P |
| Clock | 500 kHz | Folga para o ATmega atender cada byte por interrupção |
| Setup após SS baixo | 10 µs | Tempo para o periférico perceber a seleção |
| Intervalo entre bytes | 40 µs | O ATmega carrega o próximo byte na ISR; cobre a latência das interrupções de I2C e `millis()` |
| Ordem dos bytes | Little-endian | Ordem nativa dos dois MCUs |

## Fluxo

O SPI só permite que o controlador inicie a comunicação. Por isso o ESP32 faz **polling** a cada 1 s, e o ATmega numera cada amostra. O ESP32 só publica no MQTT quando o número de sequência muda.

```
ATmega: a cada 30 s gera uma amostra nova (sequence + 1)

ESP32 (a cada 1 s)            ATmega
  SS ↓
  0xA1 (ReadSample)  ───────>  ISR: copia a amostra atual e carrega o 1º byte
  0x00 ×8            <───────  8 bytes do SampleFrame
  SS ↑                         ISR de SS: volta a aguardar comando

ESP32: quadro válido e sequence diferente do anterior -> publica no MQTT
```

O ATmega copia a amostra para um buffer no momento em que recebe o comando. Assim, uma amostra nova gerada no meio de uma transação não mistura bytes de duas amostras.

## Comandos

| Comando | Código | Resposta |
|---|---|---|
| `ReadSample` | `0xA1` | `SampleFrame` (8 bytes) |

Um comando desconhecido é ignorado: o periférico responde `0x00` até o SS subir.

## `SampleFrame` (8 bytes)

| Offset | Campo | Tipo | Descrição |
|---|---|---|---|
| 0 | `version` | `uint8_t` | Versão do protocolo (`1`) |
| 1 | `sequence` | `uint16_t` | Número da amostra. Começa em 1 no boot, `0` nunca é usado |
| 3 | `temperatureCentiC` | `int16_t` | Temperatura em centésimos de °C (`2345` = 23,45 °C) |
| 5 | `rawAdc` | `uint16_t` | Valor bruto do ADC (0 a 1023), para depuração |
| 7 | `checksum` | `uint8_t` | XOR dos bytes 0 a 6 |

A temperatura vai em inteiro (centésimos) para não depender da representação de `float` dos dois MCUs.

## Validação no ESP32

O quadro é descartado e registrado como `invalid SPI frame` no serial quando:

- `version` é diferente de `1`. Isso cobre MISO desconectado, que lê tudo `0x00` ou `0xFF`.
- `checksum` não confere. Isso cobre um byte corrompido ou atrasado.

A próxima consulta, 1 s depois, tenta de novo.

## Mensagem MQTT

Publicada com QoS 0 no tópico configurado em `esp32s3/include/secrets.h`:

```json
{"sequence":12,"temperature":23.45,"raw":481}
```

Se o broker estiver fora do ar, a amostra fica pendente e é publicada assim que a conexão voltar. Se chegar uma amostra mais nova antes disso, ela substitui a pendente.
