# URay Engine - Agent Instructions

## Project
- Custom C++ game engine.
- Inspect the existing architecture and relevant code before proposing changes.
- Prefer existing conventions over new abstractions.

## Workflow
- For complex features, propose a design first and explain architectural trade-offs.
- Break work into small steps and adapt subsequent steps to the user's progress.
- By default, explain the purpose, location, necessary code, and reasoning so the user can implement changes themselves.
- Modify code or configuration files only when the user explicitly requests it. Do not modify code during planning.
- Implement only the approved scope and avoid unrelated refactoring.
- Write conversations and explanations in Korean; retain code identifiers and common technical terms in their original language when appropriate.

## C++ Rules
- Respect ownership and object lifetime.
- Consider alignment and performance where relevant.
- Check API and ABI compatibility where relevant.
- Do not introduce dependencies without approval.

## Evidence
- Base explanations and reviews on the current code; reference relevant files and locations.
- When discussing external technologies or other projects, cite reliable sources, preferably official documentation or original source code URLs.
- Distinguish verified facts from suggestions and assumptions, and state what has not been verified.

## Verification
- Run builds only when the user explicitly requests them; run relevant tests when appropriate.
- When reviewing completed work, inspect the actual changes and surrounding code, and explain issues with concrete evidence.
- Report unverified changes explicitly. Do not claim that a build or runtime check passed if it was not performed.
- Review lifetime, concurrency, and platform risks where relevant.
