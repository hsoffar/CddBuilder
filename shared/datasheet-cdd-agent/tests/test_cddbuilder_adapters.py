import os
import sys
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
TOOLS_DIR = REPO_ROOT / "tools"
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

from cddbuilder_adapters import OpenAiAdapter, StubLlmAdapter, select_adapter  # noqa: E402


class AdapterSelectionTests(unittest.TestCase):
    def test_dry_run_forces_stub(self) -> None:
        adapter = select_adapter("openai", dry_run=True)
        self.assertIsInstance(adapter, StubLlmAdapter)

    def test_openai_selected_when_not_dry_run(self) -> None:
        adapter = select_adapter("openai", dry_run=False)
        self.assertIsInstance(adapter, OpenAiAdapter)

    def test_openai_adapter_reports_missing_key(self) -> None:
        old = os.environ.pop("OPENAI_API_KEY", None)
        try:
            out = OpenAiAdapter().generate("hello")
            self.assertIn("OPENAI_ADAPTER_UNAVAILABLE", out)
        finally:
            if old is not None:
                os.environ["OPENAI_API_KEY"] = old


if __name__ == "__main__":
    unittest.main()
