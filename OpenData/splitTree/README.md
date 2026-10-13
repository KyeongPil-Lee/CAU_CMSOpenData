# Split a ROOT Tree by Event Count

This script splits the `Events` tree in a large ROOT file into multiple ROOT
files containing a fixed maximum number of events. Each output file retains all
branches from the source tree, along with auxiliary objects such as `Runs`,
`LuminosityBlocks`, `MetaData`, and `ParameterSets`.

## Requirements

- Python 3
- PyROOT (check with `python3 -c "import ROOT"`)

## Usage

Run the following command from the repository root:

```bash
python3 OpenData/splitTree/split_tree.py \
  /Users/kplee/Downloads/Example_MC_DY_NANOAODSIM.root \
  --events-per-file 50000 \
  --output-dir OpenData/splitTree/output
```

This example splits 587,515 events into 12 files:

```text
Example_MC_DY_NANOAODSIM_part001.root  # 50,000 events
...
Example_MC_DY_NANOAODSIM_part011.root  # 50,000 events
Example_MC_DY_NANOAODSIM_part012.root  # 37,515 events
```

Main options:

```text
-n, --events-per-file N  Maximum events per file (default: 50000)
-o, --output-dir DIR     Output directory (default: split_output)
-t, --tree NAME          Tree to split (default: Events)
--prefix NAME            Output filename prefix
--overwrite              Overwrite existing output files
--resume                 Keep complete parts and recreate incomplete parts
```

Display all options with:

```bash
python3 OpenData/splitTree/split_tree.py --help
```

## Note

Auxiliary trees and objects are copied into every output file so that each part
can be opened independently. Therefore, summing aggregate values from the
`Runs` trees across multiple parts would double-count them. Use the `Events`
tree for event exercises, and use the original file's `Runs` information only
once for overall MC normalization.
