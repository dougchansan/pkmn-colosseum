#!/usr/bin/env python3

"""
Quick-start script — walks through the full Phase 1 pipeline:
  1. Check prerequisites
  2. Convert RVZ → ISO (if needed)
  3. Extract DOL from ISO
  4. Run initial dtk analysis
  5. Generate build configuration

Usage:
    python tools/quickstart.py
"""

import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
TOOLS_DIR = PROJECT_ROOT / "tools"
ORIG_DIR = PROJECT_ROOT / "orig" / "GC6E01"


def run_script(script_name, args=None):
    """Run a Python script from the tools directory."""
    cmd = [sys.executable, str(TOOLS_DIR / script_name)]
    if args:
        cmd.extend(args)
    return subprocess.run(cmd).returncode


def main():
    print("=" * 60)
    print("Pokémon Colosseum Decompilation — Quick Start")
    print("=" * 60)
    print()

    # Step 1: Check toolchain
    print("Step 1/5: Checking toolchain...")
    print("-" * 40)
    run_script("setup_toolchain.py")
    print()

    # Step 2: Check for ISO or RVZ
    iso_exists = any(ORIG_DIR.glob("*.iso")) or any(ORIG_DIR.glob("*.gcm"))
    dol_exists = (ORIG_DIR / "start.dol").exists()

    if dol_exists:
        print("Step 2/5: DOL already extracted — skipping conversion.")
        print("Step 3/5: DOL already extracted — skipping extraction.")
    elif iso_exists:
        print("Step 2/5: ISO found — skipping conversion.")
        print()
        print("Step 3/5: Extracting DOL...")
        print("-" * 40)
        rc = run_script("extract_dol.py")
        if rc != 0:
            print("DOL extraction failed. See errors above.")
            return 1
    else:
        print("Step 2/5: Converting RVZ → ISO...")
        print("-" * 40)
        rc = run_script("convert_rvz.py")
        if rc != 0:
            print("\nRVZ conversion failed or no RVZ found.")
            print("Convert manually and place the ISO in orig/GC6E01/")
            return 1
        print()
        print("Step 3/5: Extracting DOL...")
        print("-" * 40)
        rc = run_script("extract_dol.py")
        if rc != 0:
            print("DOL extraction failed. See errors above.")
            return 1

    # Step 4: DTK analysis
    print()
    print("Step 4/5: Running initial DOL analysis...")
    print("-" * 40)
    if not dol_exists and not (ORIG_DIR / "start.dol").exists():
        print("Skipping — no DOL available yet.")
    else:
        # Try to run dtk info
        import shutil
        dtk = shutil.which("dtk") or str(TOOLS_DIR / "dtk")
        try:
            subprocess.run([dtk, "dol", "info", str(ORIG_DIR / "start.dol")])
        except FileNotFoundError:
            print("dtk not available — install it to run analysis.")

    # Step 5: Configure
    print()
    print("Step 5/5: Generating build configuration...")
    print("-" * 40)
    rc = subprocess.run(
        [sys.executable, str(PROJECT_ROOT / "configure.py")],
    ).returncode

    print()
    print("=" * 60)
    if rc == 0:
        print("Setup complete! Next steps:")
    else:
        print("Setup incomplete. After resolving issues above:")
    print()
    print("  1. Open the DOL in Ghidra with PowerPC/Gekko language")
    print("  2. Cross-reference with StarsMmd's offset docs")
    print("  3. Start matching functions on https://decomp.me/")
    print("  4. Run: dtk dol split config/GC6E01/config.yml")
    print("=" * 60)

    return 0


if __name__ == "__main__":
    sys.exit(main())
