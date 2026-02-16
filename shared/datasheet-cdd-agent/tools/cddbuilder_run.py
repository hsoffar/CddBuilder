#!/usr/bin/env python3
"""CddBuilder v1 dry workflow runner (OpenClaw-first)."""

from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path
from typing import List, Protocol


@dataclass
class PromptSections:
    context: str
    constraints: str
    outputs: str
    acceptance: str


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


def _read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _normalize_device(device: str) -> str:
    return "".join(c.lower() if c.isalnum() else "_" for c in device).strip("_")


def assemble_sections(profile_md: str, rules_md_list: List[str], device: str, pdf_path: str) -> PromptSections:
    constraints = "\n\n".join(rules_md_list).strip()
    outputs = (
        f"Generate package for {device}:\n"
        "- src/<device>_driver_stub.c\n"
        "- docs/<device>_cdd_outline.md\n"
        "- tests/<device>_test_plan.md\n"
        "- report/summary.md\n"
    )
    acceptance = (
        "1) Platform-agnostic generator-owned files\n"
        "2) Assumptions and missing data explicitly listed\n"
        "3) Validation report emitted with pass/fail"
    )
    context = f"Device: {device}\nPDF: {pdf_path}\n\n{profile_md.strip()}"
    return PromptSections(context=context, constraints=constraints, outputs=outputs, acceptance=acceptance)


def build_prompt(sections: PromptSections) -> str:
    return (
        "## context\n"
        f"{sections.context}\n\n"
        "## constraints\n"
        f"{sections.constraints}\n\n"
        "## outputs\n"
        f"{sections.outputs}\n\n"
        "## acceptance criteria\n"
        f"{sections.acceptance}\n"
    )


def write_output_package(out_dir: Path, device_slug: str, prompt: str, llm_text: str) -> List[Path]:
    paths = {
        "src": out_dir / "src" / f"{device_slug}_driver_stub.c",
        "docs": out_dir / "docs" / f"{device_slug}_cdd_outline.md",
        "tests": out_dir / "tests" / f"{device_slug}_test_plan.md",
        "summary": out_dir / "report" / "summary.md",
        "prompt": out_dir / "report" / "prompt.md",
    }

    for p in paths.values():
        p.parent.mkdir(parents=True, exist_ok=True)

    paths["src"].write_text(
        "/* Auto-generated stub (v1 dry-run) */\n"
        "#include \"Std_Types.h\"\n\n"
        f"Std_ReturnType {device_slug}_Init(void) {{ return E_OK; }}\n",
        encoding="utf-8",
    )
    paths["docs"].write_text(
        f"# {device_slug.upper()} CDD Outline\n\n"
        "- Scope\n- Interfaces\n- Configuration\n- Diagnostics\n- Timing\n",
        encoding="utf-8",
    )
    paths["tests"].write_text(
        f"# {device_slug.upper()} Test Plan\n\n"
        "- Unit: conversion + state machine\n"
        "- Component: port contracts\n"
        "- Integration: AUTOSAR wiring\n"
        "- Fault injection: timeout/NACK/stale\n",
        encoding="utf-8",
    )
    paths["prompt"].write_text(prompt, encoding="utf-8")
    paths["summary"].write_text(
        "# CddBuilder Summary\n\n"
        "## Decisions\n"
        "- Used markdown-driven prompt sections.\n"
        "- Generated deterministic dry-run package.\n\n"
        "## Assumptions\n"
        "- Datasheet parsing is deferred in v1.\n"
        "- Integrator APIs are project-specific and out-of-scope for generated core stubs.\n\n"
        "## Missing Info\n"
        "- Project-specific DEM event IDs\n"
        "- Final RTE/API naming conventions\n\n"
        "## LLM Output/Notes\n"
        f"```\n{llm_text}\n```\n",
        encoding="utf-8",
    )

    return list(paths.values())


def run_validation(out_dir: Path, device_slug: str) -> dict:
    required = [
        out_dir / "src" / f"{device_slug}_driver_stub.c",
        out_dir / "docs" / f"{device_slug}_cdd_outline.md",
        out_dir / "tests" / f"{device_slug}_test_plan.md",
        out_dir / "report" / "summary.md",
        out_dir / "report" / "prompt.md",
    ]
    missing = [str(p) for p in required if not p.exists()]
    result = {"pass": len(missing) == 0, "missing": missing}

    validation_path = out_dir / "report" / "validation.json"
    validation_path.parent.mkdir(parents=True, exist_ok=True)
    validation_path.write_text(json.dumps(result, indent=2), encoding="utf-8")
    return result


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="OpenClaw CddBuilder v1 runner")
    parser.add_argument("--pdf", required=True, help="Path to datasheet PDF")
    parser.add_argument("--profile", required=True, help="Path to profile markdown")
    parser.add_argument("--rules", action="append", default=[], help="Path(s) to rules markdown")
    parser.add_argument("--out", required=True, help="Output directory")
    parser.add_argument("--device", required=True, help="Device name")
    parser.add_argument("--dry-run", action="store_true", help="Enable deterministic dry-run mode")
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    profile_path = Path(args.profile)
    rules_paths = [Path(p) for p in args.rules]
    out_dir = Path(args.out)

    profile_md = _read_text(profile_path)
    rules_md_list = [_read_text(p) for p in rules_paths]

    sections = assemble_sections(profile_md, rules_md_list, args.device, args.pdf)
    prompt = build_prompt(sections)

    adapter: LlmAdapter = StubLlmAdapter()
    llm_text = adapter.generate(prompt)

    device_slug = _normalize_device(args.device)
    write_output_package(out_dir, device_slug, prompt, llm_text)
    validation = run_validation(out_dir, device_slug)

    print(json.dumps({"out": str(out_dir), "validation": validation}, indent=2))
    return 0 if validation["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
