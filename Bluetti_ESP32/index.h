// Status page of the bridge. Served as is (no template processing), the values come from /status
// (JSON) and from the /events stream, see BWifi.cpp.
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Bluetti MQTT Bridge</title>
<style>
  :root {
    color-scheme: light;
    --bg: #eef1ef;
    --surface: #ffffff;
    --surface-2: #f6f8f7;
    --ink: #14211d;
    --muted: #5a6b65;
    --line: #d8dfdb;
    --accent: #f09a10;
    --accent-ink: #7a4a00;
    --ok: #1b8a4e;
    --ok-bg: #e2f3e9;
    --warn: #a96300;
    --warn-bg: #fbeed4;
    --bad: #c0362b;
    --bad-bg: #f9e1de;
    --shadow: 0 1px 2px rgba(20, 33, 29, .06), 0 6px 18px rgba(20, 33, 29, .06);
    --sans: system-ui, -apple-system, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
    --mono: ui-monospace, "SF Mono", "Cascadia Mono", Consolas, "Liberation Mono", monospace;
  }
  @media (prefers-color-scheme: dark) {
    :root {
      color-scheme: dark;
      --bg: #0e1513;
      --surface: #16201d;
      --surface-2: #1b2723;
      --ink: #e5eee9;
      --muted: #8ea39b;
      --line: #263631;
      --accent: #f4a62a;
      --accent-ink: #f4c879;
      --ok: #4cc585;
      --ok-bg: #143323;
      --warn: #efb04d;
      --warn-bg: #3a2b10;
      --bad: #f0796e;
      --bad-bg: #3c1a17;
      --shadow: 0 1px 2px rgba(0, 0, 0, .4), 0 6px 18px rgba(0, 0, 0, .3);
    }
  }

  * { box-sizing: border-box; }
  body {
    margin: 0;
    background: var(--bg);
    color: var(--ink);
    font: 15px/1.45 var(--sans);
    padding-inline: 16px;
    padding-block: 0 40px;
  }
  .wrap { max-width: 1040px; margin-inline: auto; }
  .num, .mono { font-variant-numeric: tabular-nums; }
  .mono { font-family: var(--mono); font-size: .92em; letter-spacing: -.01em; }

  header { display: flex; flex-wrap: wrap; align-items: center; justify-content: space-between; gap: 12px 24px; padding-block: 22px 18px; }
  h1 { margin: 0; font-size: 1.35rem; line-height: 1.2; letter-spacing: -.01em; }
  .sub { color: var(--muted); font-size: .88rem; margin-top: 2px; }
  .sub a { color: var(--accent-ink); text-underline-offset: 3px; }
  .brand { display: flex; align-items: center; gap: 12px; }
  .mark { inline-size: 34px; block-size: 34px; border-radius: 9px; background: var(--accent); display: grid; place-items: center; flex: none; }
  .mark svg { inline-size: 20px; block-size: 20px; }
  .chip { display: inline-flex; align-items: center; gap: 8px; padding: 6px 12px; border: 1px solid var(--line); border-radius: 999px; background: var(--surface); font-size: .85rem; }
  .chip b { font-weight: 650; }
  .chip span { color: var(--muted); }

  .notice { margin-bottom: 12px; padding: 10px 14px; border-radius: 10px; background: var(--warn-bg); color: var(--warn); font-size: .9rem; font-weight: 600; }

  .strip { display: grid; grid-template-columns: repeat(auto-fit, minmax(min(15rem, calc(100vw - 32px)), 1fr)); gap: 12px; margin-bottom: 12px; }
  .tile { background: var(--surface); border: 1px solid var(--line); border-radius: 12px; padding: 14px 16px; box-shadow: var(--shadow); display: grid; gap: 4px; }
  .tile .lbl { font-size: .74rem; text-transform: uppercase; letter-spacing: .08em; color: var(--muted); display: flex; align-items: center; justify-content: space-between; gap: 8px; }
  .tile .lbl .pill { text-transform: none; letter-spacing: 0; }
  .tile .big { font-size: 1.25rem; font-weight: 650; letter-spacing: -.01em; }
  .tile .small { color: var(--muted); font-size: .85rem; overflow-wrap: anywhere; }

  .pill { display: inline-flex; align-items: center; gap: 6px; padding: 2px 10px 2px 8px; border-radius: 999px; font-size: .78rem; font-weight: 600; background: var(--surface-2); color: var(--muted); white-space: nowrap; }
  .pill::before { content: ""; inline-size: 7px; block-size: 7px; border-radius: 50%; background: currentColor; }
  .pill[data-s="ok"] { background: var(--ok-bg); color: var(--ok); }
  .pill[data-s="warn"] { background: var(--warn-bg); color: var(--warn); }
  .pill[data-s="bad"] { background: var(--bad-bg); color: var(--bad); }
  .sig { display: inline-flex; align-items: flex-end; gap: 2px; block-size: 16px; --c: var(--muted); }
  .sig i { inline-size: 4px; border-radius: 1px; background: var(--line); }
  .sig i:nth-child(1) { block-size: 5px; }
  .sig i:nth-child(2) { block-size: 8px; }
  .sig i:nth-child(3) { block-size: 12px; }
  .sig i:nth-child(4) { block-size: 16px; }
  .sig i.on { background: var(--c); }

  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(min(22rem, calc(100vw - 32px)), 1fr)); gap: 12px; }
  .card { background: var(--surface); border: 1px solid var(--line); border-radius: 12px; box-shadow: var(--shadow); padding: 16px 18px 6px; }
  .card h2 { margin: 0 0 6px; font-size: .74rem; text-transform: uppercase; letter-spacing: .08em; color: var(--muted); font-weight: 650; }
  .wide { grid-column: 1 / -1; }
  dl { margin: 0; }
  .row { display: flex; align-items: center; justify-content: space-between; gap: 16px; padding-block: 10px; border-top: 1px solid var(--line); min-block-size: 44px; }
  .row:first-of-type { border-top: 0; }
  dt { color: var(--muted); font-size: .9rem; flex: none; }
  dd { margin: 0; display: flex; align-items: center; justify-content: flex-end; flex-wrap: wrap; gap: 4px 10px; text-align: end; overflow-wrap: anywhere; min-inline-size: 0; }
  dd small { color: var(--muted); font-size: .8rem; }

  .meter { display: grid; gap: 6px; padding-block: 10px 14px; border-top: 1px solid var(--line); }
  .meter .top { display: flex; justify-content: space-between; align-items: baseline; gap: 12px; }
  .meter .top > span:first-child { color: var(--muted); font-size: .9rem; }
  .bar { display: block; block-size: 8px; border-radius: 999px; background: var(--surface-2); box-shadow: inset 0 0 0 1px var(--line); overflow: hidden; }
  .bar i { display: block; block-size: 8px; background: var(--ok); transform-origin: left center; transform: scaleX(var(--v, 0)); transition: transform .5s ease; }
  .bar[data-s="warn"] i { background: var(--warn); }
  .bar[data-s="bad"] i { background: var(--bad); }
  .cores { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
  .cores .bar, .cores .bar i { block-size: 5px; }
  .cores small { color: var(--muted); font-size: .78rem; display: flex; justify-content: space-between; margin-bottom: 4px; }

  button { font: inherit; color: var(--ink); background: var(--surface); border: 1px solid var(--line); border-radius: 8px; padding: 5px 12px; min-block-size: 32px; cursor: pointer; }
  button:hover { background: var(--surface-2); }
  button:focus-visible, a:focus-visible { outline: 2px solid var(--accent); outline-offset: 2px; }
  button.danger { color: var(--bad); border-color: var(--line); }
  button.danger[data-arm="1"] { background: var(--bad); color: #fff; border-color: var(--bad); }

  .log { margin: 0 0 12px; padding: 0; list-style: none; font: .82rem/1.5 var(--mono); max-block-size: 22rem; overflow-y: auto; overscroll-behavior: contain; }
  .log li { display: grid; grid-template-columns: 6.5rem 1fr; gap: 12px; padding-block: 6px; border-top: 1px solid var(--line); }
  .log li:first-child { border-top: 0; }
  .log time { color: var(--muted); }
  .log .k { overflow-wrap: anywhere; }
  .toggle { display: inline-flex; align-items: center; gap: 10px; font-size: .88rem; color: var(--muted); }
  .switch { inline-size: 38px; block-size: 22px; border-radius: 999px; padding: 0; position: relative; background: var(--line); border: 0; min-block-size: 22px; }
  .switch::after { content: ""; position: absolute; inset-block-start: 3px; inset-inline-start: 3px; inline-size: 16px; block-size: 16px; border-radius: 50%; background: #fff; transition: transform .2s ease; }
  .switch[aria-checked="true"] { background: var(--ok); }
  .switch[aria-checked="true"]::after { transform: translateX(16px); }
  .cardhead { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 6px; }
  .cardhead h2 { margin: 0; }
  .empty { color: var(--muted); padding-block: 12px 14px; font-size: .9rem; }
  [hidden] { display: none !important; }

  #toast { position: fixed; inset-inline: 16px; inset-block-end: 20px; margin-inline: auto; inline-size: fit-content; max-inline-size: calc(100vw - 32px); background: var(--ink); color: var(--bg); padding: 10px 16px; border-radius: 10px; font-size: .9rem; box-shadow: var(--shadow); }
  @media (prefers-reduced-motion: reduce) { .bar i, .switch::after { transition: none; } }
</style>
</head>
<body>
<div class="wrap">
  <header>
    <div class="brand">
      <div class="mark" aria-hidden="true">
        <svg viewBox="0 0 20 20" fill="none" stroke="#241600" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M11 2 4 11h5l-1 7 8-10h-5z"/></svg>
      </div>
      <div>
        <h1>Bluetti MQTT Bridge</h1>
        <div class="sub">Firmware 0.1.1 · <a href="/update" target="_blank">Update</a></div>
      </div>
    </div>
    <div class="chip" title="Model used for the register tables"><b id="model">-</b><span id="modelsrc"></span></div>
  </header>

  <div class="notice" id="notice" hidden>No update from the bridge, the values on this page are old.</div>

  <section class="strip" aria-label="Status">
    <div class="tile">
      <div class="lbl"><span>WiFi</span><span class="sig" id="wifiSig"><i></i><i></i><i></i><i></i></span></div>
      <div class="big num" id="tWifi">-</div>
      <div class="small" id="tWifiSub"></div>
    </div>
    <div class="tile">
      <div class="lbl"><span>MQTT</span><span class="pill" id="pMqtt" data-s="">-</span></div>
      <div class="big mono" id="tMqtt">-</div>
      <div class="small" id="tMqttSub"></div>
    </div>
    <div class="tile">
      <div class="lbl"><span>Bluetooth</span><span class="sig" id="btSig"><i></i><i></i><i></i><i></i></span></div>
      <div class="big" id="tBt">-</div>
      <div class="small" id="tBtSub"></div>
    </div>
  </section>

  <div class="grid">
    <section class="card" aria-labelledby="hSys">
      <h2 id="hSys">System</h2>
      <dl>
        <div class="row"><dt>IP address</dt><dd><span class="mono" id="ip">-</span><button class="danger" data-act="reboot">Reboot</button></dd></div>
        <div class="row"><dt>MAC</dt><dd class="mono" id="mac">-</dd></div>
        <div class="row"><dt>Uptime</dt><dd><span class="num" id="uptime">-</span></dd></div>
      </dl>
      <div class="meter">
        <div class="top"><span>CPU load</span><span class="num"><b id="cpu">-</b></span></div>
        <span class="bar" id="cpuBar" data-s="ok"><i></i></span>
        <div class="cores">
          <div><small><span>Core 0</span><span class="num" id="c0">-</span></small><span class="bar" id="c0Bar" data-s="ok"><i></i></span></div>
          <div><small><span>Core 1</span><span class="num" id="c1">-</span></small><span class="bar" id="c1Bar" data-s="ok"><i></i></span></div>
        </div>
      </div>
      <div class="meter">
        <div class="top"><span>Memory</span><span class="num"><b id="mem">-</b> <small style="color:var(--muted)" id="memOf"></small></span></div>
        <span class="bar" id="memBar" data-s="ok"><i></i></span>
        <div class="top"><span style="font-size:.8rem">Lowest free since start</span><span class="num" style="font-size:.85rem" id="memMin">-</span></div>
      </div>
    </section>

    <section class="card" aria-labelledby="hWifi">
      <h2 id="hWifi">WiFi</h2>
      <dl>
        <div class="row"><dt>Network</dt><dd><b id="ssid">-</b><button class="danger" data-act="reset">Reset WiFi</button></dd></div>
        <div class="row"><dt>Access point</dt><dd><span class="mono" id="bssid">-</span><small>channel <span id="ch">-</span></small></dd></div>
        <div class="row"><dt>Signal</dt><dd><span class="num" id="rssi">-</span><span class="pill" id="pWifi" data-s="">-</span></dd></div>
      </dl>
      <div class="meter">
        <div class="top"><span>Quality</span><span class="num"><b id="wq">-</b></span></div>
        <span class="bar" id="wqBar" data-s="ok"><i></i></span>
      </div>
    </section>

    <section class="card" aria-labelledby="hMqtt">
      <h2 id="hMqtt">MQTT</h2>
      <dl>
        <div class="row"><dt>Server</dt><dd><span class="mono" id="mqttHost">-</span><small>port <span id="mqttPort">-</span></small></dd></div>
        <div class="row"><dt>Connection</dt><dd><span class="pill" id="pMqtt2" data-s="">-</span></dd></div>
        <div class="row"><dt>Last message</dt><dd><span class="num" id="mqttLast">-</span></dd></div>
        <div class="row"><dt>Home Assistant discovery</dt><dd><span class="pill" id="pHa" data-s="">-</span></dd></div>
      </dl>
    </section>

    <section class="card" aria-labelledby="hBt">
      <h2 id="hBt">Bluetti device</h2>
      <dl>
        <div class="row"><dt>Bluetooth ID</dt><dd class="mono" id="btId">-</dd></div>
        <div class="row"><dt>Model</dt><dd><b id="btModel">-</b><small id="btSrc"></small></dd></div>
        <div class="row"><dt>Connection</dt><dd><span class="pill" id="pBt" data-s="">-</span><span class="num" id="btRssi"></span></dd></div>
        <div class="row"><dt>Last message</dt><dd><span class="num" id="btLast">-</span></dd></div>
        <div class="row"><dt>Publish errors</dt><dd><span class="num" id="btErr">-</span></dd></div>
      </dl>
    </section>

    <section class="card wide" aria-labelledby="hLog">
      <div class="cardhead">
        <h2 id="hLog">Last messages</h2>
        <span class="toggle"><span id="logCount"></span><button id="logClear" hidden>Clear</button><span>Detailed logging</span><button class="switch" id="logSwitch" role="switch" aria-checked="false" aria-label="Detailed logging"></button></span>
      </div>
      <ul class="log" id="log" hidden></ul>
      <div class="empty" id="logEmpty">Detailed logging is off. Switch it on to see each value the bridge publishes.</div>
    </section>
  </div>
  <div id="toast" role="status" hidden></div>
</div>

<script>
  var S = null;          // last status from the bridge
  var got = 0;           // when it arrived
  var rows = [];         // log messages [time, text, raw line], newest first, kept while the page is open
  var seen = {};         // raw lines already in rows

  function el(id) { return document.getElementById(id); }
  function txt(id, t) { el(id).textContent = t; }
  function html(id, h) { el(id).innerHTML = h; }
  function clamp(v, a, b) { return Math.max(a, Math.min(b, v)); }
  function sev(q) { return q >= 60 ? 'ok' : q >= 30 ? 'warn' : 'bad'; }
  function word(q) { return q >= 60 ? 'Good' : q >= 30 ? 'Weak' : 'Poor'; }
  function level(q) { return q >= 75 ? 4 : q >= 50 ? 3 : q >= 25 ? 2 : q > 0 ? 1 : 0; }
  function quality(r) { return clamp(2 * (r + 100), 0, 100); }
  function ago(s) { return s < 0 ? 'no message yet' : s < 90 ? Math.round(s) + ' s ago' : Math.round(s / 60) + ' min ago'; }
  function uptime(s) {
    var d = Math.floor(s / 86400), h = Math.floor(s % 86400 / 3600), m = Math.floor(s % 3600 / 60);
    return (d ? d + ' d ' : '') + h + ' h ' + m + ' min';
  }
  function bar(id, frac, s) {
    var b = el(id);
    b.dataset.s = s;
    b.firstElementChild.style.setProperty('--v', String(clamp(frac, 0, 1)));
  }
  function sig(id, lvl, s) {
    var box = el(id);
    box.style.setProperty('--c', 'var(--' + s + ')');
    Array.prototype.forEach.call(box.children, function (bit, i) { bit.className = i < lvl ? 'on' : ''; });
  }
  function pill(id, s, t) { var p = el(id); p.dataset.s = s; p.textContent = t; }
  function srcText(s) { return s === 'manual' ? 'chosen in setup' : s === 'auto' ? 'auto-detected' : 'config.h default'; }

  function render() {
    if (!S) return;
    var late = (Date.now() - got) / 1000;
    var q = quality(S.rssi), s = sev(q);

    txt('model', S.model); txt('modelsrc', srcText(S.model_src));
    txt('btModel', S.model); txt('btSrc', srcText(S.model_src));

    html('tWifi', q + ' &percnt;');
    txt('tWifiSub', S.ssid + ' · ' + S.rssi + ' dBm');
    sig('wifiSig', level(q), s);
    txt('rssi', S.rssi + ' dBm');
    html('wq', q + ' &percnt;');
    bar('wqBar', q / 100, s);
    pill('pWifi', s, word(q));
    txt('ssid', S.ssid); txt('bssid', S.bssid); txt('ch', S.ch);

    var m = S.mqtt_on ? 'ok' : 'bad', mt = S.mqtt_on ? 'Connected' : 'Disconnected';
    pill('pMqtt', m, mt); pill('pMqtt2', m, mt);
    txt('tMqtt', S.mqtt_host);
    txt('tMqttSub', S.mqtt_on ? 'last message ' + ago(S.mqtt_age < 0 ? -1 : S.mqtt_age + late) : 'trying to reconnect');
    txt('mqttHost', S.mqtt_host); txt('mqttPort', S.mqtt_port);
    txt('mqttLast', S.mqtt_on ? ago(S.mqtt_age < 0 ? -1 : S.mqtt_age + late) : 'not connected');
    pill('pHa', S.ha ? 'ok' : '', S.ha ? 'On' : 'Off');

    var bq = S.bt_on ? quality(S.bt_rssi) : 0, bs = S.bt_on ? sev(bq) : 'bad';
    txt('tBt', S.bt_on ? 'Connected' : 'Not connected');
    txt('tBtSub', S.bt_on ? S.bt_rssi + ' dBm · last message ' + ago(S.bt_age < 0 ? -1 : S.bt_age + late) : 'scanning for ' + S.bt_id);
    sig('btSig', S.bt_on ? level(bq) : 0, bs);
    pill('pBt', S.bt_on ? 'ok' : 'bad', S.bt_on ? 'Connected' : 'Disconnected');
    txt('btRssi', S.bt_on ? S.bt_rssi + ' dBm' : 'no link');
    txt('btLast', S.bt_on ? ago(S.bt_age < 0 ? -1 : S.bt_age + late) : 'not connected');
    txt('btId', S.bt_id);
    txt('btErr', String(S.errors));

    txt('ip', S.ip); txt('mac', S.mac);
    txt('uptime', uptime(S.up + late));
    var avg = Math.round((S.cpu0 + S.cpu1) / 2);
    html('cpu', avg + ' &percnt;');
    bar('cpuBar', avg / 100, avg > 85 ? 'bad' : avg > 65 ? 'warn' : 'ok');
    html('c0', S.cpu0 + ' &percnt;'); html('c1', S.cpu1 + ' &percnt;');
    bar('c0Bar', S.cpu0 / 100, 'ok'); bar('c1Bar', S.cpu1 / 100, 'ok');
    var used = S.heap_total - S.heap_free, f = used / S.heap_total;
    txt('mem', Math.round(used / 1024) + ' KB used');
    txt('memOf', 'of ' + Math.round(S.heap_total / 1024) + ' KB');
    txt('memMin', Math.round(S.heap_min / 1024) + ' KB');
    bar('memBar', f, f > .9 ? 'bad' : f > .75 ? 'warn' : 'ok');

    el('logSwitch').setAttribute('aria-checked', String(!!S.logging));
    el('notice').hidden = late < 20;
  }

  function apply(j) { S = j; got = Date.now(); render(); }

  function load() {
    return fetch('/status').then(function (r) { return r.json(); }).then(apply);
  }

  function drawLog() {
    var on = S && S.logging;
    var ul = el('log');
    ul.hidden = !on;
    el('logEmpty').hidden = !!on;
    el('logClear').hidden = !on;
    txt('logCount', on ? rows.length + ' messages' : '');
    var frag = document.createDocumentFragment();
    rows.forEach(function (row) {
      var li = document.createElement('li');
      var t = document.createElement('time'); t.textContent = row[0];
      var k = document.createElement('span'); k.className = 'k'; k.textContent = row[1];
      li.appendChild(t); li.appendChild(k); frag.appendChild(li);
    });
    // keep what the reader is looking at in place when new lines are added at the top
    var top = ul.scrollTop, height = ul.scrollHeight;
    ul.innerHTML = '';
    ul.appendChild(frag);
    if (top > 0) ul.scrollTop = top + (ul.scrollHeight - height);
  }

  function two(n) { return n < 10 ? '0' + n : String(n); }
  function hms(ms) {
    var s = Math.floor(ms / 1000);
    return Math.floor(s / 3600) + ':' + two(Math.floor(s % 3600 / 60)) + ':' + two(s % 60);
  }

  // the bridge sends its message list as <p>time: text</p> lines, newest last (time is the uptime in ms).
  // It only keeps the last few, so new lines are added to the list kept here.
  var MAX_ROWS = 300;
  function parseLog(data) {
    var fresh = [];
    data.split('</p>').forEach(function (part) {
      var line = part.replace('<p>', '').trim();
      if (!line || seen[line]) return;
      seen[line] = true;
      var m = line.match(/^(\d+):\s*(.*)$/);
      fresh.push(m ? [hms(Number(m[1])), m[2], line] : ['', line, line]);
    });
    if (!fresh.length) return;
    rows = fresh.reverse().concat(rows);
    if (rows.length > MAX_ROWS) {
      rows.slice(MAX_ROWS).forEach(function (row) { delete seen[row[2]]; });
      rows = rows.slice(0, MAX_ROWS);
    }
    drawLog();
  }

  function loadLog() {
    return fetch('/log').then(function (r) { return r.text(); }).then(parseLog).catch(function () {});
  }

  function toast(t) {
    var n = el('toast'); n.textContent = t; n.hidden = false;
    clearTimeout(toast.h); toast.h = setTimeout(function () { n.hidden = true; }, 6000);
  }

  function waitForBridge() {
    var tries = 0;
    var t = setInterval(function () {
      tries++;
      fetch('/status').then(function (r) { if (r.ok) { clearInterval(t); location.reload(); } }).catch(function () {});
      if (tries > 40) clearInterval(t);
    }, 3000);
  }

  document.addEventListener('click', function (e) {
    var b = e.target.closest('button');
    if (!b) return;
    if (b.dataset.act) {
      if (b.dataset.arm === '1') {
        b.dataset.arm = ''; b.textContent = b.dataset.label;
        if (b.dataset.act === 'reboot') {
          fetch('/rebootDevice').catch(function () {});
          toast('Rebooting. This page reloads when the bridge is back.');
          setTimeout(waitForBridge, 4000);
        } else {
          fetch('/resetConfig').catch(function () {});
          toast('WiFi settings cleared. Connect to the WiFi network Bluetti_ESP32 and open 192.168.4.1 to set up again.');
        }
      } else {
        b.dataset.label = b.textContent; b.dataset.arm = '1'; b.textContent = 'Tap again to confirm';
        setTimeout(function () { if (b.dataset.arm === '1') { b.dataset.arm = ''; b.textContent = b.dataset.label; } }, 4000);
      }
    }
    if (b.id === 'logClear') { rows = []; drawLog(); }
    if (b.id === 'logSwitch') {
      fetch('/switchLogging').then(function (r) { return r.json(); }).then(function (j) { apply(j); rows = []; seen = {}; drawLog(); if (j.logging) loadLog(); }).catch(function () {});
    }
  });

  if (window.EventSource) {
    var source = new EventSource('/events');
    source.addEventListener('status', function (e) { try { apply(JSON.parse(e.data)); } catch (x) {} }, false);
  }
  setInterval(render, 1000);
  setInterval(function () { if (S && S.logging) loadLog(); }, 2000);
  load().then(function () { if (S.logging) loadLog(); }).catch(function () {});
</script>
</body>
</html>
)rawliteral";
