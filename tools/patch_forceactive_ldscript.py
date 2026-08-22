import argparse
from pathlib import Path
from typing import Iterable, List


def parse_symbols(raw: str) -> List[str]:
    symbols = [symbol.strip() for symbol in raw.split(",") if symbol.strip()]
    return list(dict.fromkeys(symbols))


def patch_forceactive_block(text: str, symbols: Iterable[str]) -> str:
    symbol_list = list(dict.fromkeys(symbols))
    if not symbol_list:
        return text

    newline = "\r\n" if "\r\n" in text else "\n"
    marker = "FORCEACTIVE"
    marker_index = text.find(marker)
    if marker_index < 0:
        raise ValueError("FORCEACTIVE block not found")

    open_index = text.find("{", marker_index)
    if open_index < 0:
        raise ValueError("FORCEACTIVE block missing opening brace")

    close_index = text.find("}", open_index)
    if close_index < 0:
        raise ValueError("FORCEACTIVE block missing closing brace")

    existing_symbols = [
        line.strip()
        for line in text[open_index + 1 : close_index].splitlines()
        if line.strip()
    ]
    merged_symbols = list(dict.fromkeys([*existing_symbols, *symbol_list]))

    block = marker + newline + "{" + newline
    for symbol in merged_symbols:
        block += f"    {symbol}{newline}"
    block += "}"
    return text[:marker_index] + block + text[close_index + 1 :]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("input_path")
    parser.add_argument("output_path")
    parser.add_argument("--symbols", default="")
    args = parser.parse_args()

    input_path = Path(args.input_path)
    output_path = Path(args.output_path)
    text = input_path.read_text(encoding="utf-8")
    patched = patch_forceactive_block(text, parse_symbols(args.symbols))
    output_path.write_text(patched, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
