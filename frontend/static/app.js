const stepperEls = {};
document.querySelectorAll("#stepper li").forEach(li => {
  stepperEls[li.dataset.stage] = li;
});

const STAGE_ACTIVITY = {
  init: "Generating fresh U, V and additive shares…",
  dpf: "Client constructing a DPF key for this query…",
  preprocess: "Running Beaver-style preprocessing for both parties…",
  exchange: "Party 0 and Party 1 exchanging masked shares…",
  update: "Both parties applying their share of the update locally…",
};

const statusLine = document.getElementById("status-line");
const runBtn = document.getElementById("run-btn");
const form = document.getElementById("config-form");
const queryLog = document.getElementById("query-log");
const activityLine = document.getElementById("activity-line");
const metricsRow = document.getElementById("metrics-row");

const grids = {
  U0: document.getElementById("U0-grid"),
  V0: document.getElementById("V0-grid"),
  U1: document.getElementById("U1-grid"),
  V1: document.getElementById("V1-grid"),
  U: document.getElementById("U-grid"),
  V: document.getElementById("V-grid"),
};

let prevValues = {}; // track previous matrix values per grid id for change-pop animation
let latestState = null; // full {m,n,k,q,U0,V0,U1,V1,U,V} from the most recent run, used by the breach panel

function colorFor(value, max, hue){
  // hue: 'p0' teal, 'p1' amber, 'neutral'
  const t = Math.min(1, value / Math.max(1, max));
  const light = 78 - t * 40; // 78% -> 38%
  if (hue === "p0") return `hsl(172, 66%, ${light}%)`;
  if (hue === "p1") return `hsl(32, 90%, ${light}%)`;
  return `hsl(214, 18%, ${light}%)`;
}

function renderMatrix(gridEl, key, matrix, hue){
  if (!matrix) return;
  const flat = matrix.flat();
  const max = Math.max(1, ...flat.map(v => Math.abs(v)));
  const prev = prevValues[key] || [];
  gridEl.innerHTML = "";
  matrix.forEach((row, r) => {
    const rowEl = document.createElement("div");
    rowEl.className = "row";
    row.forEach((val, c) => {
      const cell = document.createElement("div");
      cell.className = "cell";
      cell.textContent = val;
      cell.style.background = colorFor(Math.abs(val), max, hue);
      const prevVal = prev[r] ? prev[r][c] : undefined;
      if (prevVal !== undefined && prevVal !== val) {
        cell.classList.add("changed");
      }
      rowEl.appendChild(cell);
    });
    gridEl.appendChild(rowEl);
  });
  prevValues[key] = matrix;
}

function setStage(stage, status){
  const el = stepperEls[stage];
  if (!el) return;
  if (status === "start") {
    Object.values(stepperEls).forEach(e => e.classList.remove("active"));
    el.classList.add("active");
    el.classList.remove("done");
    activityLine.textContent = STAGE_ACTIVITY[stage] || "";
  } else if (status === "done") {
    el.classList.remove("active");
    el.classList.add("done");
  }
}

function resetStepper(){
  Object.values(stepperEls).forEach(e => e.classList.remove("active", "done"));
  activityLine.textContent = "Waiting to run…";
}

function addQueryEntry(index, uIdx, vIdx, status){
  let entry = document.getElementById(`q-entry-${index}`);
  if (queryLog.querySelector(".empty")) queryLog.innerHTML = "";
  if (!entry) {
    entry = document.createElement("div");
    entry.className = "entry";
    entry.id = `q-entry-${index}`;
    queryLog.prepend(entry);
  }
  entry.innerHTML = `<span>Query ${index + 1} — update U[${uIdx}] · V[${vIdx}]</span><span class="badge">${status === "done" ? "✓ applied" : "running…"}</span>`;
  entry.classList.toggle("done", status === "done");
}

function formatBytes(n){
  if (n < 1024) return `${n} B`;
  return `${(n / 1024).toFixed(1)} KB`;
}

function showMetrics(d){
  document.getElementById("metric-exchange-ms").textContent = `${d.exchange_ms} ms`;
  document.getElementById("metric-bytes").textContent = formatBytes(d.total_bytes / Math.max(1, d.queries));
  document.getElementById("metric-queries").textContent = d.queries;
  document.getElementById("metric-parties").textContent = d.parties;
  metricsRow.hidden = false;
}

