# Tarefa: tela com a temperatura ao vivo

| Campo | Valor |
|---|---|
| Status | Concluída |
| Versão | 3 |
| Branch | `feature/live-display` |

Hoje a tela do ESP32-S3 só muda a cada 30 s, quando chega a amostra oficial. Como a tela será usada na apresentação, ela deve acompanhar a temperatura ao vivo. As amostras de 30 s continuam sendo as únicas publicadas no MQTT, para manter os requisitos 3 e 4 da especificação.

## Decisões

| Decisão | Escolha | Motivo |
|---|---|---|
| Como obter o valor ao vivo | Novo comando SPI `ReadLive` (`0xA2`) | Separa a leitura da tela da amostra oficial; o MQTT continua com o ritmo de 30 s |
| Formato da resposta | O mesmo `SampleFrame`, com `sequence = 0` | Não muda o formato do quadro; a versão do protocolo continua `1` |
| Leitura ao vivo no ATmega | A cada 500 ms | Rápida para a tela, sem sobrecarregar a ISR do SPI |
| Consulta ao vivo no ESP32 | A cada 500 ms | Acompanha a leitura do ATmega |
| Quando redesenhar a temperatura | Variação de pelo menos 0,1 °C em relação ao valor exibido | 1 passo do ADC (≈ 0,098 °C) não atualiza; 2 passos atualizam. Evita piscar com o ruído |
| Contador "Next in N s" | Continua a cada 1 s, baseado nas amostras oficiais | Mostra quando sai a próxima publicação MQTT |

## Histórico

| Versão | Data | Mudança |
|---|---|---|
| 1 | 2026-11-15 | Fluxograma, decisões e checklist iniciais |
| 2 | 2026-11-15 | Passos 1 a 3 concluídos e gravados. Decisões extras: o ATmega zera a transação SPI em qualquer borda do SS (as duas consultas podem vir em sequência); os tempos do ESP32 agrupados em `GatewaySettings` |
| 3 | 2026-11-15 | Concluída: tela acompanhando a temperatura confirmada no hardware |

## Fluxograma

```mermaid
flowchart TD
    start(["Início"])
    start --> s0["0. Branch feature/live-display<br/>a partir da develop"]

    subgraph S1["1. Protocolo"]
        s11["1.1 common/protocol.h<br/>Command::ReadLive = 0xA2<br/>livePeriodMs = 500"]
        s12["1.2 docs/protocolo_spi.md<br/>comando, fluxo e sequence = 0"]
        s11 --> s12
    end

    s0 --> s11

    subgraph S2["2. ATmega328P"]
        s21["2.1 SpiPeripheral: guardar duas amostras<br/>oficial e ao vivo; o comando escolhe qual enviar"]
        s22["2.2 TemperatureMonitor: ler o sensor a cada 500 ms<br/>e atualizar a amostra ao vivo"]
        s23["2.3 pio run -e uno"]
        s21 --> s22 --> s23
    end

    s12 --> s21

    subgraph S3["3. ESP32-S3"]
        s31["3.1 SpiController: read_live"]
        s32["3.2 GatewayApp: consultar ao vivo a cada 500 ms"]
        s33["3.3 GatewayApp: redesenhar a temperatura<br/>se variar 0,1 °C ou mais;<br/>contador continua a cada 1 s"]
        s34["3.4 pio run -e esp32s3"]
        s31 --> s32 --> s33 --> s34
    end

    s23 --> s31
    s34 --> d1{"Os dois builds<br/>passaram?"}
    d1 -- não --> fix1["Corrigir o erro"] --> s23
    d1 -- sim --> s41

    subgraph S4["4. Teste no hardware"]
        s41["4.1 Gravar as duas placas"]
        s42["4.2 Variar a temperatura<br/>(aquecer o sensor com a mão)"]
        s43["4.3 Serial do ESP32: amostras oficiais<br/>continuam a cada 30 s"]
        s41 --> s42 --> s43
    end

    s43 --> d2{"A tela acompanha<br/>a temperatura?"}
    d2 -- não --> diag["Diagnóstico<br/>invalid SPI frame → comando ReadLive<br/>tela parada → margem ou consulta ao vivo"]
    diag --> s41

    subgraph S5["5. Fechar"]
        s51["5.1 pio run -e uno<br/>pio run -e esp32s3"]
        s52["5.2 Commit<br/>feat: show live temperature on the ESP32-S3 display"]
        s53["5.3 Merge na develop e push"]
        s51 --> s52 --> s53
    end

    d2 -- sim --> s51
    s53 --> finish(["Fim"])

    classDef decision fill:#fff4ce,stroke:#b58900,color:#222
    classDef terminal fill:#d8ecff,stroke:#1f6feb,color:#222
    classDef warning fill:#ffe1e1,stroke:#d1242f,color:#222
    class d1,d2 decision
    class start,finish terminal
    class fix1,diag warning
```

## Checklist

### 0. Branch
- [x] `feature/live-display` criada a partir da `develop`

### 1. Protocolo
- [x] `Command::ReadLive` e `livePeriodMs` em `common/protocol.h`
- [x] `docs/protocolo_spi.md` atualizado no mesmo commit

### 2. ATmega328P
- [x] `SpiPeripheral` com amostra oficial e ao vivo (reset em qualquer borda do SS)
- [x] `TemperatureMonitor` atualizando a amostra ao vivo a cada 500 ms
- [x] `pio run -e uno` passando

### 3. ESP32-S3
- [x] `SpiController::read_live`
- [x] `GatewayApp` consultando ao vivo a cada 500 ms
- [x] Tela redesenhada com variação de 0,1 °C ou mais
- [x] `pio run -e esp32s3` passando

### 4. Hardware
- [x] Firmware gravado nas duas placas
- [x] Tela acompanha a variação de temperatura
- [x] Amostras oficiais continuam a cada 30 s (seq 1 e 2 no serial)

### 5. Fechar
- [x] Builds dos dois ambientes passando
- [x] Commit `feat: show live temperature on the ESP32-S3 display`
- [x] Merge na `develop` e push
