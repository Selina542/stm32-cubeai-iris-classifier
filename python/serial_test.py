"""Send the held-out Iris test set to the STM32 over UART.

Example:
    python 02_serial_test.py --port COM5
"""

from __future__ import annotations

import argparse
import csv
import time
from pathlib import Path

import serial


def read_until_prefix(ser: serial.Serial, prefix: str, timeout_s: float = 3.0) -> str:
    deadline = time.time() + timeout_s
    seen: list[str] = []
    while time.time() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="replace").strip()
        if line:
            print(f"STM32> {line}")
            seen.append(line)
        if line.startswith(prefix):
            return line
    raise TimeoutError(f"Timed out waiting for {prefix!r}. Last lines: {seen[-5:]}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", required=True, help="Windows example: COM5")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument(
        "--csv",
        type=Path,
        default=Path(__file__).resolve().parent / "iris_test.csv",
    )
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=0.25) as ser:
        # Some USB-UART adapters reset a board when the port opens.
        try:
            ser.dtr = False
            ser.rts = False
        except OSError:
            pass
        time.sleep(1.0)
        ser.reset_input_buffer()

        ser.write(b"R\r\n")
        read_until_prefix(ser, "RESET,")

        with args.csv.open("r", encoding="utf-8", newline="") as f:
            reader = csv.DictReader(f)
            rows = list(reader)

        for index, row in enumerate(rows, start=1):
            command = (
                f"V,{row['label']},{row['sepal_length']},{row['sepal_width']},"
                f"{row['petal_length']},{row['petal_width']}\r\n"
            )
            print(f"PC   > [{index:02d}/{len(rows):02d}] {command.strip()}")
            ser.write(command.encode("ascii"))
            read_until_prefix(ser, "VAL,")

        ser.write(b"S\r\n")
        stat = read_until_prefix(ser, "STAT,")
        print("\nFinal result:")
        print(stat)


if __name__ == "__main__":
    main()
