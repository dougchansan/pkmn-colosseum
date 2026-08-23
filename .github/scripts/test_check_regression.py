#!/usr/bin/env python3
"""Focused tests for the one-time regression cleanup mechanism."""

import json
import tempfile
import unittest
from pathlib import Path

import check_regression


def report(scores):
    return {
        "units": [
            {
                "functions": [
                    {"name": name, "fuzzy_match_percent": score}
                    for name, score in scores.items()
                ]
            }
        ]
    }


class RegressionCleanupTests(unittest.TestCase):
    def setUp(self):
        self.tempdir = tempfile.TemporaryDirectory()
        root = Path(self.tempdir.name)
        self.base = root / "base.json"
        self.head = root / "head.json"
        self.manifest = root / "cleanup.json"
        self.base.write_text(json.dumps(report({"target": 100.0})))
        self.head.write_text(json.dumps(report({"target": 98.125})))
        self.manifest.write_text(
            json.dumps(
                {
                    "base_sha": "approved-base",
                    "regressions": {
                        "target": {"base": 100.0, "head": 98.125}
                    },
                }
            )
        )

    def tearDown(self):
        self.tempdir.cleanup()

    def run_check(self, base_sha="approved-base"):
        return check_regression.check(
            self.base, self.head, self.manifest, base_sha
        )

    def test_exact_transition_on_bound_base_is_allowed(self):
        self.assertEqual(self.run_check(), 0)

    def test_different_base_sha_is_blocked(self):
        self.assertEqual(self.run_check("different-base"), 1)

    def test_unlisted_function_is_blocked(self):
        self.head.write_text(
            json.dumps(report({"target": 98.125, "other": 50.0}))
        )
        self.base.write_text(
            json.dumps(report({"target": 100.0, "other": 60.0}))
        )
        self.assertEqual(self.run_check(), 1)

    def test_different_head_score_is_blocked(self):
        self.head.write_text(json.dumps(report({"target": 98.0})))
        self.assertEqual(self.run_check(), 1)

    def test_malformed_manifest_fails_closed(self):
        self.manifest.write_text(
            json.dumps(
                {
                    "base_sha": "approved-base",
                    "regressions": {},
                    "unexpected": True,
                }
            )
        )
        with self.assertRaises(ValueError):
            check_regression.check(
                self.base, self.head, self.manifest, "approved-base"
            )

    def test_omitted_fuzzy_score_is_not_comparable(self):
        self.base.write_text(json.dumps(report({"target": 53.396824})))
        self.head.write_text(
            json.dumps(
                {
                    "units": [
                        {"functions": [{"name": "target", "size": "252"}]}
                    ]
                }
            )
        )
        self.assertEqual(check_regression.check(self.base, self.head), 0)
        self.assertNotIn("target", check_regression.fmap(self.head))

    def test_omitted_base_score_does_not_create_false_regression(self):
        self.base.write_text(
            json.dumps(
                {
                    "units": [
                        {"functions": [{"name": "target", "size": "252"}]}
                    ]
                }
            )
        )
        self.head.write_text(json.dumps(report({"target": 53.396824})))
        self.assertEqual(check_regression.check(self.base, self.head), 0)

    def test_sub_hundredth_score_drift_is_ignored(self):
        self.base.write_text(json.dumps(report({"target": 52.94297})))
        self.head.write_text(json.dumps(report({"target": 52.93899})))
        self.assertEqual(check_regression.check(self.base, self.head), 0)


if __name__ == "__main__":
    unittest.main()
