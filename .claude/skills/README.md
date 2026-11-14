# Skills do projeto

Índice das skills em `.claude/skills/`, organizadas por classe. Cada skill fica em `.claude/skills/<nome>/SKILL.md`.

- A parte entre os marcadores `skills-index` é mantida pela skill `skills-index`. Para atualizar, peça "atualize o índice de skills" ou rode `/skills-index`. Para só conferir, rode `/skills-index check`.
- As seções **Classes** e **Regras de manutenção** são editadas à mão.

## Classes

| Classe | Critério |
|---|---|
| Harness | Configuração e manutenção do próprio Claude Code: CLAUDE.md, rules, skills, hooks, permissões e este índice |
| Firmware e placas | Compilar, gravar e monitorar o firmware do ESP32-S3 e do ATmega328P: PlatformIO, portas seriais e monitor serial |
| Protocolo SPI | Contrato em `common/protocol.h` (comandos e frames), pinagem, clock, modo SPI e `docs/protocolo_spi.md` |
| Testes e depuração | Validação da comunicação, capturas e análise de logs seriais |
| Fluxo do projeto | Documentos em `docs/`, README, commits e branches |
| Outras | Skills que ainda não se encaixam em nenhuma classe |

## Skills por classe

<!-- skills-index:start -->

Atualizado em: 2026-11-01 · 3 skills

### Harness

| Skill | O que faz | Quando usar | Como chamar | Ferramentas liberadas |
|---|---|---|---|---|
| `harness-pattern-capture` | Aplica um padrão de resposta que você definiu e pergunta, no fim, se deve salvá-lo no harness | Quando você define um padrão ou uma regra que deve valer para as próximas respostas | automática e manual (`/harness-pattern-capture`) | nenhuma |
| `skills-index` | Mantém este índice, organizado por classe, em dia com as pastas de skills | Logo depois de criar, renomear, alterar ou apagar uma skill, ou ao pedir para conferir ou atualizar o índice | automática e manual (`/skills-index [check]`) | nenhuma |

### Firmware e placas

| Skill | O que faz | Quando usar | Como chamar | Ferramentas liberadas |
|---|---|---|---|---|
| `upload-firmware` | Compila e grava o firmware de uma das placas pela USB com PlatformIO | Quando você pede para gravar (flash, upload) o firmware do ATmega328P (`uno`) ou do ESP32-S3 (`esp32s3`) | só manual (`/upload-firmware [uno\|esp32s3] [COMx]`) | nenhuma |

<!-- skills-index:end -->

## Regras de manutenção

- Use só o frontmatter do `SKILL.md`: nome, `description`, `when_to_use`, `argument-hint`, `disable-model-invocation`, `user-invocable` e `allowed-tools`. Não copie comandos nem instruções do corpo da skill.
- Cada skill fica em uma única classe, escolhida pela coluna Critério.
- As seções seguem a ordem da tabela Classes. Classe sem skills não ganha seção.
- Dentro de cada seção, as skills ficam em ordem alfabética, uma linha por skill.
- **Como chamar:** "automática e manual (`/nome`)" por padrão; "só manual (`/nome`)" com `disable-model-invocation: true`; "só automática" com `user-invocable: false`. Se houver `argument-hint`, ele vem depois do nome.
- **Ferramentas liberadas:** o valor de `allowed-tools`, ou "nenhuma". Confira esta coluna em toda revisão: essas ferramentas rodam sem pedir permissão enquanto a skill está ativa.
- Se nenhuma classe servir, a skill entra em **Outras** e a skill `skills-index` sugere uma classe nova. Classe nova só entra na tabela com a sua aprovação.
- Se uma skill sumir, a entrada só é removida depois da sua confirmação, porque a pasta pode estar sendo renomeada.