async function checkStatus(){
  try {
    const res = await fetch("/api/status");
    const data = await res.json();
    if (data.ready) {
      statusLine.textContent = `Binaries found at ${data.root}. Ready to run.`;
      statusLine.className = "status-line ok";
      runBtn.disabled = false;
    } else {
      statusLine.textContent = `Missing compiled binaries: ${data.missing.join(", ")}. See README to build the project first.`;
      statusLine.className = "status-line error";
      runBtn.disabled = true;
    }
  } catch (e) {
    statusLine.textContent = "Could not reach the backend.";
    statusLine.className = "status-line error";
    runBtn.disabled = true;
  }
}
checkStatus();

form.addEventListener("submit", (e) => {
  e.preventDefault();
  const m = document.getElementById("m").value;
  const n = document.getElementById("n").value;
  const k = document.getElementById("k").value;
  const q = document.getElementById("q").value;

  runBtn.disabled = true;
  statusLine.textContent = "Running protocol…";
  statusLine.className = "status-line";
  resetStepper();
  queryLog.innerHTML = `<p class="empty">Waiting for first query…</p>`;
  prevValues = {};
  metricsRow.hidden = true;
  resetBreachPanel();

  const es = new EventSource(`/api/run?m=${m}&n=${n}&k=${k}&q=${q}`);

  es.addEventListener("stage", (ev) => {
    const d = JSON.parse(ev.data);
    setStage(d.stage, d.status);
    if (d.status === "start" && d.label) {
      statusLine.textContent = d.label + "…";
    }
  });

  es.addEventListener("init", (ev) => {
    const d = JSON.parse(ev.data);
    renderMatrix(grids.U0, "U0", d.party0.U0, "p0");
    renderMatrix(grids.V0, "V0", d.party0.V0, "p0");
    renderMatrix(grids.U1, "U1", d.party1.U1, "p1");
    renderMatrix(grids.V1, "V1", d.party1.V1, "p1");
    renderMatrix(grids.U, "U", d.U, "neutral");
    renderMatrix(grids.V, "V", d.V, "neutral");
    latestState = { m: d.m, n: d.n, k: d.k, q: d.q,
      U0: d.party0.U0, V0: d.party0.V0, U1: d.party1.U1, V1: d.party1.V1,
      U: d.U, V: d.V };
    onStateUpdated();
  });

  es.addEventListener("query", (ev) => {
    const d = JSON.parse(ev.data);
    addQueryEntry(d.index, d.u_idx, d.v_idx, d.status);
    if (d.status === "done") {
      renderMatrix(grids.U0, "U0", d.party0.U0, "p0");
      renderMatrix(grids.V0, "V0", d.party0.V0, "p0");
      renderMatrix(grids.U1, "U1", d.party1.U1, "p1");
      renderMatrix(grids.V1, "V1", d.party1.V1, "p1");
      renderMatrix(grids.U, "U", d.U, "neutral");
      renderMatrix(grids.V, "V", d.V, "neutral");
      latestState = { ...latestState,
        U0: d.party0.U0, V0: d.party0.V0, U1: d.party1.U1, V1: d.party1.V1,
        U: d.U, V: d.V };
      onStateUpdated();
    }
  });

  es.addEventListener("complete", (ev) => {
    const d = JSON.parse(ev.data);
    statusLine.textContent = "Protocol run complete — all queries applied.";
    statusLine.className = "status-line ok";
    activityLine.textContent = "Done.";
    runBtn.disabled = false;
    showMetrics(d);
    es.close();
  });

  es.addEventListener("error", (ev) => {
    let message = "Connection lost.";
    if (ev.data) {
      try { message = JSON.parse(ev.data).message; } catch (_) { /* ignore */ }
    }
    statusLine.textContent = message;
    statusLine.className = "status-line error";
    runBtn.disabled = false;
    es.close();
  });
});

// --- insider-threat / breach simulation panel ---
const breachP0Btn = document.getElementById("breach-p0");
const breachP1Btn = document.getElementById("breach-p1");
const breachResetBtn = document.getElementById("breach-reset");
const breachEscalateBtn = document.getElementById("breach-escalate");
const breachEmpty = document.getElementById("breach-empty");
const breachStage1 = document.getElementById("breach-stage-1");
const breachStage2 = document.getElementById("breach-stage-2");
const exfilTitle = document.getElementById("exfil-title");
const breachUGrid = document.getElementById("breach-U-grid");
const breachVGrid = document.getElementById("breach-V-grid");
const breachHistogram = document.getElementById("breach-histogram");
const reconUGrid = document.getElementById("recon-U-grid");
const reconVGrid = document.getElementById("recon-V-grid");
const certaintyFill = document.getElementById("certainty-fill");
const certaintyCaption = document.getElementById("certainty-caption");
const serverP0 = document.getElementById("server-p0");
const serverP1 = document.getElementById("server-p1");

