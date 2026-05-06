import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_DIR = SCRIPT_DIR.parent
EXECUTABLE = str(PROJECT_DIR / "log_impl")
if not Path(EXECUTABLE).exists():
    EXECUTABLE = str(PROJECT_DIR / "build" / "log_impl")

OUTPUT_FILE = str(PROJECT_DIR / "output.txt")

TEST_RANGES = [
    ("00000001", "007FFFFF", "All Denormal numbers"),
    ("00800000", "01000000", "Smallest normalized numbers"),
    ("3E800000", "3F7FFFFF", "Numbers in [0.25, 1.0)"),
    ("3F800000", "407FFFFF", "Numbers in [1.0, 4.0)"),
    ("7F000000", "7F7FFFFF", "Large numbers near MAX_FLOAT")
]

def main():
    log_lines = []

    if not Path(EXECUTABLE).exists():
        print(f"ERROR: Executable not found at {EXECUTABLE}", file=sys.stderr)
        sys.exit(1)

    global_max_ulp = 0.0
    global_max_ulp_info = "N/A"
    total_errors = 0

    print(f"{'Range Description':<30} | {'Max ULP':<10} | Status")
    print("-" * 60)

    for start_hex, end_hex, desc in TEST_RANGES:
        try:
            proc = subprocess.run(
                [EXECUTABLE, start_hex, end_hex],
                capture_output=True,
                text=True,
                timeout=60
            )
            
            stdout = proc.stdout.strip()
            current_max_ulp = 0.0
            current_errors = 0

            for line in stdout.split('\n'):
                if "Max ULP:" in line:
                    try:
                        val_str = line.split(":")[1].split("(")[0].strip()
                        current_max_ulp = float(val_str)
                        if current_max_ulp > global_max_ulp:
                            global_max_ulp = current_max_ulp
                            global_max_ulp_info = line.strip()
                    except: continue
                if "Errors" in line:
                    try:
                        current_errors = int(line.split(":")[1].strip())
                        total_errors += current_errors
                    except: continue

            status = "OK" if current_errors == 0 else f"FAIL ({current_errors} err)"
            print(f"{desc:<30} | {current_max_ulp:<10.4f} | {status}")
            
            log_lines.append(f"--- {desc} [0x{start_hex}-0x{end_hex}] ---")
            log_lines.append(stdout + "\n")

        except Exception as e:
            print(f"{desc:<30} | ERROR      | {e}")
            log_lines.append(f"ERROR on range {desc}: {e}")

    summary = (
        f"\nTotal points with >3.5 ULP error: {total_errors}\n"
        f"Global {global_max_ulp_info}"
    )
    print(summary)
    
    try:
        with open(OUTPUT_FILE, "w") as f:
            f.write("\n".join(log_lines) + "\n" + summary)
    except Exception as e:
        print(f"Failed to write output file: {e}")

if __name__ == "__main__":
    main()