const ALGOS = ['FCFS', 'SCAN', 'C-SCAN'];

function loadCompare(event) {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = function(e) {
    try {
      const data = JSON.parse(e.target.result);
      // Validate it's a compare file (has FCFS, SCAN, CSCAN keys)
      if (!data.FCFS || !data.SCAN || !data.CSCAN) {
        alert('This does not look like a compare_result.json. Run Option 5 in the C++ program.');
        return;
      }
      setStatus(true, 'Loaded — all 3 algorithms');
      renderAll(data);
    } catch(err) {
      setStatus(false, 'Invalid JSON');
      alert('Could not parse compare_result.json.');
    }
  };
  reader.readAsText(file);
  // Reset so the same file can be reloaded after a new run
  event.target.value = '';
}

function renderAll(data) {
  const seeks = [data.FCFS.total_seek, data.SCAN.total_seek, data.CSCAN.total_seek];
  const minSeek = Math.min(...seeks);
  const maxSeek = Math.max(...seeks);

  renderSummary(data, minSeek, maxSeek);
  renderBarChart(data, minSeek, maxSeek);
}

function renderSummary(data, minSeek, maxSeek) {
  const row = document.getElementById('summary-row');
  row.innerHTML = '';

  const map = { 'FCFS': data.FCFS, 'SCAN': data.SCAN, 'C-SCAN': data.CSCAN };

  ALGOS.forEach(algo => {
    const d    = map[algo];
    const seek = d.total_seek;
    const isBest  = seek === minSeek;
    const isWorst = seek === maxSeek;

    let cls = 'algo-card loaded';
    if (isBest)  cls += ' best';
    if (isWorst && minSeek !== maxSeek) cls += ' worst';

    const card = document.createElement('div');
    card.className = cls;

    let badge = '';
    if (isBest)                        badge = '<div class="badge-big best">✓ BEST</div>';
    if (isWorst && minSeek !== maxSeek) badge = '<div class="badge-big worst">✗ WORST</div>';

    card.innerHTML = `
      <div class="algo-card-label">ALGORITHM</div>
      <div class="algo-card-name">${algo}</div>
      <div class="algo-card-label">TOTAL SEEK TIME</div>
      <div class="algo-card-seek ${isBest ? 'best-val' : ''}">${seek}</div>
      ${badge}
      <div class="algo-card-meta">
        Head: <span>${data.head}</span> &nbsp;|&nbsp;
        Disk: <span>${data.disk_size}</span> &nbsp;|&nbsp;
        Steps: <span>${d.steps.length}</span>
      </div>
    `;
    row.appendChild(card);
  });
}

function renderBarChart(data, minSeek, maxSeek) {
  const chart = document.getElementById('bar-chart');
  chart.innerHTML = '';

  const max = maxSeek || 1;
  const map = { 'FCFS': data.FCFS, 'SCAN': data.SCAN, 'C-SCAN': data.CSCAN };

  ALGOS.forEach(algo => {
    const seek    = map[algo].total_seek;
    const pct     = (seek / max) * 100;
    const isBest  = seek === minSeek;
    const isWorst = seek === maxSeek && minSeek !== maxSeek;

    const rowEl = document.createElement('div');
    rowEl.className = 'bar-row';
    rowEl.innerHTML = `
      <div class="bar-algo-label">${algo}</div>
      <div class="bar-track">
        <div class="bar-fill ${isBest ? 'bar-best' : isWorst ? 'bar-worst' : ''}"
             style="width:0%" data-target="${pct}%">
          <span class="bar-fill-label">${algo}</span>
        </div>
      </div>
      <div class="bar-val">${seek}</div>
    `;
    chart.appendChild(rowEl);
  });

  setTimeout(() => {
    chart.querySelectorAll('.bar-fill').forEach(b => b.style.width = b.dataset.target);
  }, 60);
}

function setStatus(active, text) {
  document.getElementById('status-dot').className    = 'dot' + (active ? ' active' : '');
  document.getElementById('status-text').textContent = text;
}