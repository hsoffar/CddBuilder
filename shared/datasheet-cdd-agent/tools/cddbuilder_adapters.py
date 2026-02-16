"""LLM adapter abstractions for CddBuilder."""

from __future__ import annotations

import os
from typing import Protocol


class LlmAdapter(Protocol):
    def generate(self, prompt: str) -> str:
        ...


class StubLlmAdapter:
    def generate(self, prompt: str) -> str:
        return (
            "LLM_DISABLED_OR_STUB\n"
            "This is a deterministic placeholder output.\n"
            f"Prompt length: {len(prompt)} chars\n"
        )


class EchoOpenAiAdapter:
    """Placeholder adapter path: checks env and emits deterministic placeholder."""

    def __init__(self) -> None:
        self.api_key_present = bool(os.getenv("OPENAI_API_KEY"))

    def generate(self, prompt: str) -> str:
        if not self.api_key_present:
            return "OPENAI_ADAPTER_DISABLED: OPENAI_API_KEY not set"
        return (
            "OPENAI_ADAPTER_PLACEHOLDER\n"
            "Provider wiring acknowledged (network call intentionally deferred in v1).\n"
            f"Prompt checksum basis length={len(prompt)}\n"
        )


def select_adapter(adapter_name: str, dry_run: bool) -> LlmAdapter:
    if dry_run:
        return StubLlmAdapter()
    if adapter_name == "openai":
        return EchoOpenAiAdapter()
    return StubLlmAdapter()
