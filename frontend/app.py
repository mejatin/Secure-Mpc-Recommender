"""
Frontend server for the Scalable MPC Framework for Privacy-Preserving Profile Updates.

This does NOT reimplement any cryptography. It shells out to the real, compiled
C++ binaries (run1..run7) in the exact sequence Main/main.cpp uses, then reads
the same .txt files those binaries read/write to reflect true protocol state
in the browser. Progress is streamed to the frontend over Server-Sent Events.
"""

import json
import os
import subprocess
import threading
import time

from flask import Flask, Response, jsonify, render_template, request

# ---------------------------------------------------------------------------
# Locate the compiled project. Defaults to ../mpc-protocol relative to this
# file (i.e. this "frontend" folder is expected to sit next to "mpc-protocol"
# in the repo). Override with the MPC_PROJECT_ROOT env var if you keep it
# somewhere else.
# ---------------------------------------------------------------------------
DEFAULT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "mpc-protocol")
)
ROOT = os.environ.get("MPC_PROJECT_ROOT", DEFAULT_ROOT)

app = Flask(__name__)

# Only one pipeline run at a time -- the C++ side uses fixed file paths, so
# concurrent runs would corrupt each other's state.
RUN_LOCK = threading.Lock()


class PipelineError(Exception):
    pass


def run_exe(rel_exe_path, args=""):
    """Run a compiled binary the same way Main/main.cpp's runExe() does:
    cd into its own directory, then execute it with the given args."""
    rel_exe_path = rel_exe_path.strip("/")
    dirpart, exe = os.path.split(rel_exe_path)
    cwd = os.path.join(ROOT, dirpart)
    exe_path = os.path.join(cwd, exe)
    if not os.path.isfile(exe_path):
        raise PipelineError(
            f"Missing binary: {rel_exe_path}. Build the project first (see README)."
        )
    cmd = ["./" + exe] + (args.split() if args else [])
    proc = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, timeout=60)
    if proc.returncode != 0:
        raise PipelineError(
            f"{rel_exe_path} exited with code {proc.returncode}\n{proc.stderr.strip()}"
        )
    return proc.stdout


def read_matrix(rel_path, rows):
    path = os.path.join(ROOT, rel_path)
    with open(path) as f:
        lines = [ln for ln in f.read().splitlines() if ln.strip() != ""]
    return [[int(x) for x in lines[i].split()] for i in range(rows)]


def read_queries(rel_path, q):
    return read_matrix(rel_path, q)


def sse(event, data):
    return f"event: {event}\ndata: {json.dumps(data)}\n\n"


def dir_size(rel_path):
    total = 0
    root = os.path.join(ROOT, rel_path)
    for dirpath, _, filenames in os.walk(root):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            try:
                total += os.path.getsize(fp)
            except OSError:
                pass
    return total


def payload_size():
    """Total bytes currently sitting in the cross-party payload folders --
    the files each party actually writes for the other during an exchange."""
    total = 0
    for party in ("files/party0", "files/party1"):
        for sub in ("Recieved", "Recieved2", "last_item"):
            total += dir_size(f"{party}/{sub}")
    return total