let breachedParty = null; // 'p0' | 'p1'

function setServerState(card, compromised){
  card.classList.toggle("compromised", compromised);
  card.querySelector(".server-state").textContent = compromised ? "compromised" : "secure";
}

function resetBreachPanel(){
  breachP0Btn.disabled = false;
  breachP1Btn.disabled = false;
  breachEmpty.hidden = true;
  breachedParty = null;
  breachStage1.hidden = true;
  breachStage2.hidden = true;
  breachResetBtn.hidden = true;
  setServerState(serverP0, false);
  setServerState(serverP1, false);
}

function onStateUpdated(){
  breachP0Btn.disabled = false;
  breachP1Btn.disabled = false;
  breachEmpty.hidden = true;
  // a fresh run invalidates any breach already shown
  breachedParty = null;
  breachStage1.hidden = true;
  breachStage2.hidden = true;
  breachResetBtn.hidden = true;
  setServerState(serverP0, false);
  setServerState(serverP1, false);
}

function renderHistogram(container, matrix, fieldSize){
  const counts = new Array(8).fill(0); // 8 buckets across the field
  const bucketWidth = fieldSize / 8;
  matrix.flat().forEach(v => {
    const b = Math.min(7, Math.floor(((v % fieldSize) + fieldSize) % fieldSize / bucketWidth));
    counts[b]++;
  });
  const max = Math.max(1, ...counts);
  container.innerHTML = "";
  counts.forEach(c => {
    const bar = document.createElement("div");
    bar.className = "bar";
    bar.style.height = `${Math.max(4, (c / max) * 100)}%`;
    bar.title = `${c} value(s)`;
    container.appendChild(bar);
  });
}

function showBreach(party){
  if (!latestState) return;
  breachedParty = party;
  breachStage1.hidden = false;
  breachStage2.hidden = true;
  breachResetBtn.hidden = false;

  setServerState(serverP0, party === "p0");
  setServerState(serverP1, party === "p1");

  exfilTitle.textContent = `Files pulled from ${party === "p0" ? "Party 0" : "Party 1"}'s disk`;
  document.getElementById("breach-u-label").textContent =
    `${party === "p0" ? "U₀" : "U₁"} shares`;
  document.getElementById("breach-v-label").textContent =
    `${party === "p0" ? "V₀" : "V₁"} shares`;

  const Umat = party === "p0" ? latestState.U0 : latestState.U1;
  const Vmat = party === "p0" ? latestState.V0 : latestState.V1;

  renderMatrix(breachUGrid, "breachU", Umat, "neutral");
  renderMatrix(breachVGrid, "breachV", Vmat, "neutral");
  breachUGrid.classList.add("attacker");
  breachVGrid.classList.add("attacker");

  const combined = Umat.flat().concat(Vmat.flat()).map(v => [v]);
  renderHistogram(breachHistogram, combined, 64);

  certaintyFill.style.width = "0%";
  certaintyCaption.textContent = "0% — one share alone determines nothing";

  breachStage1.scrollIntoView({ behavior: "smooth", block: "nearest" });
}

breachP0Btn.addEventListener("click", () => showBreach("p0"));
breachP1Btn.addEventListener("click", () => showBreach("p1"));

breachResetBtn.addEventListener("click", resetBreachPanel);

breachEscalateBtn.addEventListener("click", () => {
  if (!latestState || !breachedParty) return;
  const { m, n } = latestState;
  const recon = (a, b) => a.map((row, i) => row.map((v, j) => ((v + b[i][j]) % 64 + 64) % 64));
  const Urecon = recon(latestState.U0, latestState.U1);
  const Vrecon = recon(latestState.V0, latestState.V1);
  renderMatrix(reconUGrid, "reconU", Urecon, "neutral");
  renderMatrix(reconVGrid, "reconV", Vrecon, "neutral");

  setServerState(serverP0, true);
  setServerState(serverP1, true);
  certaintyFill.style.width = "100%";
  certaintyCaption.textContent = "100% — both shares together fully determine the values";

  breachStage2.hidden = false;
  breachStage2.scrollIntoView({ behavior: "smooth", block: "nearest" });
});
