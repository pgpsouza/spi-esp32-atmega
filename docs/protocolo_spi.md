# Protocolo SPI

Contrato entre o **ESP32-S3** (controlador, embarcado A) e o **ATmega328P** (periférico, embarcado B). A fonte da verdade é [`common/protocol.h`](../common/protocol.h). Qualquer mudança nele deve atualizar este documento no mesmo commit.

## Parâmetros do barramento

| Parâmetro | Valor | Motivo |
|---|---|---|
| Versão do protocolo | `1` | Enviada em todo quadro; o ESP32 descarta versões diferentes |
| Modo SPI | 0 (CPOL=0, CPHA=0), MSB primeiro | Padrão do hardware SPI do ATmega328P |
| Clock | 500 kHz | Folga para o ATmega atender cada byte por interrupção |
| Setup após SS baixo | 10 µs | Tempo para o periférico perceber a seleção |
| Intervalo entre bytes | 40 µs | O ATmega carrega o próximo byte na ISR; cobre a latência de outras interrupções, como a de `millis()` |
| Ordem dos bytes | Little-endian | Ordem nativa dos dois MCUs |
| Período das amostras | 30 s (`samplePeriodMs`) | Requisito 4. O ATmega usa para gerar amostras; o ESP32 usa para mostrar na tela quanto falta para a próxima |
| Período da leitura ao vivo | 500 ms (`livePeriodMs`) | O ATmega atualiza a leitura ao vivo e o ESP32 consulta no mesmo ritmo, só para a tela |

## Fluxo

O SPI só permite que o controlador inicie a comunicação. Por isso o ESP32 faz **polling**, e o ATmega numera cada amostra. O ATmega mantém dois valores:

- **Amostra oficial** (`ReadSample`): gerada a cada 30 s e numerada. O ESP32 consulta a cada 1 s e **só publica no MQTT quando o número de sequência muda**. Isso atende os requisitos 3 e 4.
- **Leitura ao vivo** (`ReadLive`): atualizada a cada 500 ms, sempre com `sequence = 0`. O ESP32 consulta a cada 500 ms e usa **só para a tela**; ela nunca é publicada.

```
ATmega: a cada 30 s gera uma amostra nova (sequence + 1)

ESP32 (a cada 1 s)            ATmega
  SS ↓
  0xA1 (ReadSample)  ───────>  ISR: copia a amostra atual e carrega o 1º byte
  0x00 ×8            <───────  8 bytes do SampleFrame
  SS ↑                         ISR de SS: volta a aguardar comando

ESP32: quadro válido e sequence diferente do anterior -> publica no MQTT

ESP32 (a cada 500 ms)          ATmega
  SS ↓                         ISR de SS: volta a aguardar comando
  0xA2 (ReadLive)    ───────>  ISR: copia a leitura ao vivo e carrega o 1º byte
  0x00 ×8            <───────  8 bytes do SampleFrame (sequence = 0)
  SS ↑                         ISR de SS: volta a aguardar comando

ESP32: redesenha a tela se a temperatura variou 0,1 °C ou mais
```

O ATmega copia a amostra para um buffer no momento em que recebe o comando. Assim, uma amostra nova gerada no meio de uma transação não mistura bytes de duas amostras.

O estado da transação é zerado em **qualquer borda** do SS, e não só na subida. As duas consultas podem acontecer em sequência, e se a interrupção de SS rodar depois de o SS já ter descido de novo, o próximo comando ainda é reconhecido.

## Comandos

| Comando | Código | Resposta |
|---|---|---|
| `ReadSample` | `0xA1` | `SampleFrame` (8 bytes) da amostra oficial |
| `ReadLive` | `0xA2` | `SampleFrame` (8 bytes) da leitura ao vivo, com `sequence = 0` |

Um comando desconhecido é ignorado: o periférico responde `0x00` até o SS subir.

## `SampleFrame` (8 bytes)

| Offset | Campo | Tipo | Descrição |
|---|---|---|---|
| 0 | `version` | `uint8_t` | Versão do protocolo (`1`) |
| 1 | `sequence` | `uint16_t` | Número da amostra oficial. Começa em 1 no boot. `0` identifica a leitura ao vivo |
| 3 | `temperatureCentiC` | `int16_t` | Temperatura em centésimos de °C (`2345` = 23,45 °C) |
| 5 | `rawAdc` | `uint16_t` | Valor bruto do ADC (0 a 1023), para depuração |
| 7 | `checksum` | `uint8_t` | XOR dos bytes 0 a 6 |

A temperatura vai em inteiro (centésimos) para não depender da representação de `float` dos dois MCUs.

## Validação no ESP32

O quadro é descartado e registrado como `invalid SPI frame` no serial quando:

- `version` é diferente de `1`. Isso cobre MISO desconectado, que lê tudo `0x00` ou `0xFF`.
- `checksum` não confere. Isso cobre um byte corrompido ou atrasado.

A próxima consulta, 1 s depois, tenta de novo. Leituras ao vivo inválidas são descartadas sem log, para não repetir no serial a mesma falha já registrada pela consulta da amostra oficial.

## Mensagem MQTT

Publicada com QoS 0 no tópico configurado em `esp32s3/include/secrets.h`:

```json
{"sequence":12,"temperature":23.45,"raw":481}
```

Se o broker estiver fora do ar, a amostra fica pendente e é publicada assim que a conexão voltar. Se chegar uma amostra mais nova antes disso, ela substitui a pendente.
