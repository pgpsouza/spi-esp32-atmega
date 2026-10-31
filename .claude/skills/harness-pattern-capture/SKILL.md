---
name: "harness-pattern-capture"
description: "Use when the user states a reusable response pattern or rule for future answers, to follow it now and ask once at the end whether to save it to a Claude Code harness part."
---

# Harness Pattern Capture

## Purpose

When the user sets a response pattern (format, structure, tone, checklist, scope rule), follow it in the current answer and then ask once, as the last line of the answer, whether to add it to the harness. Never edit any harness file without an explicit yes in the user's next message.

## 1. Detect a pattern

A pattern is a rule meant to repeat in future answers. Signals (Portuguese and English):

- Repetition words: "sempre", "nunca", "a partir de agora", "daqui pra frente", "toda vez", "always", "never", "from now on", "every time".
- A described response shape: "primeiro o risco, depois a solução", "use tabela para comparar", "responda em passos com comando e caminho".
- A style correction that applies beyond this answer: "não faça X, faça Y".

Not a pattern (do not ask anything): instructions that only concern this answer ("resuma esta resposta em 3 linhas"), plain factual questions, or requests to produce a specific deliverable.

If it is unclear whether the rule is meant to repeat, put the clarification in the final question instead of guessing.

## 2. Follow the pattern now

Apply the pattern fully to the current answer. Do not talk about the harness inside the answer body.

## 3. Restate the rule in one line

Write the rule as: `RULE: when <condition> -> <required behavior>`.

## 4. Ask once, at the end

The last line of the answer is one short question, written in the user's language (pt-BR for this user):

"Quer que eu salve esta regra no harness? Onde: (a) CLAUDE.md, orientação geral; (b) .claude/rules/, só para certos arquivos ou pastas; (c) skill, procedimento repetível; (d) subagent, papel especializado; (e) lock em settings.json (deny/ask) ou hook, regra que não pode falhar. Minha sugestão: <opção>, porque <motivo>. Responda a, b, c, d, e ou não."

Choose the recommended option with these rules:

- Format or style for all answers: CLAUDE.md.
- Applies only to some file types or folders: a file in .claude/rules/ with a `paths:` frontmatter.
- Multi-step procedure repeated often: a skill.
- Needs its own role, tool limits or isolated context: a subagent.
- Security or a "must never happen" rule: a lock (permissions deny/ask or a hook). Guidance text can be overridden by instructions hidden in files, so safety rules belong in locks.

If the user's workflow is in doubt, mention the verification command in one line (for example `/memory` to see loaded CLAUDE.md and rules, `/hooks` and `/permissions` to see active locks).

## 5. After the user answers

- No, or no reply about it: do not ask again about the same rule in this conversation.
- Yes: provide the complete file content, copy-ready, with the exact path. For Windows use `<repo>\.claude\...` for project scope and `%USERPROFILE%\.claude\...` for user scope. Keep the change minimal, and give one small test step to confirm it works.
- Write files in the workspace only when the user explicitly asks for it in that message.
- For a lock, include the security review: what it blocks, what it allows, and how to test it. Never add bypass flags such as `--dangerously-skip-permissions`.

## Guardrails

- Ask at most one question per response.
- If the pattern includes real process data, client names, internal paths or proprietary source code, warn the user before writing it into any harness file.
- If the pattern concerns security, network access, downloads, writes outside the project or destructive commands, flag it at the top of the answer, not at the end.
- Keep the harness-related question short. The answer always comes first.