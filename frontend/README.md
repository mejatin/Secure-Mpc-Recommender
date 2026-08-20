# Frontend — MPC Profile-Update Dashboard

A local web dashboard that drives the real, compiled C++ protocol binaries
(`run1`–`run7`) exactly the way `Main/main.cpp` does, and visualizes the live
protocol state (each party's secret shares, the pipeline stages, the query
log) by reading the same files the C++ code reads and writes.

**Nothing about the cryptography is reimplemented here.** This is a thin
Flask bridge that shells out to the binaries and streams progress to the
browser over Server-Sent Events.

## Folder layout this expects

```
repo-root/
├── mpc-protocol/          ← the C++ backend
│   ├── Gen_queries/
│   ├── Communication/
│   ├── files/party0/, files/party1/
│   ├── Updater/
│   └── Main/
├── frontend/               ← this folder
│   ├── app.py
│   ├── requirements.txt
│   ├── templates/index.html
│   └── static/{style.css, app.js}
└── build.sh                 ← compiles mpc-protocol's binaries (see root README)
```

If you keep `mpc-protocol` somewhere else, set an env var instead of moving
folders:

```bash
export MPC_PROJECT_ROOT=/absolute/path/to/mpc-protocol
```

## 1. Build the C++ binaries

The dashboard does not compile anything itself. Build once from the repo
root:

```bash
./build.sh
```

See the root `README.md` for what this does and how to build with Docker
instead if you don't have a local C++ toolchain.

## 2. Run the dashboard

```bash
cd frontend
python3 -m venv .venv && source .venv/bin/activate   # optional but recommended
pip install -r requirements.txt
python3 app.py
```

Open **http://localhost:5050**.

The page checks on load whether all seven binaries are present (`/api/status`)
and disables the "Run protocol" button with a clear message if any are
missing.

## 3. Use it

1. Set `m` (users), `n` (items), `k` (latent dimension), `q` (number of
   update queries), then click **Run protocol**. Keep these small (≤ 12) —
   the grids are meant to be read at a glance, not to benchmark performance.
2. Watch the pipeline stepper light up through each stage: query generation,
   DPF key construction, preprocessing, the actual Party 0 ↔ Party 1 socket
   exchange, then the share update.
3. The two **Party** panels show exactly what that party has on disk —
   random-looking numbers, by design.
4. The **ground truth** panel at the bottom is only there so you (the
   presenter) can verify correctness: it's `U0+U1` and `V0+V1` reconstructed.
   In the real protocol, nothing ever computes this — it's for the demo only.

## Notes for a live demo / recording

- Every run regenerates `U`, `V`, and both parties' shares from scratch
  (that's `run1`'s behavior, not something the dashboard adds), so each click
  of "Run protocol" is a clean new scenario.
- Only one run executes at a time; the backend enforces this since the C++
  side writes to fixed file paths.
- If a run fails partway (e.g. a socket bind issue from a previous run not
  cleaning up), just click "Run protocol" again — `run1` resets state first.
