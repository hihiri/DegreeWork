# AI Assistance Disclosure

## Purpose and Scope
This document describes how AI tools were used during the DegreeWork project.

AI was used as an assistant for suggestions and drafting support.
Final technical decisions remained with the project author.

## Tools Used
- Tool: GitHub Copilot Chat (VS Code)
- Access mode: In-editor assistant
- Model used in assistant sessions: GPT-5.3-Codex

## How AI Was Used
- Refactoring suggestions and code cleanup
- Error diagnosis support
- Documentation drafting and wording improvements
- Generation of repetitive code scaffolding (for example, simple method skeletons, basic logging patterns, and documentation templates), followed by manual review

## What AI Was Not Used For
- Autonomous final architecture decisions without human review
- Unverified protocol or concurrency changes in production logic
- Final submission text without human review and editing

## Human Verification Process
- AI suggestions were reviewed before applying
- Compilation/error checks were run after significant changes
- Protocol behavior was validated manually (status polling, payload handling, and error paths)

## Responsibility Statement
All final code and documentation decisions were made by the project author.
AI support was used as a productivity aid, not as an autonomous author.
