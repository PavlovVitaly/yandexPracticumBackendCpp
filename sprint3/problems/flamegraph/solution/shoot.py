import argparse
import subprocess
import time
import random
import shlex
import signal

RANDOM_LIMIT = 1000
SEED = 123456789
random.seed(SEED)

AMMUNITION = [
    'localhost:8080/api/v1/maps/map1',
    'localhost:8080/api/v1/maps'
]

SHOOT_COUNT = 100
COOLDOWN = 0.1


def start_server():
    parser = argparse.ArgumentParser()
    parser.add_argument('server', type=str)
    return parser.parse_args().server


def perf_record_of(pid):
	return "perf record -p " + str(pid) + " -o perf.data -ag"

def run(command, output=None):
    process = subprocess.Popen(shlex.split(command), stdout=output, stderr=subprocess.DEVNULL)
    return process


def stop(process, wait=False):
    if process.poll() is None and wait:
        process.wait()
    process.terminate()


def shoot(ammo):
    hit = run('curl ' + ammo, output=subprocess.DEVNULL)
    time.sleep(COOLDOWN)
    stop(hit, wait=True)


def make_shots():
    for _ in range(SHOOT_COUNT):
        ammo_number = random.randrange(RANDOM_LIMIT) % len(AMMUNITION)
        shoot(AMMUNITION[ammo_number])
    print('Shooting complete')


server = run(start_server(), subprocess.DEVNULL)
time.sleep(0.1)
perf_record = run(perf_record_of(server.pid))
time.sleep(0.1)
make_shots()
perf_record.send_signal(signal.SIGINT)
time.sleep(0.1)
stop(server)
time.sleep(1)
with open("graph.svg", "w") as graph_file:
	perf_script = subprocess.Popen(shlex.split("perf script"), stdout=subprocess.PIPE)
	flamegraph_stackcollapse = subprocess.Popen(shlex.split("./FlameGraph/stackcollapse-perf.pl"), stdin=perf_script.stdout, stdout=subprocess.PIPE)
	flamegraph_output = subprocess.Popen(shlex.split("./FlameGraph/flamegraph.pl"), stdin=flamegraph_stackcollapse.stdout, stdout=graph_file)
	stop(perf_script, True)
	stop(flamegraph_stackcollapse, True)
	stop(flamegraph_output, True)
print('Job done')
# python3 shoot.py "/test_cpp_backend/sprint1/problems/map_json/solution/build/bin/game_server /test_cpp_backend/sprint1/problems/map_json/solution/data/config.json" || true
