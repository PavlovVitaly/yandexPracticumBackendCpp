import argparse
import json

names = ['slowest_func_v0', 'slowest_func_v1', 'slowest_func_v2']
parser = argparse.ArgumentParser()

for name in names:
    parser.add_argument(name, type=str, nargs='?')

with open('report','w') as report:
    report.write(json.dumps({name: parser.parse_args().__dict__[name] for name in names}))
