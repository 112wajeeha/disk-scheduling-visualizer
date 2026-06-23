// ─── STATE ────────────────────────────────────────────────────────────────────
const loadedResults = {};
let currentData = null;
let animFrame   = null;

// ─── FILE INPUT ───────────────────────────────────────────────────────────────
function loadFromFile(event) {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = function(e) {
    try {
      const data = JSON.parse(e.target.result);
      loadedResults[data.algorithm] = data;
      render(data);
    } catch (err) {
      setStatus(false, 'Invalid JSON file');
      alert('Could not parse result.json. Make sure the C++ program ran successfully.');
    }
  };
  reader.readAsText(file);
  event.target.value = '';
}

// ─── RENDER ───────────────────────────────────────────────────────────────────
function render(d) {
  currentData = d;
  setStatus(true, 'Loaded — ' + d.algorithm);
  document.getElementById('no-data-msg').classList.add('hidden');

  const minVal = 0;
  const maxVal = d.disk_size - 1;

  setValue('algo-name', d.algorithm);
  setValue('head-pos',  d.head);
  setValue('disk-size', d.disk_size);
  setValue('direction', d.direction.toUpperCase());
  setValue('cyl-range', minVal + ' → ' + maxVal);
  setValue('seek-time', d.total_seek);

  document.querySelectorAll('.card').forEach(c => c.classList.add('loaded'));

  const tagsEl = document.getElementById('request-tags');
  tagsEl.innerHTML = '';
  d.requests.forEach((r, i) => {
    const t = document.createElement('span');
    t.className = 'tag';
    t.textContent = r;
    t.style.animationDelay = (i * 40) + 'ms';
    tagsEl.appendChild(t);
  });

  buildTable(d.steps);
  drawCanvas(d, minVal, maxVal);
}