def pipeline(m, n, k, q):
    """Generator mirroring Main/main.cpp's control flow exactly, yielding
    SSE-formatted progress events as each stage completes."""

    run_start = time.time()

    yield sse("stage", {"stage": "init", "status": "start",
                         "label": "Generating data & DPF query set"})
    run_exe("Gen_queries/run1", f"{m} {n} {k} {q}")

    queries = read_queries("globalfiles/queries.txt", q)
    U = read_matrix("globalfiles/U.txt", m)
    V = read_matrix("globalfiles/V.txt", n)
    U0 = read_matrix("files/party0/U0_party0.txt", m)
    V0 = read_matrix("files/party0/V0_party0.txt", n)
    U1 = read_matrix("files/party1/U1_party1.txt", m)
    V1 = read_matrix("files/party1/V1_party1.txt", n)

    yield sse("init", {
        "m": m, "n": n, "k": k, "q": q,
        "queries": queries,
        "U": U, "V": V,
        "party0": {"U0": U0, "V0": V0},
        "party1": {"U1": U1, "V1": V1},
    })
    yield sse("stage", {"stage": "init", "status": "done"})

    exchange_ms_list = []
    exchange_bytes_list = []

    for i, (u_idx, v_idx) in enumerate(queries):
        yield sse("query", {"index": i, "u_idx": u_idx, "v_idx": v_idx, "status": "start"})

        yield sse("stage", {"stage": "dpf", "status": "start",
                             "label": "Client builds DPF query key"})
        run_exe("Gen_queries/run2", f"{n} {v_idx}")
        yield sse("stage", {"stage": "dpf", "status": "done"})

        yield sse("stage", {"stage": "preprocess", "status": "start",
                             "label": "Beaver-style preprocessing"})
        run_exe("Communication/run3", f"{n}")
        run_exe("Communication/run4", "")
        yield sse("stage", {"stage": "preprocess", "status": "done"})

        yield sse("stage", {"stage": "exchange", "status": "start",
                             "label": "Party 0 \u2194 Party 1 secure exchange"})
        args = f"{u_idx} {v_idx} {m} {n}"
        errors = {}

        def target(rel, label):
            try:
                run_exe(rel, args)
            except PipelineError as e:
                errors[label] = str(e)

        t0 = threading.Thread(target=target, args=("files/party0/run5", "party0"))
        t0.start()
        time.sleep(1)  # let the server-side party bind its socket first
        exchange_start = time.time()  # measure only the actual exchange, not the bind wait
        t1 = threading.Thread(target=target, args=("files/party1/run6", "party1"))
        t1.start()
        t0.join()
        t1.join()
        if errors:
            raise PipelineError("; ".join(f"{k}: {v}" for k, v in errors.items()))

        exchange_ms = round((time.time() - exchange_start) * 1000, 1)
        exchange_bytes = payload_size()
        exchange_ms_list.append(exchange_ms)
        exchange_bytes_list.append(exchange_bytes)

        yield sse("stage", {"stage": "exchange", "status": "done",
                             "exchange_ms": exchange_ms, "exchange_bytes": exchange_bytes})

        yield sse("stage", {"stage": "update", "status": "start",
                             "label": "Applying secret-shared update"})
        run_exe("Updater/run7", f"{u_idx} {v_idx} {n}")
        yield sse("stage", {"stage": "update", "status": "done"})

        U0 = read_matrix("files/party0/U0_party0.txt", m)
        V0 = read_matrix("files/party0/V0_party0.txt", n)
        U1 = read_matrix("files/party1/U1_party1.txt", m)
        V1 = read_matrix("files/party1/V1_party1.txt", n)
        U = read_matrix("globalfiles/U.txt", m)
        V = read_matrix("globalfiles/V.txt", n)

        yield sse("query", {
            "index": i, "u_idx": u_idx, "v_idx": v_idx, "status": "done",
            "party0": {"U0": U0, "V0": V0},
            "party1": {"U1": U1, "V1": V1},
            "U": U, "V": V,
            "exchange_ms": exchange_ms, "exchange_bytes": exchange_bytes,
        })

    total_ms = round((time.time() - run_start) * 1000, 1)
    avg_exchange_ms = round(sum(exchange_ms_list) / len(exchange_ms_list), 1)
    avg_exchange_bytes = round(sum(exchange_bytes_list) / len(exchange_bytes_list))
    total_bytes = avg_exchange_bytes * q  # payload folders are overwritten each round, so
                                           # this is avg per-query payload x queries, not a
                                           # live cumulative on-disk total
    yield sse("complete", {
        "total_ms": total_ms,
        "exchange_ms": avg_exchange_ms,
        "total_bytes": total_bytes,
        "queries": q,
        "parties": 2,
    })


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/run")
def api_run():
    try:
        m = int(request.args.get("m", 6))
        n = int(request.args.get("n", 6))
        k = int(request.args.get("k", 3))
        q = int(request.args.get("q", 3))
    except ValueError:
        return jsonify({"error": "m, n, k, q must be integers"}), 400

    if not (1 <= m <= 12 and 1 <= n <= 12 and 1 <= k <= 8 and 1 <= q <= 8):
        return jsonify({"error": "m,n<=12, k<=8, q<=8 keep the grids readable"}), 400

    if not RUN_LOCK.acquire(blocking=False):
        return jsonify({"error": "A protocol run is already in progress"}), 409

    def stream():
        try:
            for event in pipeline(m, n, k, q):
                yield event
        except PipelineError as e:
            yield sse("error", {"message": str(e)})
        except Exception as e:  # noqa: BLE001
            yield sse("error", {"message": f"Unexpected error: {e}"})
        finally:
            RUN_LOCK.release()

    return Response(stream(), mimetype="text/event-stream")


@app.route("/api/status")
def api_status():
    required = [
        "Gen_queries/run1", "Gen_queries/run2",
        "Communication/run3", "Communication/run4",
        "files/party0/run5", "files/party1/run6",
        "Updater/run7",
    ]
    missing = [r for r in required if not os.path.isfile(os.path.join(ROOT, r))]
    return jsonify({"root": ROOT, "ready": not missing, "missing": missing})


if __name__ == "__main__":
    app.run(debug=True, port=5050)
