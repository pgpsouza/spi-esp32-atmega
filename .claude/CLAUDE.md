# CLAUDE.md

## Project

SPI communication between an ESP32-S3 N16R8 (controller, MQTT gateway) and an ATmega328P (peripheral, data acquisition). Monorepo with two PlatformIO projects and one shared protocol contract.

## Structure

- `common/protocol.h`: the only shared file in the MVP. It is the single source of truth for commands and frame structures.
- `esp32s3/`: controller firmware. Arduino framework.
- `atmega328p/`: peripheral firmware. Arduino framework, `uno` board.
- `docs/`: `protocolo_spi.md`, `ligacoes.md`, and the official documentation for the ESP32-S3 and the Arduino Uno.

## Language

- Responses to the user are always in Portuguese, explaining the technical decisions.
- Code, comments, identifiers, and commits are always in English.
- Official third-party documents are kept in their original language.
- Project documents written by us (`docs/protocolo_spi.md`, `docs/ligacoes.md`, README) are in Portuguese.

## Code style

- Variables in `camelCase`.
- Functions in `snake_case`.
- Classes in `PascalCase`.
- Methods in `snake_case`.
- Members in `camelCase`.
- File names are the same as their classes.
- Every function has a Doxygen summary comment directly above it.
- Each step of a function's scope occupies one line.
- Use the ternary operator when it simplifies reading.
- Follow clean code: descriptive names, small functions with a single responsibility, no dead code, and no comments that repeat the code.
- Data that is transmitted or shared uses fixed-width types (`uint8_t`, `uint16_t`, `uint32_t`). Never use `int` for protocol data.
- Protocol commands and structures come from `common/protocol.h`. Do not redefine them in the projects.

## Protocol and hardware

- Any change to `common/protocol.h` must update `docs/protocolo_spi.md` in the same commit.
- Pinout, clock frequency, and SPI mode are recorded in `docs/ligacoes.md` once defined.
- Do not change the pinout or the protocol version without asking first.

## Libraries

- External libraries are accepted during the MVP.
- Declare them in `lib_deps` in `platformio.ini`. Do not copy library code into the project.

## Git

- Commits in English, following Conventional Commits: `feat:`, `fix:`, `docs:`, `refactor:`, `chore:`.
- Create one branch per task (`feature/...`, `fix/...`).
- Commit only when the user explicitly allows it.
- Merge branches only when the user explicitly allows it.
- At the end of a task that makes sense to commit, ask the user for permission to commit.
- Do not open pull requests. The user handles them manually.

## Build and validation

- Before considering a task done, both environments must build: `pio run -e esp32s3` and `pio run -e uno`.
- There are no unit tests in the MVP. ESP32 debugging is done over serial.

## Restrictions

- Do not delete capture files in `docs/capturas/`.

## How to respond

- Start with a simplified summary of what will be answered.
- Keep answers objective and short, but complete where it matters.
- No fallacies: do not claim something is verified when it was not, and do not hide problems.