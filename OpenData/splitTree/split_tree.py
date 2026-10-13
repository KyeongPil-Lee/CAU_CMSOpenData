#!/usr/bin/env python3
"""Split a ROOT TTree into smaller ROOT files by entry count."""

from __future__ import annotations

import argparse
import math
import sys
from pathlib import Path

import ROOT


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Split one tree in a ROOT file into fixed-size event chunks."
    )
    parser.add_argument("input", type=Path, help="Input ROOT file")
    parser.add_argument(
        "-n",
        "--events-per-file",
        type=int,
        default=50_000,
        help="Maximum number of events per output file (default: 50000)",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        type=Path,
        default=Path("split_output"),
        help="Output directory (default: split_output)",
    )
    parser.add_argument(
        "-t",
        "--tree",
        default="Events",
        help="Tree to split (default: Events)",
    )
    parser.add_argument(
        "--prefix",
        help="Output filename prefix (default: input filename without .root)",
    )
    parser.add_argument(
        "--overwrite",
        action="store_true",
        help="Overwrite output files that already exist",
    )
    parser.add_argument(
        "--resume",
        action="store_true",
        help="Keep valid completed parts and recreate missing or incomplete parts",
    )
    return parser.parse_args()


def open_root_file(path: Path, mode: str = "READ") -> ROOT.TFile:
    root_file = ROOT.TFile.Open(str(path), mode)
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Cannot open ROOT file: {path}")
    return root_file


def copy_auxiliary_objects(
    source: ROOT.TFile, destination: ROOT.TFile, split_tree_name: str
) -> None:
    """Copy top-level objects other than the tree being split."""
    destination.cd()
    for key in source.GetListOfKeys():
        if key.GetName() == split_tree_name:
            continue
        obj = key.ReadObj()
        obj.Write(key.GetName())


def has_expected_entries(path: Path, tree_name: str, expected: int) -> bool:
    if not path.exists():
        return False
    try:
        root_file = ROOT.TFile.Open(str(path), "READ")
    except OSError:
        return False
    if not root_file or root_file.IsZombie():
        return False
    try:
        tree = root_file.Get(tree_name)
        return bool(
            tree
            and tree.InheritsFrom("TTree")
            and int(tree.GetEntries()) == expected
        )
    finally:
        root_file.Close()


def split_tree(args: argparse.Namespace) -> list[Path]:
    if args.events_per_file <= 0:
        raise ValueError("--events-per-file must be at least 1.")
    if args.overwrite and args.resume:
        raise ValueError("--overwrite and --resume cannot be used together.")
    if not args.input.is_file():
        raise FileNotFoundError(f"Input file does not exist: {args.input}")

    source = open_root_file(args.input)
    try:
        tree = source.Get(args.tree)
        if not tree or not tree.InheritsFrom("TTree"):
            raise RuntimeError(f"Cannot find TTree '{args.tree}'.")

        total_entries = int(tree.GetEntries())
        if total_entries == 0:
            raise RuntimeError(f"TTree '{args.tree}' contains no events.")

        number_of_parts = math.ceil(total_entries / args.events_per_file)
        digits = max(3, len(str(number_of_parts)))
        prefix = args.prefix or args.input.stem
        args.output_dir.mkdir(parents=True, exist_ok=True)

        output_paths = [
            args.output_dir / f"{prefix}_part{part:0{digits}d}.root"
            for part in range(1, number_of_parts + 1)
        ]
        existing = [path for path in output_paths if path.exists()]
        if existing and not (args.overwrite or args.resume):
            preview = ", ".join(str(path) for path in existing[:3])
            more = " ..." if len(existing) > 3 else ""
            raise FileExistsError(
                f"Output files already exist: {preview}{more}\n"
                "Use --overwrite to replace them."
            )

        print(
            f"Input: {args.input}\n"
            f"Tree: {args.tree} ({total_entries:,} events)\n"
            f"Split: up to {args.events_per_file:,} events per file, "
            f"{number_of_parts} files total"
        )

        for index, output_path in enumerate(output_paths):
            first_entry = index * args.events_per_file
            entries = min(args.events_per_file, total_entries - first_entry)
            if args.resume and has_expected_entries(
                output_path, args.tree, entries
            ):
                print(
                    f"[{index + 1:>{digits}}/{number_of_parts}] {output_path} "
                    f"({entries:,} events, already complete; skipped)"
                )
                continue

            mode = "RECREATE" if (args.overwrite or args.resume) else "CREATE"
            destination = open_root_file(output_path, mode)
            try:
                destination.SetCompressionSettings(source.GetCompressionSettings())
                destination.cd()
                output_tree = tree.CopyTree("", "fast", entries, first_entry)
                if not output_tree:
                    raise RuntimeError(
                        f"Failed to copy tree entries {first_entry}"
                        f"–{first_entry + entries - 1}"
                    )
                output_tree.Write(args.tree, ROOT.TObject.kOverwrite)
                copy_auxiliary_objects(source, destination, args.tree)
            finally:
                destination.Close()

            print(
                f"[{index + 1:>{digits}}/{number_of_parts}] {output_path} "
                f"({entries:,} events)"
            )

        return output_paths
    finally:
        source.Close()


def verify_outputs(paths: list[Path], tree_name: str) -> int:
    total = 0
    for path in paths:
        root_file = open_root_file(path)
        try:
            tree = root_file.Get(tree_name)
            if not tree or not tree.InheritsFrom("TTree"):
                raise RuntimeError(
                    f"Output file does not contain TTree '{tree_name}': {path}"
                )
            total += int(tree.GetEntries())
        finally:
            root_file.Close()
    return total


def main() -> int:
    ROOT.gROOT.SetBatch(True)
    args = parse_args()
    try:
        paths = split_tree(args)
        total = verify_outputs(paths, args.tree)
        print(f"Done: verified {total:,} events across {len(paths)} files")
        return 0
    except (FileNotFoundError, FileExistsError, RuntimeError, ValueError) as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