// ─── TABLE ────────────────────────────────────────────────────────────────────
function buildTable(steps) {
  const tbody = document.getElementById('steps-body');
  tbody.innerHTML = '';
  steps.forEach((s, i) => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${i + 1}</td>
      <td>${s.from}</td>
      <td>${s.to}</td>
      <td>${s.distance}</td>
    `;
    tr.querySelectorAll('td').forEach(td => td.style.animationDelay = (i * 35) + 'ms');
    tbody.appendChild(tr);
  });
}

// ─── CANVAS ───────────────────────────────────────────────────────────────────
function drawCanvas(d, minVal, maxVal) {
  const canvas  = document.getElementById('diskCanvas');
  const wrapper = canvas.parentElement;
  canvas.width  = wrapper.clientWidth  - 48;
  canvas.height = wrapper.clientHeight - 32;

  const sequence = [d.head, ...d.steps.map(s => s.to)];
  const range = maxVal - minVal || 1;

  const PAD_L = 52, PAD_R = 20, PAD_T = 30, PAD_B = 28;
  const W = canvas.width, H = canvas.height;
  const gW = W - PAD_L - PAD_R;
  const gH = H - PAD_T - PAD_B;

  const xFor = i => PAD_L + (i / (sequence.length - 1)) * gW;
  const yFor = v => PAD_T + gH - ((v - minVal) / range) * gH;

  if (animFrame) cancelAnimationFrame(animFrame);
  animate(canvas, sequence, xFor, yFor, minVal, range, PAD_L, PAD_T, gH, Date.now(), 2800);
}

function animate(canvas, seq, xFor, yFor, minVal, range, PAD_L, PAD_T, gH, t0, totalMs) {
  const ctx     = canvas.getContext('2d');
  const W = canvas.width, H = canvas.height;
  const elapsed  = Date.now() - t0;
  const msPerSeg = totalMs / (seq.length - 1);
  const fullSegs = Math.min(Math.floor(elapsed / msPerSeg), seq.length - 1);
  const partial  = Math.min((elapsed % msPerSeg) / msPerSeg, 1);

  ctx.clearRect(0, 0, W, H);

  // Grid lines + Y-axis labels
  ctx.strokeStyle = 'rgba(30,48,72,0.7)';
  ctx.lineWidth   = 1;
  ctx.font        = '11px Share Tech Mono';
  ctx.fillStyle   = '#4a6580';
  ctx.textAlign   = 'right';
  for (let i = 0; i <= 5; i++) {
    const v = minVal + (i / 5) * range;
    const y = yFor(v);
    ctx.beginPath(); ctx.moveTo(PAD_L, y); ctx.lineTo(W - 20, y); ctx.stroke();
    ctx.fillText(Math.round(v), PAD_L - 6, y + 4);
  }

  // Completed segments
  for (let i = 0; i < fullSegs && i < seq.length - 1; i++) {
    drawSegment(ctx, xFor(i), yFor(seq[i]), xFor(i+1), yFor(seq[i+1]));
    drawDot(ctx, xFor(i), yFor(seq[i]), i === 0 ? '#00ff88' : '#00d4ff', i === 0 ? 7 : 4);
    drawArrowHead(ctx, xFor(i), yFor(seq[i]), xFor(i+1), yFor(seq[i+1]), '#00d4ff');
    // Numbers above each point — larger font
    ctx.font = '12px Share Tech Mono';
    ctx.fillStyle = '#c9d8e8';
    ctx.textAlign = 'center';
    ctx.fillText(seq[i], xFor(i), yFor(seq[i]) - 13);
  }

  // Partial (in-progress) segment
  if (fullSegs < seq.length - 1) {
    const x1 = xFor(fullSegs),     y1 = yFor(seq[fullSegs]);
    const x2 = xFor(fullSegs + 1), y2 = yFor(seq[fullSegs + 1]);
    const px = x1 + (x2 - x1) * partial;
    const py = y1 + (y2 - y1) * partial;
    drawSegment(ctx, x1, y1, px, py);
    drawDot(ctx, px, py, '#ff6b35', 8, true);
    if (partial > 0.15) drawArrowHead(ctx, x1, y1, px, py, '#ff6b35');

    animFrame = requestAnimationFrame(() =>
      animate(canvas, seq, xFor, yFor, minVal, range, PAD_L, PAD_T, gH, t0, totalMs)
    );
  } else {
    // Done — draw last dot + all labels
    drawDot(ctx, xFor(seq.length - 1), yFor(seq[seq.length - 1]), '#ff6b35', 6);
    ctx.font = '12px Share Tech Mono';
    ctx.fillStyle = '#c9d8e8';
    ctx.textAlign = 'center';
    seq.forEach((v, i) => ctx.fillText(v, xFor(i), yFor(v) - 13));
  }
}

function drawSegment(ctx, x1, y1, x2, y2) {
  ctx.save();
  ctx.beginPath(); ctx.moveTo(x1, y1); ctx.lineTo(x2, y2);
  ctx.setLineDash([]);
  ctx.strokeStyle = '#00d4ff';
  ctx.shadowColor = '#00d4ff';
  ctx.lineWidth = 2; ctx.shadowBlur = 6; ctx.stroke(); ctx.restore();
}

function drawArrowHead(ctx, x1, y1, x2, y2, color) {
  const angle = Math.atan2(y2 - y1, x2 - x1);
  const size  = 8;
  ctx.save();
  ctx.beginPath();
  ctx.moveTo(x2, y2);
  ctx.lineTo(x2 - size * Math.cos(angle - Math.PI/6), y2 - size * Math.sin(angle - Math.PI/6));
  ctx.lineTo(x2 - size * Math.cos(angle + Math.PI/6), y2 - size * Math.sin(angle + Math.PI/6));
  ctx.closePath();
  ctx.fillStyle = color; ctx.shadowColor = color; ctx.shadowBlur = 8; ctx.fill(); ctx.restore();
}

function drawDot(ctx, x, y, color, r, glow = false) {
  ctx.save();
  if (glow) { ctx.shadowColor = color; ctx.shadowBlur = 18; }
  ctx.beginPath(); ctx.arc(x, y, r, 0, Math.PI * 2);
  ctx.fillStyle = color; ctx.fill(); ctx.restore();
}

// ─── HELPERS ─────────────────────────────────────────────────────────────────
function setValue(id, val) {
  const el = document.getElementById(id);
  if (el) el.textContent = val;
}

function setStatus(active, text) {
  document.getElementById('status-dot').className    = 'dot' + (active ? ' active' : '');
  document.getElementById('status-text').textContent = text;
}