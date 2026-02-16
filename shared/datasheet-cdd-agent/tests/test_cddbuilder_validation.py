import json
import sys
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
TOOLS_DIR = REPO_ROOT / "tools"
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

from cddbuilder_run import run_validation  # noqa: E402


class ValidationChecklistTests(unittest.TestCase):
    def _write_checklist(self, base: Path) -> Path:
        checklist = {
            "requiredOutputs": [
                "src/{device}_driver_stub.c",
                "docs/{device}_cdd_outline.md",
                "tests/{device}_test_plan.md",
                "report/summary.md",
                "report/prompt.md",
                "report/validation.json",
            ]
        }
        path = base / "checklist.json"
        path.write_text(json.dumps(checklist), encoding="utf-8")
        return path

    def test_validation_pass_with_self_validation_in_checklist(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out_dir = Path(td) / "out"
            device = "tmp102"
            checklist = self._write_checklist(Path(td))

            (out_dir / "src").mkdir(parents=True)
            (out_dir / "docs").mkdir(parents=True)
            (out_dir / "tests").mkdir(parents=True)
            (out_dir / "report").mkdir(parents=True)

            (out_dir / "src" / f"{device}_driver_stub.c").write_text("x", encoding="utf-8")
            (out_dir / "docs" / f"{device}_cdd_outline.md").write_text("x", encoding="utf-8")
            (out_dir / "tests" / f"{device}_test_plan.md").write_text("x", encoding="utf-8")
            (out_dir / "report" / "summary.md").write_text("x", encoding="utf-8")
            (out_dir / "report" / "prompt.md").write_text("x", encoding="utf-8")

            result = run_validation(out_dir, device, checklist)

            self.assertTrue(result["pass"])
            self.assertEqual(result["missing"], [])
            self.assertTrue((out_dir / "report" / "validation.json").exists())

    def test_validation_fails_on_missing_required_file(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out_dir = Path(td) / "out"
            device = "tmp102"
            checklist = self._write_checklist(Path(td))

            (out_dir / "src").mkdir(parents=True)
            (out_dir / "docs").mkdir(parents=True)
            (out_dir / "tests").mkdir(parents=True)
            (out_dir / "report").mkdir(parents=True)

            # Deliberately skip docs output file.
            (out_dir / "src" / f"{device}_driver_stub.c").write_text("x", encoding="utf-8")
            (out_dir / "tests" / f"{device}_test_plan.md").write_text("x", encoding="utf-8")
            (out_dir / "report" / "summary.md").write_text("x", encoding="utf-8")
            (out_dir / "report" / "prompt.md").write_text("x", encoding="utf-8")

            result = run_validation(out_dir, device, checklist)

            self.assertFalse(result["pass"])
            self.assertEqual(len(result["missing"]), 1)
            self.assertIn(f"docs/{device}_cdd_outline.md", result["missing"][0])


if __name__ == "__main__":
    unittest.main()
