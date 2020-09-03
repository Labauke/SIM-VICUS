@echo off

python ../../../../scripts/Python/tsvtool.py --output Result.tsv extract_columns SimQuality_TF03/results/loads.tsv "0|1"  "Zeit [h]|Tout [C]"

python ../../../../scripts/Python/tsvtool.py --output Result2.tsv extract_columns SimQuality_TF03/results/states.tsv "1|2|3|4" "Variante 1 [C]|Variante 2 [C]|Variante 3 [C]|Variante 4 [C]"

python ../../../../scripts/Python/tsvtool.py --output results/NANDRAD2_TF03_hourly.tsv insert_columns Result.tsv Result2.tsv 2 

del Result2.tsv Result.tsv

pause