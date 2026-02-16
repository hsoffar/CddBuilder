"""Adapter layer for CddBuilder LLM invocation paths."""

from __future__ import annotations

import os
from dataclasses import dataclass
from typing import Protocol


class LlmAdapter(Protocol):
    def generate(self, prompt: str) -> str:
        ...


class StubLlmAdapter:
    def generate(self, prompt: str) -> str:
        return (
            "LLM_DISABLED_OR_STUB\n"
            "Deterministic fallback output.\n"
            f"Prompt length: {len(prompt)} chars\n"
        )


@dataclass
class OpenAiAdapter:
    """Real adapter-path skeleton.

    v1 behavior:
    - Reads OPENAI_API_KEY and optional OPENAI_MODEL.
    - Does not perform external network call yet.
    - Returns machine-friendly status text so caller/report can track path used.
    """

    api_key_env: str = "OPENAI_API_KEY"
    model_env: str = "OPENAI_MODEL"

    def generate(self, prompt: str) -> str:
        api_key = os.getenv(self.api_key_env)
        model = os.getenv(self.model_env, "gpt-4.1-mini")

        if not api_key:
            return "OPENAI_ADAPTER_UNAVAILABLE: missing OPENAI_API_KEY"

        return (
            "OPENAI_ADAPTER_READY_SKELETON\n"
            f"model={model}\n"
            "network_call=disabled_in_v1\n"
            f"prompt_len={len(prompt)}\n"
        )


def select_adapter(adapter_name: str, dry_run: bool) -> LlmAdapter:
    if dry_run:
        return StubLlmAdapter()

    if adapter_name == "openai":
        return OpenAiAdapter()

    return StubLlmAdapter()
